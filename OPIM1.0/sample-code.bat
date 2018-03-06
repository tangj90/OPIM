REM Begin

SET exec=OPIM1.0.exe
CALL %exec% -func=0 -gname=facebook
PAUSE
CALL %exec% -func=1 -gname=facebook -alg=opim -mode=2 -seedsize=50 -samplesize=1000
CALL %exec% -func=1 -gname=facebook -alg=opim -mode=0 -seedsize=100 -samplesize=1024000 -model=LT -pdist=WC 
CALL %exec% -func=1 -gname=facebook -alg=opim-c -mode=2 -seedsize=50 -eps=0.01 -model=IC -pdist=load
CALL %exec% -func=1 -gname=facebook -alg=opim-c -mode=1 -seedsize=50 -eps=0.01 -model=IC -pdist=load
PAUSE