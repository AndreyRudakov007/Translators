@set do=ECHO

@set src=%1

@if "%src%"  == "" set /p  src=Source language?

_%src%-2-tree _%src%1.txt _tree.txt
_tree-2-%src% _tree.txt _%src%2.txt
_tree-2-%src%asm _tree.txt _asm.txt

mkdir bin\%src%

__treevis _tree.txt

move _%src%2.txt bin\%src%\
move _tree.txt bin\%src%\
move _asm.txt bin\%src%\
move out.dot bin\%src%\
xcopy Graph.jpg bin\%src%\ 

@pause
