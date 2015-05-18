#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <iostream>
#include "parser.h"

const char* DefinedFunctions[DEF_FUNC_BUFFER] = { "subzero", "read", "print", "sin", "cos", "getch", "call", "differentiate", "sqrt", "min", "max" };

using namespace std;


class Lexem
{
public:
    LexType type;
    char* str;
    int val;
};

Lexem LexAr [MAX_LENGTH_BUFFER];

bool IsDefinedFunction (char * str)
{
    for (int i = 0; i < DEF_FUNC_BUFFER; i++)
    {
        std::cerr << str << " " << DefinedFunctions[i] << std::endl;
        if (strcmp(str, DefinedFunctions[i]) == 0) return true;
    }
    return false;
}

void LexAnalyse (char* str)
{
    int index = 0;
    int index_lexem = 0;
    int len = strlen(str);
    while(index < len)
    {
        Lexem cur;
        if (str[index] >= '0' && str[index] <= '9')
        {
            cur.type = LexType::NUM;
            cur.val = 0;
            while (str[index] >= '0' && str[index] <= '9')
            {
                cur.val = cur.val * 10 + str[index] - '0';
                index++;
            }
        }

        else if (str[index] == '(' || str[index] == ')')
        {
            cur.type = LexType::BR;
            cur.str = (char*) calloc (2,sizeof(char));
            cur.str[0] = str[index];
            index++;
        }

        else if (str[index] == '-' || str[index] == '+' || str[index] == '*' || str[index] == '/')
        {
            cur.type = LexType::OP;
            cur.str = (char*) calloc (2,sizeof(char));
            cur.str[0] = str[index];
            index++;
        }

        else if ((str[index] >= 'a' && str[index] <= 'z') || (str[index] >= 'A' && str[index] <= 'Z'))
        {
            cur.str = (char*) calloc (MAX_LENGTH_BUFFER, sizeof(char));
            int inner_index = 0;
            while ((str[index] >= 'a' && str[index] <= 'z') || (str[index] >= 'A' && str[index] <= 'Z'))
            {
                cur.str[inner_index] = str[index];
                inner_index++;
                index++;
            }

            if (IsDefinedFunction(cur.str))
            {
                cur.type = FUNCT;
            }
            else
            {
                cur.type = VARIAB;
            }
        }

        LexAr[index_lexem] = cur;
        index_lexem++;
    }

}




class TreeNode
{
public:
    friend Parser_Tree;

    TreeNode() : left(NULL), right(NULL) {}

    void createConstant(int new_value);
	void createOperator(Parser_OperatorType new_type);
    void createFunction(FuncType new_type);
    void createVar(char* str);
    TreeNode *deep_copy();

    void printValue(FILE *f);
    int calcValue();

private:
    TreeNode *left, *right;
    int value;
    char* name;

	Parser_NodeType node_type;
	Parser_OperatorType operator_type;
    FuncType func_type;
};

void TreeNode::createConstant(int new_value)
{
    value = new_value;
	node_type = Parser_NodeType::CONSTANT;
}

void Parser_Tree::Simplify()
{
   root = MakeSimply(root);

}


bool Parser_Tree::Equal_Tree(TreeNode *cur1, TreeNode *cur2)
{
   if(cur1 == NULL || cur2 == NULL)
   {
        return true;
   }

   if (cur1->node_type == cur2->node_type)
   {
	   if (cur1->node_type == Parser_NodeType::OPERATOR && cur1->operator_type == cur2->operator_type)
      {
         return Equal_Tree (cur1->left, cur2->left) && Equal_Tree (cur1->right, cur2->right);
      }

	   if (cur1->node_type == Parser_NodeType::FUNC && cur1->func_type == cur2->func_type)
      {
         return Equal_Tree (cur1->left, cur2->left) && Equal_Tree (cur1->right, cur2->right);
      }
	   if (cur1->node_type == Parser_NodeType::CONSTANT && cur1->value == cur2->value)
      {
         return true;
      }

      return false;

    }

    else return false;
}



TreeNode* Parser_Tree::MakeSimply(TreeNode *cur)
{

    if(cur->left)
        cur->left = MakeSimply(cur->left);

    if(cur->right)
        cur->right = MakeSimply(cur->right);

	if (cur->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(cur)
        {
			if (cur->left && cur->left->node_type == Parser_NodeType::CONSTANT && cur->left->value == 0)
           {
               delete cur->left;
               cur->left = NULL;
           }
			if (cur->right && cur->right->node_type == Parser_NodeType::CONSTANT && cur->right->value == 0)
           {
               delete cur->right;
               cur->right = NULL;
           }
        }

        ISOP_MINUS(cur)
        {
           if (Equal_Tree(cur->left, cur->right))
           {
               delete cur->right;
               delete cur->left;
               cur->right = NULL;
               cur->left = NULL;
           }
		   if (cur->right && cur->right->node_type == Parser_NodeType::CONSTANT && cur->right->value == 0)
           {
               delete cur->right;
               cur->right = NULL;
           }



        }

        if((cur->right == NULL) &&(cur->left == NULL))
        {
            delete cur;
            cur = NULL;
        }

    }

    return cur;

}

void TreeNode::createOperator(Parser_OperatorType new_type)
{
    operator_type = new_type;
	node_type = Parser_NodeType::OPERATOR;
}

TreeNode* TreeNode:: deep_copy()
{
    TreeNode *cur = new TreeNode();
    cur->value = value;
    cur->node_type = node_type;
    cur->operator_type = operator_type;
    cur->func_type = func_type;
    if (left)
        cur->left = left->deep_copy();
    if (right)
        cur->right = right->deep_copy();
    return cur;
}



void TreeNode::createFunction(FuncType new_type)
{
	node_type = Parser_NodeType::FUNC;
	cerr << "TYPE" << new_type << endl;
    func_type = new_type;
}

void TreeNode::createVar(char * str)
{
	node_type = Parser_NodeType::VAR;
    name = str;
}


void Parser_Tree::CalcDerivate()
{
    root = derivate(root);
}

void TreeNode::printValue(FILE *f)
{
    IS_CONST(this)
    {
        fprintf(f, "%d", value);
    }
	else if (node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(this)
        {
            fprintf(f, "+");
        }
        ISOP_MINUS(this)
        {
            fprintf(f, "-");
        }
        ISOP_MUL(this)
        {
            fprintf(f, "*");
        }
        ISOP_DIV(this)
        {
            fprintf(f, "/");
        }
        ISOP_LESS(this)
        {
            fprintf(f, "<");
        }
        ISOP_GREAT(this)
        {
            fprintf(f, ">");
        }
    }
	else if (node_type == Parser_NodeType::FUNC)
    {
        fprintf(f, "%s", DefinedFunctions[func_type]);

    }
    else ISX(this)
    {
        fprintf (f, "%s", name);
    }
}

Parser_Tree::~Parser_Tree()
{
    free(expression);
}

Parser_Tree::Parser_Tree(const char *str, int _numfuncs, char **_namesfuncs)
{
    expression = (char *)calloc(strlen(str) + 2, sizeof(char));
    for(int i = 0; i < strlen(str); i++)
    {
        expression[i] = str[i];
    }
    expression[strlen(str)] = END_SYMBOL;
    expression[strlen(str) + 1] = 0;
    std::cerr << "#" << expression << "#" << std::endl;
    pos = 0;

    numfuncs = _numfuncs;
    namesfuncs = _namesfuncs;

    root = Logic();
}


TreeNode *Parser_Tree::derivate(TreeNode *cur)
{
    TreeNode* res = new TreeNode();

	if (cur->node_type == Parser_NodeType::FUNC)
    {
        ISFUNC_SIN(cur)
        {
			res->createOperator(Parser_OperatorType::MUL);
            res->left = new TreeNode();
            res->left->createFunction(FuncType::COS);
            res->left->right = cur->right->deep_copy();

            res->right = derivate(cur->right);
        }

        ISFUNC_COS(cur)
        {
			res->createOperator(Parser_OperatorType::MUL);
            res->left = new TreeNode();
            res->left->createFunction(FuncType::SIN);
            res->left->right = cur->right->deep_copy();

            res->right = derivate(cur->right);

            TreeNode* tmp = new TreeNode();
			tmp->createOperator(Parser_OperatorType::MUL);
            tmp->left = new TreeNode();
            tmp->left->createConstant(-1);

            tmp->right = res;
            res = tmp;
        }

    }

    IS_CONST(cur)
    {
        res->createConstant(0);
    }

    ISX(cur)
    {
        res->createConstant(1);
    }

	if (cur->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(cur)
        {
			res->createOperator(Parser_OperatorType::PLUS);
            res->left = derivate(cur->left);
            res->right = derivate(cur->right);
        }

        ISOP_MINUS(cur)
        {
			res->createOperator(Parser_OperatorType::MINUS);
            res->left = derivate(cur->left);
            res->right = derivate(cur->right);
        }

        ISOP_DIV(cur)
        {
			res->createOperator(Parser_OperatorType::DIV);

            res->left = new TreeNode ();
			res->left->createOperator(Parser_OperatorType::MINUS);
            res->left->left = new TreeNode();
			res->left->left->createOperator(Parser_OperatorType::MUL);
            res->left->left->left = derivate(cur->left);
            res->left->left->right = cur->right->deep_copy();

            res->left->right = new TreeNode();
			res->left->right->createOperator(Parser_OperatorType::MUL);
            res->left->right->left = derivate(cur->right);
            res->left->right->right = cur->left->deep_copy();

			res->right->createOperator(Parser_OperatorType::MUL);
            res->right->left = cur->right->deep_copy();
            res->right->right = cur->right->deep_copy();
        }

        ISOP_MUL(cur)
        {
			res->createOperator(Parser_OperatorType::PLUS);
            res->left = new TreeNode();
			res->left->createOperator(Parser_OperatorType::MUL);
            res->left->left = derivate(cur->left);
            res->left->right = cur->right->deep_copy();

            res->right = new TreeNode();
			res->right->createOperator(Parser_OperatorType::MUL);
            res->right->left = derivate(cur->right);
            res->right->right = cur->left->deep_copy();

        }
    }
    return res;

}


TreeNode *Parser_Tree::Add()
{
	//std::cerr << expression + pos << std::endl;
	Parser_OperatorType op_type;
    TreeNode *tree1 = NULL, *tree2 = NULL, *ans = NULL;

    tree1 = Multiple();
    ans = tree1;
    while (expression[pos] == '+' || expression[pos] == '-')
    {
        if (expression[pos] == '+')
        {
			op_type = Parser_OperatorType::PLUS;
        }
        else
        {
			op_type = Parser_OperatorType::MINUS;
        }
        pos++;
        tree2 = Multiple();

        tree1 = new TreeNode();
        tree1->createOperator(op_type);
        tree1->left = ans;
        tree1->right = tree2;
        ans = tree1;
    }
    return ans;
}


TreeNode *Parser_Tree::Logic()
{
	Parser_OperatorType op_type;
	TreeNode *tree1 = NULL, *tree2 = NULL, *ans = NULL;

	tree1 = Comp();
	ans = tree1;
	while (expression[pos] == '|' || expression[pos] == '&')
	{
		if (expression[pos] == '|')
		{
			op_type = Parser_OperatorType::OR;
			pos++;
		}
		if (expression[pos] == '&')
		{
			op_type = Parser_OperatorType::AND;
			pos++;
		}
		tree2 = Comp();

		tree1 = new TreeNode();
		tree1->createOperator(op_type);
		tree1->left = ans;
		tree1->right = tree2;
		ans = tree1;
	}
	return ans;
}


TreeNode *Parser_Tree::Comp()
{
	//std::cerr << expression + pos << std::endl;
	Parser_OperatorType op_type;
    TreeNode *tree1 = NULL, *tree2 = NULL, *ans = NULL;

    tree1 = Add();
    ans = tree1;
	while (expression[pos] == '!' || expression[pos] == '=' || expression[pos] == '<' || expression[pos] == '>')
    {
		if (expression[pos] == '=')
		{
			op_type = Parser_OperatorType::EQUAL;
			pos += 2;
		}
		if (expression[pos] == '!')
		{
			op_type = Parser_OperatorType::NE;
			pos += 2;
		}
		if (expression[pos] == '<')
        {
			if (expression[pos + 1] == '=')
			{
				op_type = Parser_OperatorType::LE;
				pos += 2;
			}
			else
			{
				op_type = Parser_OperatorType::LESS;
				pos++;
			}
		}
		if (expression[pos] == '>')
		{
			if (expression[pos + 1] == '=')
			{
				op_type = Parser_OperatorType::GE;
				pos += 2;
			}
			else
			{
				op_type = Parser_OperatorType::GREAT;
				pos++;
			}
		}
        tree2 = Add();

        tree1 = new TreeNode();
        tree1->createOperator(op_type);
        tree1->left = ans;
        tree1->right = tree2;
        ans = tree1;
    }
    return ans;
}

char* GetPrefix(char* str)
{
    char* res = (char *)calloc(strlen(str) + 2, sizeof(char));
    for (int i = 0; i < strlen(str) && str[i] >= 'a' && str[i] <= 'z'; i++)
    {
        res[i]=str[i];
    }
    return res;
}

int IsStdFunc (char* str)
{
    for (int i = 0; i < DEF_FUNC_BUFFER; i++)
    {
        //std::cerr << "EQUALITY " << str << " " << DefinedFunctions[i] << endl;
		if (strcmp(str, DefinedFunctions[i]) == 0)
		{
			cerr << "DEFINED" << i << endl;
			return i;
		}
    }
    return -1;
}

int IsUserFunc(char* str, int num, char **funcs)
{
    printf("$$NUM = %d", num);
    for (int i = 0; i < num; i++)
    {
        printf("NAME: %s\n", funcs[i]);
        if (strcmp(str, funcs[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

TreeNode *Parser_Tree::Multiple()
{
	//std::cerr << expression + pos << std::endl;
	Parser_OperatorType op_type;
    TreeNode *tree1 = NULL, *tree2 = NULL, *ans = NULL;

    tree1 = Factor();
    ans = tree1;
    while (expression[pos] == '*' || expression[pos] == '/')
    {
        if (expression[pos] == '*')
        {
			op_type = Parser_OperatorType::MUL;
        }
        else
        {
			op_type = Parser_OperatorType::DIV;
        }
        pos++;
        tree2 = Factor();

        tree1 = new TreeNode();
        tree1->createOperator(op_type);
        tree1->left = ans;
        tree1->right = tree2;
        ans = tree1;
    }
    return ans;
}


void Parser_Tree::FuncBodyParse(TreeNode**left, TreeNode**right)
{
	pos++;
	*left = Logic();
	if (expression[pos] == ',')
	{
		pos++;
		*right = Logic();
	}
	else
	{
		*right = *left;
		*left = NULL;
	}
	pos++;
}


TreeNode *Parser_Tree::Factor()
{
	//std::cerr << expression + pos << std::endl;
    if (expression[pos] >= '0' && expression[pos] <= '9')
    {
        return Number();
    }
    else if (expression[pos] == '(')
    {
        pos++; // Opening bracket
        TreeNode *ans = Logic();
        pos++; // Closing bracket
        return ans;
    }
    else
    {
        char* temp = GetPrefix(expression + pos);
        int t = IsStdFunc(temp);
        int k = IsUserFunc(temp, numfuncs, namesfuncs);
        //std::cerr << temp << std::endl;
        //std::cerr << t << std::endl;
        if (t == -1 && k == -1)
        {
            pos += strlen(temp);
            TreeNode *ans = new TreeNode();
            ans->createVar(temp);
            //ans->name = temp;
            return ans;
        }
        else
        {
            if (t != -1)
            {
                pos += strlen(temp);
                TreeNode *ans = new TreeNode();
                ans->createFunction((FuncType)t);
                printf("function body ->> %s <<-\n", expression + pos);
				FuncBodyParse(&ans->left, &ans->right);
                return ans;
            }
            else
            {
                pos += strlen(temp);
                TreeNode *ans = new TreeNode();
                FuncType ustype;
                printf("USER FUNCTION %s#\n", temp);
                if (k == 0)
                    ustype = FuncType::USER_0;
                if (k == 1)
                    ustype = FuncType::USER_1;
                if (k == 2)
                    ustype = FuncType::USER_2;
                ans->createFunction(ustype);
                //ans->right = Logic();
                return ans;
            }
        }
    }
}

TreeNode *Parser_Tree::Number()
{
    int cur_number = 0;
    while (expression[pos] >= '0' && expression[pos] <= '9')
    {
        cur_number = cur_number * 10 + expression[pos] - '0';
        pos++;
    }
    TreeNode *ans = new TreeNode();
    ans->createConstant(cur_number);
    return ans;
}

Tree* Parser_Tree::export(int number_of_vars, char** namesvar, int number_of_funcs, char** namesfuncs)
{
	printInfix(root, stdout);
	return make_export(root, number_of_vars, namesvar, number_of_funcs, namesfuncs);
}

Tree* Parser_Tree::make_export(TreeNode * t, int number_of_vars, char** namesvar, int number_of_funcs, char** namesfuncs)
{
	Tree* cur = new Tree();
	if (t->node_type == Parser_NodeType::CONSTANT)
	{
		cur->type = NodeType::Number;
		cur->value = t->value;
	}

	else if (t->node_type == Parser_NodeType::OPERATOR)
	{
		cur->type = NodeType::Operator;
		if (t->operator_type == Parser_OperatorType::DIV)
		{
			cur->value = OperatorType::Divide;
		}
		if (t->operator_type == Parser_OperatorType::GREAT)
		{
			cur->value = OperatorType::Bool_greater;
		}
		if (t->operator_type == Parser_OperatorType::LESS)
		{
			cur->value = OperatorType::Bool_lower;
		}
		if (t->operator_type == Parser_OperatorType::MINUS)
		{
			cur->value = OperatorType::Minus;
		}
		if (t->operator_type == Parser_OperatorType::MUL)
		{
			cur->value = OperatorType::Multiply;
		}
		if (t->operator_type == Parser_OperatorType::PLUS)
		{
			cur->value = OperatorType::Plus;
		}
		if (t->operator_type == Parser_OperatorType::EQUAL)
		{
			cur->value = OperatorType::Bool_equal;
		}
		if (t->operator_type == Parser_OperatorType::GE)
		{
			cur->value = OperatorType::Bool_not_lower;
		}
		if (t->operator_type == Parser_OperatorType::LE)
		{
			cur->value = OperatorType::Bool_not_greater;
		}
		if (t->operator_type == Parser_OperatorType::NE)
		{
			cur->value = OperatorType::Bool_not_equal;
		}
		if (t->operator_type == Parser_OperatorType::OR)
		{
			cur->value = OperatorType::Bool_or;
		}
		if (t->operator_type == Parser_OperatorType::AND)
		{
			cur->value = OperatorType::Bool_and;
		}
	}
	
	else if (t->node_type == Parser_NodeType::VAR)
	{
		for (int i = 0; i < number_of_vars; i++)
		{
			if (strcmp(t->name, namesvar[i]) == 0)
			{
				cur->type = NodeType::Variable;
				cur->value = i;
			}
		}
	}
    else if (t->node_type == Parser_NodeType::FUNC)
    {
        
        /*for (int i = 0; i < number_of_funcs; i++)
        {
            if (strcmp(DefinedFunctions[t->func_type], namesfuncs[i]) == 0)
            {
                cur->type = NodeType::User_function;
                cur->value = i;
            }
        }*/
        if (t->func_type == FuncType::USER_0 || t->func_type == FuncType::USER_1 || t->func_type == FuncType::USER_2)
        {
			cerr << "DEFINED?" << cur->value << endl;
            cur->type = NodeType::Standart_function;
            cur->value = StandartFunction::Call;
            int indexUser = (int)t->func_type - (int)FuncType::USER_0;
            cur->AddChild(NodeType::User_function, indexUser);
        }
        else
        {
            cur->type = NodeType::Standart_function;
            cur->value = double(t->func_type);
			cerr << "DEFINED#" << cur->value << endl;
        }
    }


	if (t->left)
	{
		cur->AddChild(make_export(t->left, number_of_vars, namesvar, number_of_vars, namesfuncs));
	}
	if (t->right)
	{
		cur->AddChild(make_export(t->right, number_of_vars, namesvar, number_of_vars, namesfuncs));
	}
	return cur;
}


void Parser_Tree::printTree(Order order, const char *fname)
{
    FILE *f = stdout;
    if (fname != NULL)
    {
        f = fopen(fname, "w");
    }

    if (order == Order::INFIX)
    {
        printInfix(root, f);
    }
    else if (order == Order::POSTFIX)
    {
        printPostfix(root, f);
    }
    else if (order == Order::PREFIX)
    {
        printPrefix(root, f);
    }
}

void Parser_Tree::printTreeTEX(FILE *f)
{
    printInfixTEX(root, f);
}

void Parser_Tree::printInfixTEX(TreeNode *t, FILE *f)
{
    if (t == NULL)
        return;

	if (t && t->left && t->right && t->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(t)
        {
            fprintf(f, "(");
        }

        ISOP_MINUS(t)
        {
            fprintf(f, "(");
        }
    }

    printInfixTEX(t->left, f);

	if (!(t->node_type == Parser_NodeType::OPERATOR) || (t->left && t->right))
    {
		if (t->node_type == Parser_NodeType::FUNC)
        {
            fprintf (f, "\\");
        }
        t->printValue(f);

    }

	if (t->node_type == Parser_NodeType::FUNC)
    {
        fprintf (f, "{");
    }

    printInfixTEX(t->right, f);

	if (t->node_type == Parser_NodeType::FUNC)
    {
        fprintf (f, "}");
    }

	if (t && t->left && t->right && t->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(t)
        {
            fprintf(f, ")");
        }

        ISOP_MINUS(t)
        {
            fprintf(f, ")");
        }
    }
}

void Parser_Tree::printInfix(TreeNode *t, FILE *f)
{
    if (t == NULL)
        return;

	if (t && t->left && t->right && t->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(t)
        {
            fprintf(f, "(");
        }

        ISOP_MINUS(t)
        {
            fprintf(f, "(");
        }
    }

    printInfix(t->left, f);

	if (!(t->node_type == Parser_NodeType::OPERATOR) || (t->left && t->right))
        t->printValue(f);

    printInfix(t->right, f);

	if (t && t->left && t->right && t->node_type == Parser_NodeType::OPERATOR)
    {
        ISOP_PLUS(t)
        {
            fprintf(f, ")");
        }

        ISOP_MINUS(t)
        {
            fprintf(f, ")");
        }
    }
}

void Parser_Tree::printPrefix(TreeNode *t, FILE *f)
{
    if (t == NULL)
        return;
    t->printValue(f);
    printPrefix(t->left, f);
    printPrefix(t->right, f);
}

void Parser_Tree::printPostfix(TreeNode *t, FILE *f)
{
    if (t == NULL)
        return;
    printPostfix(t->left, f);
    printPostfix(t->right, f);
    t->printValue(f);
}

int Parser_Tree::printResult()
{
    return root->calcValue();
}

int TreeNode::calcValue()
{
    int ans1 = 0, ans2 = 0;
	if (node_type == Parser_NodeType::CONSTANT)
        return value;

    if (left)
        ans1 = left->calcValue();
    if (right)
        ans2 = right->calcValue();

    ISOP_PLUS(this)
        return ans1 + ans2;
    ISOP_MINUS(this)
        return ans1 - ans2;
    ISOP_MUL(this)
        return ans1 * ans2;
    ISOP_DIV(this)
    {
        if (ans2 == 0)
            return 0;
        else
            return ans1 / ans2;
    }
}

//int main()
//{
//    //return 0;
//	Parser_Tree *t = new Parser_Tree("(1+ababa)*3<100");
//    t->Simplify();
//    t->printTree(Order::POSTFIX, 0);
//    //printf("\n");
//    //fprintf(f, ")' = ");
//    //t->printTreeTEX(f);
//    //fprintf(f, " $$\n\\bye");
//    //fclose(f);
//    //system("run.bat");
//
//	int x;
//	cin >> x;
//
//
//
//    return 0;
//}
