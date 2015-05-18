#pragma once


#include <cstdlib>

enum NodeType
{
	Root, User_function, Number, Variable, Operator, Logic, Main, Standart_function, Var_init, Service
};

enum OperatorType
{
	Equal = 1, Plus, Minus, Multiply, Divide, Power, Bool_equal, Bool_not_equal, Bool_lower, Bool_greater, Bool_not_lower, Bool_not_greater, Bool_not, Bool_or, Bool_and, Plus_equal, Minus_equal, Multiply_equal, Divide_equal
};

enum StandartFunction
{
	Input = 1, Output, Sin, Cos, Getch, Call, Differentiate, Square_Root, MIN, MAX
};

enum Logic1
{
	If = 1, While, Else, Condition, Condition_met
};



class Tree
{
public:
	int type;
	double value;
	int size;
	Tree** childrens;

	Tree() {}
	void AddChild(int _type, double _value);
	void AddChild(Tree *t);

};
