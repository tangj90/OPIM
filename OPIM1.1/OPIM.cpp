/**
* @file OPIM.cpp
* @brief This project implements the OPIM and OPIM-C for the following paper:
* Jing Tang, Xueyan Tang, Xiaokui Xiao, Junsong Yuan, "Online Processing Algorithms for Influence Maximization," in Proc. ACM SIGMOD, 2018.
*
* @author Jing Tang (Nanyang Technological University)
*
* Copyright (C) 2018 Jing Tang and Nanyang Technological University. All rights reserved.
*
*/

#include "stdafx.h"
#include "SFMT/dSFMT/dSFMT.c"
#include "alg.cpp"

int main(int argc, char* argv[])
{
	// Randomize the seed for generating random numbers
	dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
	const TArgument Arg(argc, argv);
	const std::string infilename = Arg._dir + "/" + Arg._graphname;
	if (Arg._func == 0 || Arg._func == 2)
	{
		// Format the graph
		GraphBase::format_graph(infilename, Arg._mode);
		if (Arg._func == 0) return 1;
	}

	std::cout << "---The Begin of " << Arg._outFileName << "---\n";
	Timer mainTimer("main");
	
	// Load the reverse graph
	Graph graph = GraphBase::load_graph(infilename, true, Arg._probDist, Arg._probEdge);
	if (Arg._model == LT)
	{
		// Normalize the propagation probabilities in accumulation format for LT cascade model for quickly generating RR sets
		to_normal_accum_prob(graph);
	}
	// Initialize a result object to record the results
	TResult tRes;
	TAlg tAlg(graph, tRes);
	tAlg.set_cascade_model(Arg._model); // Set propagation model

	std::cout << "  ==>Graph loaded for RIS! total time used (sec): " << mainTimer.get_total_time() << '\n';
	int mode = 2; // Default is to use the minimum upper bound among all the rounds
	if (Arg._mode == "0" || Arg._mode == "vanilla")
	{
		mode = 0;
	}
	else if (Arg._mode == "1" || Arg._mode == "last")
	{
		mode = 1;
	}
	auto delta = Arg._delta;
	if (delta < 0) delta = 1.0 / graph.size();
	if (Arg._algName == "opim-c" || Arg._algName == "OPIM-C")
	{
		tAlg.opimc(Arg._seedsize, Arg._eps, delta, mode);
	}
	else if (Arg._algName == "opim" || Arg._algName == "OPIM")
	{
		tAlg.opim(Arg._seedsize, Arg._samplesize, delta, mode);
	}
	TIO::write_result(Arg._outFileName, tRes, Arg._resultFolder);
	TIO::write_order_seeds(Arg._outFileName, tRes, Arg._resultFolder);
	std::cout << "---The End of " << Arg._outFileName << "---\n";
	return 0;
}
