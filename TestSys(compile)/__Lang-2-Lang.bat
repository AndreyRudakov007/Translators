@set do=ECHO

@set src=%1
@set dest=%2

@if "%src%"  == "" set /p  src=Source language?
@if "%dest%"  == "" set /p  dest=Destination language?

_%src%-2-tree _%src%1.txt _tree.txt
_tree-2-%dest% _tree.txt _%dest%.txt

_tree-2-%src%asm _tree.txt _%src%_asm.txt
_tree-2-%dest%asm _tree.txt _%dest%_asm.txt

mkdir bin\%src%-2-%dest%\

__treevis _tree.txt

move _tree.txt bin\%src%-2-%dest%\
move _%dest%.txt bin\%src%-2-%dest%\
move _%src%_asm.txt bin\%src%-2-%dest%\
move _%dest%_asm.txt bin\%src%-2-%dest%\
move out.dot bin\%src%-2-%dest%\
xcopy Graph.jpg bin\%src%-2-%dest%\ 

@pause
