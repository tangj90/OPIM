# OPIM version 1.1
This project implements the OPIM and OPIM-C algorithms for the following paper:
- Jing Tang, Xueyan Tang, Xiaokui Xiao, Junsong Yuan, "Online Processing Algorithms for Influence Maximization," in Proc. ACM SIGMOD, 2018.

**Author: Jing Tang (Nanyang Technological University)**
> Cross-platform supported, including **\*nix** and **Windows** systems!\
Under **\*nix** system, compile and build codes:\
`g++ -std=c++14 -O3 OPIM.cpp -o OPIM1.1.o`\
Note: `c++1x` is required.

**_Before running influence maximization algorithms, please format the graph first!_**

Execute the command: `{your_exec} [options]`. `{your_exec}` may be `OPIM1.1.o` under **\*nix** or `OPIM1.1.exe` under **Windows**.

See some sample codes in **sample-code.sh** or **sample-code.bat**. For example,

- Format the graph with the WC setting: 

	`{your_exec} -func=0 -gname=facebook`

- Run the OPIM algorithm using the minimum bound among all the rounds to select 50 nodes and report its solution quality (approximation guarantee) when 1000 RR sets are generated under the IC model (diffusion probability is loaded directly from the file, which is the WC setting if the graph is formatted in the above procedure):

	`{your_exec} -func=1 -gname=facebook -alg=opim -mode=2 -seedsize=50 -samplesize=1000`

- Run the vanilla OPIM algorithm to select 100 nodes and report its solution quality when 1024000 RR sets are generated under the LT-WC model:

	`{your_exec} -func=1 -gname=facebook -alg=opim -mode=0 -seedsize=100 -samplesize=1024000 -model=LT -pdist=WC`

- Run the OPIM-C algorithm using the minumum upper bound to select 50 nodes under the IC model:

	`{your_exec} -func=1 -gname=facebook -alg=opim-c -mode=2 -seedsize=50 -eps=0.01 -model=IC -pdist=load`

- Run the OPIM-C algorithm using the upper bound in the last round to select 500 nodes under the IC model:

	`{your_exec} -func=1 -gname=facebook -alg=opim-c -mode=1 -seedsize=50 -eps=0.01 -model=IC -pdist=load`

### Options
- **-func=integer**

	Specify the function to execute. Possible values:
  + **0**: Format the graph.
  + **1** *[default]*: Run influence maximization algorithms.
  + **2**: Format the graph and run influence maximization algorithms.
	
- **-dir=string**

	Specify the direction for the input files, e.g., **-dir=graphInfo** *[default]*.
    
- **-gname=string**
	
	Specify the graph name to process, e.g., **-gname=facebook** *[default]*.
    
- **-mode=string**

  *When -func=0:* Specify how to format the graph file. Two types of original graph file are supported. The file should have *m+1* lines where the first line has two integers to indicate the number *n* of nodes and the number *m* of edges. The following *m* lines list the edges. The node is indexed from *0* to *n-1*. Each edge line has two integers to indicate the source node and target node.
	+ **-mode=g** *[default]*: Weighted Cascade (WC) setting is used where *p(u,v)=1/indeg(v)*. For example,
    	> 3 4\
    	0 1\
    	0 2\
    	1 0\
    	1 2
    
   + **-mode=w**: Each edge line can also follow with a positive number to indicate the diffusion probability. In this case, the diffusion probability is associated by the given value. For example,
    	> 3 4\
    	0 1 0.2\
    	0 2 0.3\
    	1 0 0.1\
    	1 2 0.4
      
  *When -func=1:* Specify which upper bound is used for calculating approximation guarantee.
  + **-mode=0**: Vanilla version, which returns (1-1/e)-approximation.
  + **-mode=1**: Only use the upper bound in the last round.
  + **-mode=2** *[default]*: Use the minimum upper bound among all the rounds.

#### The following options are valid for *-func=1* or *-func=2*.

- **-outpath=string**

  Specify the folder to save results, e.g., **-outpath=result** *[default]*.

- **-alg=string**

  Specify the algorithm used for influence maximization. Possible values:
	+ **opim** *[default]*: OPIM algorithm, which aims to maximize the approximation guarantee.
	+ **opim-c**: OPIM-C algorithm, which aims to reduce running time for achieving (1-1/e-eps)-approximation guarantee.
  
- **-seedsize=integer**

  Specify the number of nodes to be selected. The default value is *50*.

- **-samplesize=integer** *(works for OPIM only)*

  Specify the number of RR sets to be generated for OPIM.
  
- **-eps=decimal** *(works for OPIM-C only)*

  Specify the error threshold such that OPIM-C returns (1-1/e-eps)-approximate solution.
  
- **-delta=decimal**

  Specify the failure probability, which means the probability of success is at least 1-delta.
  
- **-model=string**
  
  Specify the cascade model. Possible values:
	+ **IC** *[default]*: Independent Cascade model.
	+ **LT**: Linear Threshold cascade model.
	
- **-pdist=string**

  Specify the diffusion probability. Possible values:
  + **load** *[default]*: Load directly from the file if unspecified. Note that if the graph is formatted from the edge list without weights, **WC** setting is used already. So this parameter can be unspecified for the WC setting in this case. 
  + **WC**: Weighted Cascade (WC) setting, i.e., *p(u,v)=1/indeg(v)*. Support the IC and LT models.
  + **TRI**: Trivalency (TRI) setting, i.e., *p(u,v)* of each edge *(u,v)* is set by choosing a probability from the set *{0.1, 0.01, 0.001}* at random. Support the IC model. It may violate the LT model.
  + **UNI**: Uniform (UNI) setting, i.e., *p(u,v)* of each edge *(u,v)* is set to a given value specified by **-pedge=decimal** (default is *0.1*). Support the IC model. It may violate the LT model.
  
- **-pedge=decimal**

  Specify the diffusion probability for every edge under the *UNI* setting. The default value is *0.1*. 
