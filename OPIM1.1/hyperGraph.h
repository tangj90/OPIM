#pragma once

class HyperGraph
{
private:
	/// __numV: number of nodes in the graph.
	uint32_t __numV;
	/// __numE: number of edges in the graph.
	size_t __numE;
	/// __numRRsets: number of RR sets.
	size_t __numRRsets = 0;
	std::vector<bool> __vecVisitBool;
	Nodelist __vecVisitNode;

	/// Initialization
	void init_hypergraph()
	{
		__numV = (uint32_t)_graph.size();
		for (auto& nbrs : _graph) __numE += nbrs.size();
		_FRsets = FRsets(__numV);
		__vecVisitBool = std::vector<bool>(__numV);
		__vecVisitNode = Nodelist(__numV);
	}

public:
	/// _graph: reverse graph
	const Graph& _graph;
	/// _FRsets: forward cover sets, _FRsets[i] is the node sets that node i can reach
	FRsets _FRsets;
	/// _RRsets: reverse cover sets, _RRsets[i] is the node set that can reach node i
	RRsets _RRsets;
	/// _cascadeModel: the cascade model, default is IC
	CascadeModel _cascadeModel = IC;

	explicit HyperGraph(const Graph& graph) : _graph(graph)
	{
		init_hypergraph();
	}

	/// Set cascade model
	void set_cascade_model(const CascadeModel model)
	{
		_cascadeModel = model;
	}

	/// Returns the number of nodes in the graph.
	uint32_t get_nodes() const
	{
		return __numV;
	}

	/// Returns the number of edges in the graph.
	size_t get_edges() const
	{
		return __numE;
	}

	/// Returns the number of RR sets in the graph.
	size_t get_RR_sets_size() const
	{
		return __numRRsets;
	}

	/// Get out degree
	std::vector<size_t> get_out_degree() const
	{
		std::vector<size_t> outDeg(__numV);
		for (auto& nbrs : _graph)
		{
			for (auto& nbr : nbrs)
			{
				outDeg[nbr.first]++;
			}
		}
		return outDeg;
	}

	/// Generate a set of n RR sets
	void build_n_RRsets(const size_t numSamples)
	{
		if (numSamples > SIZE_MAX)
		{
			std::cout << "Error:R too large" << std::endl;
			exit(1);
		}
		const auto prevSize = __numRRsets;
		__numRRsets = __numRRsets > numSamples ? __numRRsets : numSamples;
		for (auto i = prevSize; i < numSamples; i++)
		{
			build_one_RRset(dsfmt_gv_genrand_uint32_range(__numV), i);
		}
	}

	/// Generate one RR set
	void build_one_RRset(const uint32_t uStart, const size_t hyperIdx)
	{
		size_t numVisitNode = 0, currIdx = 0;
		_FRsets[uStart].push_back(hyperIdx);
		__vecVisitNode[numVisitNode++] = uStart;
		__vecVisitBool[uStart] = true;
		while (currIdx < numVisitNode)
		{
			const auto expand = __vecVisitNode[currIdx++];
			if (_cascadeModel == IC)
			{
				for (auto& nbr : _graph[expand])
				{
					const auto nbrId = nbr.first;
					if (__vecVisitBool[nbrId])
						continue;
					const auto randDouble = dsfmt_gv_genrand_open_close();
					if (randDouble > nbr.second)
						continue;
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
					_FRsets[nbrId].push_back(hyperIdx);
				}
			}
			else if (_cascadeModel == LT)
			{
				if (_graph[expand].size() == 0)
					continue;
				const auto nextNbrIdx = gen_random_node_by_weight_LT(_graph[expand]);
				if (nextNbrIdx >= _graph[expand].size()) break; // No element activated
				const auto nbrId = _graph[expand][nextNbrIdx].first;
				if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
				__vecVisitNode[numVisitNode++] = nbrId;
				__vecVisitBool[nbrId] = true;
				_FRsets[nbrId].push_back(hyperIdx);
			}
		}
		for (int i = 0; i < numVisitNode; i++) __vecVisitBool[__vecVisitNode[i]] = false;
		_RRsets.push_back(RRset(__vecVisitNode.begin(), __vecVisitNode.begin() + numVisitNode));
	}

	/// Evaluate the influence spread of a seed set on current generated RR sets
	double self_inf_cal(const Nodelist& vecSeed)
	{
		std::vector<bool> vecBoolVst = std::vector<bool>(__numRRsets);
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;
		for (auto seed : vecSeed)
		{
			for (auto node : _FRsets[seed])
			{
				vecBoolVst[node] = true;
			}
		}
		return 1.0 * std::count(vecBoolVst.begin(), vecBoolVst.end(), true) * __numV / __numRRsets;
	}

	/// Efficiently estimate the influence spread with sampling error epsilon within probability 1-delta
	double effic_inf_valid_algo(const Nodelist& vecSeed, const double delta = 1e-3, const double eps = 0.01)
	{
		const double c = 2.0 * (exp(1.0) - 2.0);
		const double LambdaL = 1.0 + 2.0 * c * (1.0 + eps) * log(2.0 / delta) / (eps * eps);
		size_t numHyperEdge = 0;
		size_t numCoverd = 0;
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;

		while (numCoverd < LambdaL)
		{
			numHyperEdge++;
			size_t numVisitNode = 0, currIdx = 0;
			const auto uStart = dsfmt_gv_genrand_uint32_range(__numV);
			if (vecBoolSeed[uStart])
			{
				// Stop, this sample is covered
				numCoverd++;
				continue;
			}
			__vecVisitNode[numVisitNode++] = uStart;
			__vecVisitBool[uStart] = true;
			while (currIdx < numVisitNode)
			{
				const auto expand = __vecVisitNode[currIdx++];
				if (_cascadeModel == IC)
				{
					for (auto& nbr : _graph[expand])
					{
						const auto nbrId = nbr.first;
						if (__vecVisitBool[nbrId])
							continue;
						const auto randDouble = dsfmt_gv_genrand_open_close();
						if (randDouble > nbr.second)
							continue;
						if (vecBoolSeed[nbrId])
						{
							// Stop, this sample is covered
							numCoverd++;
							goto postProcess;
						}
						__vecVisitNode[numVisitNode++] = nbrId;
						__vecVisitBool[nbrId] = true;
					}
				}
				else if (_cascadeModel == LT)
				{
					if (_graph[expand].size() == 0)
						continue;
					const auto nextNbrIdx = gen_random_node_by_weight_LT(_graph[expand]);
					if (nextNbrIdx >= _graph[expand].size()) break; // No element activated
					const auto nbrId = _graph[expand][nextNbrIdx].first;
					if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
					if (vecBoolSeed[nbrId])
					{
						// Stop, this sample is covered
						numCoverd++;
						goto postProcess;
					}
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
				}
			}
		postProcess:
			for (auto i = 0; i < numVisitNode; i++)
				__vecVisitBool[__vecVisitNode[i]] = false;
		}
		return 1.0 * numCoverd * __numV / numHyperEdge;
	}

	/// Efficiently evaluate the influence spread of a seed set with a given number of RR sets to test
	double effic_inf_valid_algo_with_samplesize(const std::vector<uint32_t>& vecSeed, const size_t numSamples)
	{
		size_t numHyperEdge = 0;
		size_t numCoverd = 0;
		std::vector<bool> vecBoolSeed(__numV);
		for (auto seed : vecSeed) vecBoolSeed[seed] = true;
		while (++numHyperEdge < numSamples)
		{
			size_t numVisitNode = 0, currIdx = 0;
			const auto uStart = dsfmt_gv_genrand_uint32_range(__numV);
			if (vecBoolSeed[uStart])
			{
				// Stop, this sample is covered
				numCoverd++;
				continue;
			}
			__vecVisitNode[numVisitNode++] = uStart;
			__vecVisitBool[uStart] = true;
			while (currIdx < numVisitNode)
			{
				const auto expand = __vecVisitNode[currIdx++];
				if (_cascadeModel == IC)
				{
					for (auto& nbr : _graph[expand])
					{
						const auto nbrId = nbr.first;
						if (__vecVisitBool[nbrId])
							continue;
						const auto randDouble = dsfmt_gv_genrand_open_close();
						if (randDouble > nbr.second)
							continue;
						if (vecBoolSeed[nbrId])
						{
							// Stop, this sample is covered
							numCoverd++;
							goto postProcess;
						}
						__vecVisitNode[numVisitNode++] = nbrId;
						__vecVisitBool[nbrId] = true;
					}
				}
				else if (_cascadeModel == LT)
				{
					if (_graph[expand].size() == 0)
						continue;
					const auto nextNbrIdx = gen_random_node_by_weight_LT(_graph[expand]);
					if (nextNbrIdx >= _graph[expand].size()) break; // No element activated
					const auto nbrId = _graph[expand][nextNbrIdx].first;
					if (__vecVisitBool[nbrId]) break; // Stop, no further node activated
					if (vecBoolSeed[nbrId])
					{
						// Stop, this sample is covered
						numCoverd++;
						goto postProcess;
					}
					__vecVisitNode[numVisitNode++] = nbrId;
					__vecVisitBool[nbrId] = true;
				}
			}
		postProcess:
			for (auto i = 0; i < numVisitNode; i++)
				__vecVisitBool[__vecVisitNode[i]] = false;
		}
		return 1.0 * numCoverd * __numV / numHyperEdge;
	}

	/// Refresh the hypergraph
	void refresh_hypergraph()
	{
		for (auto i = __numRRsets; i--;)
		{
			RRset().swap(_RRsets[i]);
		}
		RRsets().swap(_RRsets);
		for (auto i = __numV; i--;)
		{
			FRset().swap(_FRsets[i]);
		}
		__numRRsets = 0;
	}

	/// Release memory
	void release_memory()
	{
		refresh_hypergraph();
		std::vector<bool>().swap(__vecVisitBool);
		Nodelist().swap(__vecVisitNode);
		FRsets().swap(_FRsets);
	}
};

using THyperGraph = HyperGraph;
using PHyperGraph = std::shared_ptr<THyperGraph>;
