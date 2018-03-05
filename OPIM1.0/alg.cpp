#include "stdafx.h"

double Alg::max_cover_lazy(const int targetSize, const int mode)
{
	// mode: optimization mode.
	// 0->no optimization, 
	// 1->optimization with the upper bound in the last round,
	// 2->optimization with minimum upper bound among all rounds [Default].
	__boundLast = DBL_MAX, __boundMin = DBL_MAX;
	FRset coverage(__numV, 0);
	size_t maxDeg = 0;
	for (auto i = __numV; i--;)
	{
		const auto deg = __hyperG._FRsets[i].size();
		coverage[i] = deg;
		if (deg > maxDeg) maxDeg = deg;
	}
	RRsets degMap(maxDeg + 1); // degMap: map degree to the nodes with this degree
	for (auto i = __numV; i--;)
	{
		if (coverage[i] == 0) continue;
		degMap[coverage[i]].push_back(i);
	}
	size_t sumInf = 0;

	// check if an edge is removed
	std::vector<bool> edgeMark(__numRRsets, false);

	__vecSeed.clear();
	for (auto deg = maxDeg + 1; deg--;)
	{
		auto& vecNode = degMap[deg];
		for (auto idx = vecNode.size(); idx--;)
		{
			auto argmaxIdx = vecNode[idx];
			const auto currDeg = coverage[argmaxIdx];
			if (deg > currDeg)
			{
				degMap[currDeg].push_back(argmaxIdx);
				continue;
			}
			if (mode == 2 || (mode == 1 && __vecSeed.size() == targetSize))
			{
				// Find upper bound
				auto topk = targetSize;
				auto degBound = deg;
				FRset vecBound(targetSize);
				// Initialize vecBound
				auto idxBound = idx + 1;
				while (topk && idxBound--)
				{
					vecBound[--topk] = coverage[degMap[degBound][idxBound]];
				}
				while (topk && --degBound)
				{
					idxBound = degMap[degBound].size();
					while (topk && idxBound--)
					{
						vecBound[--topk] = coverage[degMap[degBound][idxBound]];
					}
				}
				make_min_heap(vecBound);

				// Find the top-k marginal coverage
				auto flag = topk == 0;
				while (flag && idxBound--)
				{
					const auto currDegBound = coverage[degMap[degBound][idxBound]];
					if (vecBound[0] >= degBound)
					{
						flag = false;
					}
					else if (vecBound[0] < currDegBound)
					{
						min_heap_replace_min_value(vecBound, currDegBound);
					}
				}
				while (flag && --degBound)
				{
					idxBound = degMap[degBound].size();
					while (flag && idxBound--)
					{
						const auto currDegBound = coverage[degMap[degBound][idxBound]];
						if (vecBound[0] >= degBound)
						{
							flag = false;
						}
						else if (vecBound[0] < currDegBound)
						{
							min_heap_replace_min_value(vecBound, currDegBound);
						}
					}
				}
				__boundLast = double(accumulate(vecBound.begin(), vecBound.end(), size_t(0)) + sumInf);
				if (__boundMin > __boundLast) __boundMin = __boundLast;
			}
			if (__vecSeed.size() >= targetSize)
			{
				// Top-k influential nodes constructed
				__boundMin *= 1.0 * __numV / __numRRsets;
				__boundLast *= 1.0 * __numV / __numRRsets;
				const auto finalInf = 1.0 * sumInf * __numV / __numRRsets;
				std::cout << "  >>>[greedy-lazy] influence: " << finalInf << ", min-bound: " << __boundMin <<
					", last-bound: " << __boundLast << '\n';
				return finalInf;
			}
			sumInf += currDeg;
			__vecSeed.push_back(argmaxIdx);
			coverage[argmaxIdx] = 0;
			for (auto edgeIdx : __hyperG._FRsets[argmaxIdx])
			{
				if (edgeMark[edgeIdx]) continue;
				edgeMark[edgeIdx] = true;
				for (auto nodeIdx : __hyperG._RRsets[edgeIdx])
				{
					if (coverage[nodeIdx] == 0) continue; // This node is seed, skip
					coverage[nodeIdx]--;
				}
			}
		}
		degMap.pop_back();
	}
	return 1.0 * __numV; // All RR sets are covered.
}

double Alg::max_cover_topk(const int targetSize)
{
	FRset coverage(__numV, 0);
	size_t maxDeg = 0;
	for (auto i = __numV; i--;)
	{
		const auto deg = __hyperG._FRsets[i].size();
		coverage[i] = deg;
		if (deg > maxDeg) maxDeg = deg;
	}
	RRsets degMap(maxDeg + 1); // degMap: map degree to the nodes with this degree
	for (auto i = __numV; i--;)
	{
		//if (coverage[i] == 0) continue;
		degMap[coverage[i]].push_back(i);
	}
	Nodelist sortedNode(__numV); // sortedNode: record the sorted nodes in ascending order of degree
	Nodelist nodePosition(__numV); // nodePosition: record the position of each node in the sortedNode
	Nodelist degreePosition(maxDeg + 2); // degreePosition: the start position of each degree in sortedNode
	uint32_t idxSort = 0;
	size_t idxDegree = 0;
	for (auto& nodes : degMap)
	{
		degreePosition[idxDegree + 1] = degreePosition[idxDegree] + (uint32_t)nodes.size();
		idxDegree++;
		for (auto& node : nodes)
		{
			nodePosition[node] = idxSort;
			sortedNode[idxSort++] = node;
		}
	}
	// check if an edge is removed
	std::vector<bool> edgeMark(__numRRsets, false);
	// record the total of top-k marginal gains
	size_t sumTopk = 0;
	for (auto deg = maxDeg + 1; deg--;)
	{
		if (degreePosition[deg] <= __numV - targetSize)
		{
			sumTopk += deg * (degreePosition[deg + 1] - (__numV - targetSize));
			break;
		}
		sumTopk += deg * (degreePosition[deg + 1] - degreePosition[deg]);
	}
	__boundMin = 1.0 * sumTopk;
	__vecSeed.clear();
	size_t sumInf = 0;
	/*
	* sortedNode: position -> node
	* nodePosition: node -> position
	* degreePosition: degree -> position (start position of this degree)
	* coverage: node -> degree
	* e.g., swap the position of a node with the start position of its degree
	* swap(sortedNode[nodePosition[node]], sortedNode[degreePosition[coverage[node]]])
	*/
	for (auto k = targetSize; k--;)
	{
		const auto seed = sortedNode.back();
		sortedNode.pop_back();
		const auto newNumV = sortedNode.size();
		sumTopk += coverage[sortedNode[newNumV - targetSize]] - coverage[seed];
		sumInf += coverage[seed];
		__vecSeed.push_back(seed);
		coverage[seed] = 0;
		for (auto edgeIdx : __hyperG._FRsets[seed])
		{
			if (edgeMark[edgeIdx]) continue;
			edgeMark[edgeIdx] = true;
			for (auto nodeIdx : __hyperG._RRsets[edgeIdx])
			{
				if (coverage[nodeIdx] == 0) continue; // This node is seed, skip
				const auto currPos = nodePosition[nodeIdx]; // The current position
				const auto currDeg = coverage[nodeIdx]; // The current degree
				const auto startPos = degreePosition[currDeg]; // The start position of this degree
				const auto startNode = sortedNode[startPos]; // The node with the start position
				// Swap this node to the start position with the same degree, and update their positions in nodePosition
				std::swap(sortedNode[currPos], sortedNode[startPos]);
				nodePosition[nodeIdx] = startPos;
				nodePosition[startNode] = currPos;
				// Increase the start position of this degree by 1, and decrease the degree of this node by 1
				degreePosition[currDeg]++;
				coverage[nodeIdx]--;
				// If the start position of this degree is in top-k, reduce topk by 1
				if (startPos >= newNumV - targetSize) sumTopk--;
			}
		}
		__boundLast = 1.0 * (sumInf + sumTopk);
		if (__boundMin > __boundLast) __boundMin = __boundLast;
	}
	__boundMin *= 1.0 * __numV / __numRRsets;
	__boundLast *= 1.0 * __numV / __numRRsets;
	const auto finalInf = 1.0 * sumInf * __numV / __numRRsets;
	std::cout << "  >>>[greedy-topk] influence: " << finalInf << ", min-bound: " << __boundMin <<
		", last-bound: " << __boundLast << '\n';
	return finalInf;
}

double Alg::max_cover(const int targetSize, const int mode)
{
	if (targetSize >= 1000) return max_cover_topk(targetSize);
	return max_cover_lazy(targetSize, mode);
}

void Alg::set_cascade_model(const CascadeModel model)
{
	__hyperG.set_cascade_model(model);
	__hyperGVldt.set_cascade_model(model);
}

double Alg::effic_inf_valid_algo()
{
	return effic_inf_valid_algo(__vecSeed);
}

double Alg::effic_inf_valid_algo(const Nodelist vecSeed)
{
	Timer EvalTimer;
	std::cout << "  >>>Evaluating influence in [0.99,1.01]*EPT with prob. 99.9%...\n";
	const auto inf = __hyperG.effic_inf_valid_algo(vecSeed);
	//const auto inf = __hyperGVldt.effic_inf_valid_algo(vecSeed);
	std::cout << "  >>>Down! influence: " << inf << ", time used (sec): " << EvalTimer.get_total_time() << '\n';
	return inf;
}

double Alg::opim(const int targetSize, const size_t numRRsets, const double delta, const int mode)
{
	Timer timerOPIM("OPIM");
	const double e = exp(1);
	const double approx = 1 - 1.0 / e;
	const double a1 = log(2.0 / delta);
	const double a2 = log(2.0 / delta);
	__hyperG.build_n_RRsets(numRRsets / 2); // R1
	__hyperGVldt.build_n_RRsets(numRRsets / 2); // R2
	__numRRsets = __hyperG.get_RR_sets_size();
	const auto time1 = timerOPIM.get_operation_time();
	const auto infSelf = max_cover(targetSize, mode);
	const auto time2 = timerOPIM.get_operation_time();
	const auto infVldt = __hyperGVldt.self_inf_cal(__vecSeed);
	const auto degVldt = infVldt * __numRRsets / __numV;
	auto upperBound = infSelf / approx;
	if (mode == 1) upperBound = __boundLast;
	else if (mode == 2) upperBound = __boundMin;
	const auto upperDegOPT = upperBound * __numRRsets / __numV;
	const auto lowerSelect = pow2(sqrt(degVldt + a1 * 2.0 / 9.0) - sqrt(a1 / 2.0)) - a1 / 18.0;
	const auto upperOPT = pow2(sqrt(upperDegOPT + a2 / 2.0) + sqrt(a2 / 2.0));
	const auto approxOPIM = lowerSelect / upperOPT;
	//guaranteeOpt = opt_error(degVldt, degSelfUpper, delta);
	__tRes.set_approximation(approxOPIM);
	__tRes.set_running_time(timerOPIM.get_total_time());
	__tRes.set_influence(infVldt);
	__tRes.set_influence_original(infSelf);
	__tRes.set_seed_vec(__vecSeed);
	__tRes.set_RR_sets_size(__numRRsets * 2);
	std::cout << "==>OPIM approx. (max-cover): " << approxOPIM << " (" << infSelf / upperBound << ")\n";
	std::cout << "==>Time for RR sets and greedy: " << time1 << ", " << time2 << '\n';
	__tRes.set_influence(effic_inf_valid_algo());
	return approxOPIM;
}

double Alg::opimc(const int targetSize, const double epsilon, const double delta, const int mode)
{
	Timer timerOPIMC("OPIM-C");
	const double e = exp(1);
	const double approx = 1 - 1.0 / e;
	const double alpha = sqrt(log(6.0 / delta));
	const double beta = sqrt((1 - 1 / e) * (logcnk(__numV, targetSize) + log(6.0 / delta)));
	const auto numRbase = size_t(2.0 * pow2((1 - 1 / e) * alpha + beta));
	const auto maxNumR = size_t(2.0 * __numV * pow2((1 - 1 / e) * alpha + beta) / targetSize / pow2(epsilon)) + 1;
	const auto numIter = (size_t)log2(maxNumR / numRbase) + 1;
	const double a1 = log(numIter * 3.0 / delta);
	const double a2 = log(numIter * 3.0 / delta);
	double time1 = 0.0, time2 = 0.0;
	for (auto idx = 0; idx < numIter; idx++)
	{
		const auto numR = numRbase << idx;
		timerOPIMC.get_operation_time();
		//dsfmt_gv_init_gen_rand(idx);
		__hyperG.build_n_RRsets(numR); // R1
		//dsfmt_gv_init_gen_rand(idx + 1000000);
		__hyperGVldt.build_n_RRsets(numR); // R2
		__numRRsets = __hyperG.get_RR_sets_size();
		time1 += timerOPIMC.get_operation_time();
		const auto infSelf = max_cover(targetSize, mode);
		time2 += timerOPIMC.get_operation_time();
		const auto infVldt = __hyperGVldt.self_inf_cal(__vecSeed);
		const auto degVldt = infVldt * __numRRsets / __numV;
		auto upperBound = infSelf / approx;
		if (mode == 1) upperBound = __boundLast;
		else if (mode == 2) upperBound = __boundMin;
		const auto upperDegOPT = upperBound * __numRRsets / __numV;
		const auto lowerSelect = pow2(sqrt(degVldt + a1 * 2.0 / 9.0) - sqrt(a1 / 2.0)) - a1 / 18.0;
		const auto upperOPT = pow2(sqrt(upperDegOPT + a2 / 2.0) + sqrt(a2 / 2.0));
		const auto approxOPIMC = lowerSelect / upperOPT;
		//guaranteeOpt = opt_error(degVldt, degSelfUpper, delta);
		std::cout << " -->OPIM-C (" << idx + 1 << "/" << numIter << ") approx. (max-cover): " << approxOPIMC <<
			" (" << infSelf / upperBound << "), #RR sets: " << __numRRsets << '\n';
		// Check whether the requirement is satisfied
		if (approxOPIMC >= approx - epsilon)
		{
			__tRes.set_approximation(approxOPIMC);
			__tRes.set_running_time(timerOPIMC.get_total_time());
			__tRes.set_influence(infVldt);
			__tRes.set_influence_original(infSelf);
			__tRes.set_seed_vec(__vecSeed);
			__tRes.set_RR_sets_size(__numRRsets * 2);
			std::cout << "==>Influence via R2: " << infVldt << ", time: " << __tRes.get_running_time() << '\n';
			std::cout << "==>Time for RR sets and greedy: " << time1 << ", " << time2 << '\n';
			__tRes.set_influence(effic_inf_valid_algo());
			return __tRes.get_influence();
		}
	}
	return 0.0;
}
