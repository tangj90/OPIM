REM Begin

SET exec=OPIM1.1.exe
REM Format graph
CALL %exec% -func=0 -gname=facebook
PAUSE
REM Test function 1 of 4: OPIM
CALL %exec% -func=1 -gname=facebook -alg=opim -mode=2 -seedsize=50 -samplesize=1000
PAUSE
REM Test function 2 of 4: OPIM
CALL %exec% -func=1 -gname=facebook -alg=opim -mode=0 -seedsize=100 -samplesize=1024000 -model=LT -pdist=WC 
PAUSE
REM Test function 3 of 4: OPIM-C
CALL %exec% -func=1 -gname=facebook -alg=opim-c -mode=2 -seedsize=50 -eps=0.01 -model=IC -pdist=load
PAUSE
REM Test function 4 of 4: OPIM-C
CALL %exec% -func=1 -gname=facebook -alg=opim-c -mode=1 -seedsize=50 -eps=0.01 -model=IC -pdist=load
PAUSE