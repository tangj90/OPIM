#pragma once

class Alg
{
private:
	/// __numV: number of nodes in the graph.
	uint32_t __numV;
	/// __numE: number of edges in the graph.
	size_t __numE;
	/// __numRRsets: number of RR sets.
	size_t __numRRsets = 0;
	/// Upper bound in the last round for __mode=1.
	double __boundLast = DBL_MAX;
	/// The minimum upper bound among all rounds for __model=2.
	double __boundMin = DBL_MAX;
	/// Two hyper-graphs, one is used for selecting seeds and the other is used for validating influence. 
	THyperGraph __hyperG, __hyperGVldt;
	/// Result object.
	TResult& __tRes;
	/// Seed set.
	Nodelist __vecSeed;
	/// Maximum coverage by lazy updating.
	double max_cover_lazy(const int targetSize, const int mode = 2);
	/// Maximum coverage by maintaining the top-k marginal coverage.
	double max_cover_topk(const int targetSize);
	/// Maximum coverage.
	double max_cover(const int targetSize, const int mode = 2);
public:
	Alg(const Graph& graph, TResult& tRes) : __hyperG(graph), __hyperGVldt(graph), __tRes(tRes)
	{
		__numV = __hyperG.get_nodes();
		__numE = __hyperG.get_edges();
	}
	~Alg()
	{
	}
	/// Set cascade model.
	void set_cascade_model(const CascadeModel model);
	/// Evaluate influence spread for the seed set constructed
	double effic_inf_valid_algo();
	/// Evaluate influence spread for a given seed set
	double effic_inf_valid_algo(const Nodelist vecSeed);
	/// OPIM: return the approximation guarantee alpha for the greedy algorithm when a given number of RR sets are generated.
	double opim(const int targetSize, const size_t numRRsets, const double delta, const int mode = 2);
	/// OPIM-C: return (epsilon, delta)-approximate solution for influence maximization.  
	double opimc(const int targetSize, const double epsilon, const double delta, const int mode = 2);
};

using TAlg = Alg;
using PAlg = std::shared_ptr<TAlg>;
