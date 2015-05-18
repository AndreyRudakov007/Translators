#pragma once

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <iostream>

#include "standart.h"

const int DEF_FUNC_BUFFER = 11;


using namespace std;

#define FNAME_INPUT "10.txt"
#define FNAME_INPUT2 "11.txt"
#define ISOP_PLUS(cur) if ((cur)->operator_type == Parser_OperatorType::PLUS)
#define ISOP_MINUS(cur) if ((cur)->operator_type == Parser_OperatorType::MINUS)
#define ISOP_MUL(cur) if ((cur)->operator_type == Parser_OperatorType::MUL)
#define ISOP_DIV(cur) if ((cur)->operator_type == Parser_OperatorType::DIV)
#define ISOP_LESS(cur) if ((cur)->operator_type == Parser_OperatorType::LESS)
#define ISOP_GREAT(cur) if ((cur)->operator_type == Parser_OperatorType::GREAT)



#define ISFUNC_SIN(cur) if ((cur)->func_type == FuncType::SIN)
#define ISFUNC_COS(cur) if ((cur)->func_type == FuncType::COS)
#define IS_CONST(cur) if ((cur)->node_type == Parser_NodeType::CONSTANT)
#define ISX(cur) if ((cur)->node_type == Parser_NodeType::VAR)


const int MAX_LENGTH_BUFFER = 1000;
const char END_SYMBOL = '.';

enum Parser_OperatorType
{
	PLUS, MINUS, DIV, MUL, LESS, GREAT, EQUAL, GE, LE, NE, OR, AND
};

enum Order
{
	PREFIX, INFIX, POSTFIX
};

enum Parser_NodeType
{
	CONSTANT, OPERATOR, FUNC, VAR
};

enum LexType
{
	BR, NUM, FUNCT, OP, VARIAB
};

enum FuncType
{
	COS, SIN, PRINT, READ, CALL, MIN_ = 9, MAX_ = 10, USER_0, USER_1, USER_2, USER_3, USER_4
};

class Parser_Tree;
class TreeNode;

class Parser_Tree
{
public:
	Parser_Tree(const char *str, int numfuncs, char **namesfuncs);
    ~Parser_Tree();

	void printTree(Order new_order, const char *);
	void printTreeTEX(FILE *f);
	int printResult();
	void CalcDerivate();
	void Simplify();
	Tree* export(int number_of_vars, char** namesvar, int number_of_funcs, char** namesfunc);

private:
	TreeNode *Multiple();
	TreeNode *Factor();
	TreeNode *Number();
	TreeNode *Add();
	TreeNode *Comp();
	TreeNode *Logic();
	void FuncBodyParse(TreeNode**left, TreeNode**right);
    Tree* make_export(TreeNode* t, int number_of_vars, char** namesvar, int number_of_funcs, char** namesfunc);

	TreeNode *derivate(TreeNode *cur);
	TreeNode* MakeSimply(TreeNode *cur);
	bool Equal_Tree(TreeNode *tree1, TreeNode *tree2);

	void printPrefix(TreeNode*, FILE *f);
	void printPostfix(TreeNode*, FILE *f);
	void printInfix(TreeNode*, FILE *f);
	void printInfixTEX(TreeNode*, FILE *f);


	TreeNode *root;
	char *expression;
	int pos;

    int numfuncs;
    char **namesfuncs;
};