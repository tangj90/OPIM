#pragma once


class Argument
{
public:
	int _func = 1; // Function parameter. 0: format graph, 1: maximize profit, 2: format graph and then maximize profit.
	int _seedsize = 50; // The number of nodes to be selected. Default is 50.
	size_t _samplesize = 1000; // The number of RR sets to be generated.
	float _probEdge = float(0.1); // For the UNI setting, every edge has the same diffusion probability.
	double _eps = 0.1; // Error threshold 1-1/e-epsilon. 
	double _delta = -1.0; // Failure probability delta. Default is 1/#nodes.
	CascadeModel _model = IC; // Cascade models: IC, LT. Default is IC.
	std::string _graphname = "facebook"; // Graph name. Default is "facebook".
	std::string _mode = "g"; // Format graph --> g: graph only [default, using WC], w: with edge property.
							 // OPIM or OPIM-C --> 0: vanilla, 1: last-bound, 2: min-bound [default].  
	std::string _dir = "graphInfo"; // Directory
	std::string _resultFolder = "result"; // Result folder. Default is "test".
	std::string _algName = "OPIM"; // Algorithm. Default is oneHop.
	std::string _probDist = "load"; // Probability distribution for diffusion model. Option: load, WC, TR, UNI. Default is loaded from the file.
	std::string _outFileName; // File name of the result
	
	Argument(int argc, char* argv[])
	{
		std::string param, value;
		for (int ind = 1; ind < argc; ind++)
		{
			if (argv[ind][0] != '-') break;
			std::stringstream sstr(argv[ind]);
			getline(sstr, param, '=');
			getline(sstr, value, '=');
			if (!param.compare("-func")) _func = stoi(value);
			else if (!param.compare("-seedsize")) _seedsize = stoi(value);
			else if (!param.compare("-samplesize")) _samplesize = stoull(value);
			else if (!param.compare("-pedge")) _probEdge = stof(value);
			else if (!param.compare("-eps")) _eps = stod(value);
			else if (!param.compare("-delta")) _delta = stod(value);
			else if (!param.compare("-model")) _model = value == "LT" ? LT : IC;
			else if (!param.compare("-gname")) _graphname = value;
			else if (!param.compare("-mode")) _mode = value;
			else if (!param.compare("-dir")) _dir = value;
			else if (!param.compare("-outpath")) _resultFolder = value;
			else if (!param.compare("-alg")) _algName = value;
			else if (!param.compare("-pdist")) _probDist = value;
		}
		std::string postfix = "_minBound"; // Default is to use the minimum upper bound among all the rounds
		if (_mode == "0" || _mode == "vanilla")
		{
			postfix = "_vanilla";
		}
		else if (_mode == "1" || _mode == "last")
		{
			postfix = "_lastBound";
		}
		_outFileName = TIO::get_out_file_name(_graphname, _algName + postfix, _seedsize, _probDist, _probEdge);
		if (_model == LT) _outFileName = "LT_" + _outFileName;
		if (_algName == "OPIM" || _algName == "opim") _outFileName += "_s" + std::to_string(_samplesize);
	}

	std::string get_outfilename_with_alg(const std::string& algName) const
	{
		return TIO::get_out_file_name(_graphname, algName, _seedsize, _probDist, _probEdge);
	}
};

using TArgument = Argument;
using PArgument = std::shared_ptr<TArgument>;
