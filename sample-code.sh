#!/bin/bash
echo Begin

cmd="./OPIM1.1.o"
echo Format graph
${cmd} -func=0 -gname=facebook
read -rsp $'Press any key to continue...\n' -n 1 key
echo Test function 1 of 4: OPIM
${cmd} -func=1 -gname=facebook -alg=opim -mode=2 -seedsize=50 -samplesize=1000
read -rsp $'Press any key to continue...\n' -n 1 key
echo Test function 2 of 4: OPIM
${cmd} -func=1 -gname=facebook -alg=opim -mode=0 -seedsize=100 -samplesize=1024000 -model=LT -pdist=WC
read -rsp $'Press any key to continue...\n' -n 1 key
echo Test function 3 of 4: OPIM-C
${cmd} -func=1 -gname=facebook -alg=opim-c -mode=2 -seedsize=50 -eps=0.01 -model=IC -pdist=load
read -rsp $'Press any key to continue...\n' -n 1 key
echo Test function 4 of 4: OPIM-C
${cmd} -func=1 -gname=facebook -alg=opim-c -mode=1 -seedsize=50 -eps=0.01 -model=IC -pdist=load
