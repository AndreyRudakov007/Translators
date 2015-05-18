#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
#include <cctype>
#include <iostream>
#include "standart.h"
#include "parser.h"

#define PERSONAL_CODE 9

#define MAX_FUNC_NUM 20
#define MAX_VAR_NUM 20
#define MAX_NAME_LEN 100

#define VAR_KEYWORD "var"
#define DEF_KEYWORD "def"
#define END_KEYWORD "end"
#define IF_KEYWORD "if"
#define WHILE_KEYWORD "while"
#define ELSE_KEYWORD "else"

using namespace std;


class Parser
{
public:

    Tree* Parse_Body (FILE*f, bool &flag);
    Tree* parse_if (FILE*f, char* str);
    Tree* parse_if_condition (char* str);
    Tree* parse_while_condition (char* str);
    Tree* parse_while (FILE*f, char* str);
    Tree* parse_expression (char* str);
    char* first_word (char* str);
    void ReadFile (const char* name);
    void WriteToFile (const char* name);
    void ParseFile ();
    bool SearchVars (char* name);
    bool SearchFuncs (char *name);
	

    Parser ()
    {
        number_of_func = 0;
        number_of_vars = 0;
        namesfunc = (char**)calloc(MAX_FUNC_NUM, sizeof(char*));
        namesvar = (char**)calloc(MAX_VAR_NUM, sizeof(char*));
        root = new Tree();
        root->AddChild(NodeType::Var_init, 0);
        //root->AddChild(NodeType::Main, 0);
    }

private:

    int number_of_func;
    int number_of_vars;
    char** namesfunc;
    char** namesvar;
    Tree* root;

};




void DeleteSpaces (char* &str)
{
    bool is_space = false;
    char* tmp = (char*)calloc(strlen(str), sizeof(char));
    int cur = 0;
    int l = 0;
    int r = strlen(str) - 1;
    while (str[l] == ' ' || str[l] == '\t')
        l++;

    while (str[r] == ' ' || str[r] == '\t' || str[r] == '\n')
        r--;

    for(int i = l; i<= r; i++)
    {
        if(str[i] == ' ')
        {
            if(is_space) continue;
            else
            {
                tmp[cur] = str[i];
                cur++;
                is_space = true;
            }
        }

        else
        {
            tmp[cur] = str[i];
            cur++;
            is_space = false;
        }
    }
    //printf("%s\n", tmp);
    //free (str);
    str = tmp;
}

int SubmissionIndex(char * str)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == '=') return i;
	}
	return -1;
}

char* Parser::first_word (char* str)
{
    char* temp = (char*)calloc (1000, sizeof(char));
    for (int i = 0; str[i] != ' ' && i < strlen(str); i++)
    {
        temp[i] = str[i];
    }
    return temp;
}


Tree* Parser::Parse_Body (FILE*f, bool &flag)
{
    char* temp = (char*)calloc (1000, sizeof(char));
    Tree*cur = new Tree ();
    while (fgets(temp, 1000, f) != NULL)
    {
        DeleteSpaces (temp);
        //std::cerr << temp << std::endl;

        char *first_word_str = first_word(temp);
        if (strcmp (first_word_str, END_KEYWORD) == 0)
        {
            flag = false;
            free(first_word_str);
            free(temp);
            return cur;
        }

        if (strcmp (first_word_str, ELSE_KEYWORD) == 0)
        {
            flag = true;
            free(temp);
            free(first_word_str);
            return cur;
        }

        if(strcmp(first_word(temp), IF_KEYWORD) == 0)
        {
            cur->AddChild (parse_if(f, temp));
            continue;
        }

        if(strcmp(first_word(temp), WHILE_KEYWORD) == 0)
        {
            cur->AddChild (parse_while(f, temp));
            continue;
        }
        //temp[10] = 0;
		int index = SubmissionIndex(temp);
        OperatorType op_submission = OperatorType::Equal;
        int len_op = 1;
        if (index != -1)
        {
            if (temp[index - 1] == '+')
            {
                op_submission = OperatorType::Plus_equal;
                len_op++;
            }
            if (temp[index - 1] == '-')
            {
                op_submission = OperatorType::Minus_equal;
                len_op++;
            }
            if (temp[index - 1] == '*')
            {
                op_submission = OperatorType::Multiply_equal;
                len_op++;
            }
            if (temp[index - 1] == '/')
            {
                op_submission = OperatorType::Divide_equal;
                len_op++;
            }
			if (temp[index + 1] == '=')
			{
				index = -1;
			}
			if (temp[index - 1] == '!')
			{
				index = -1;
			}
			if (temp[index - 1] == '<')
			{
				index = -1;
			}
			if (temp[index - 1] == '>')
			{
				index = -1;
			}

        }
		if (index == -1)
		{
            printf("MYNUM: %d\n", number_of_func);
			Parser_Tree pt(temp, number_of_func, namesfunc);
			cur->AddChild(pt.export(number_of_vars, namesvar, number_of_vars, namesvar));
		}
		else
		{
            Parser_Tree pt2(&temp[index + 1], number_of_func, namesfunc);
            printf("-->%s\n", &temp[index + 1]);
			temp[index - len_op + 1] = 0;
            printf("-->%s\n", temp);
            Parser_Tree pt1(temp, number_of_func, namesfunc);

			cur->AddChild(NodeType::Operator, op_submission);
			cur->childrens[cur->size - 1]->AddChild(pt1.export(number_of_vars, namesvar, number_of_func, namesfunc));
			cur->childrens[cur->size - 1]->AddChild(pt2.export(number_of_vars, namesvar, number_of_func, namesfunc));
		}

		//std::cout << "out";
		//cur->AddChild(NodeType::Standart_function, StandartFunction::Output);
    }
    return cur;
}


Tree* Parser::parse_while_condition(char *str)
{
	Tree* cur = new Tree();

	int pos_left = 0;
	int pos_right = strlen(str) - 1;
	for (; pos_left < strlen(str) && str[pos_left] != '('; pos_left++);
	for (; pos_right >= 0 && str[pos_right] != ')'; pos_right--);
	char* temp = (char*)calloc(100, sizeof(char));

	for (int i = pos_left + 1, j = 0; i < pos_right; i++, j++)
	{
		temp[j] = str[i];
	}
	cur->type = NodeType::Logic;
	cur->value = (double)Logic1::Condition;

    Parser_Tree pt(temp, number_of_func, namesfunc);
	cur->AddChild(pt.export(number_of_vars, namesvar, number_of_func, namesfunc));

	return cur;
}


Tree* Parser::parse_while(FILE*f, char* str)
{
    bool flag;
    Tree* cur = new Tree ();
    cur->type = NodeType::Logic;
    cur->value = (double)Logic1::While;

    cur->AddChild(parse_while_condition (str));


    Tree* body_while = Parse_Body(f, flag);
    body_while->type = NodeType::Logic;
    body_while->value = (double)Logic1::Condition_met;
    cur->AddChild(body_while);

    return cur;
}

Tree* Parser::parse_if(FILE*f, char* str)
{
    //std::cerr << "#if" << std::endl;
    bool flag;
    Tree* cur = new Tree ();
    cur->type = NodeType::Logic;
    cur->value = (double)Logic1::If;

    cur->AddChild(parse_if_condition (str));

    Tree* body_if = Parse_Body(f, flag);
    body_if->type = NodeType::Logic;
    body_if->value = (double)Logic1::Condition_met;
    cur->AddChild(body_if);

    if(flag)
    {
        Tree* body_else = Parse_Body(f, flag);
        body_else->type = NodeType::Logic;
        body_else->value = (double)Logic1::Else;
        cur->AddChild(body_else);
    }
    //std::cerr << "#ifend" << std::endl;
    return cur;
}

Tree* Parser::parse_if_condition(char *str)
{
    Tree* cur = new Tree ();
    
	int pos_left = 0;
	int pos_right = strlen(str) - 1;
	for (; pos_left < strlen(str) && str[pos_left] != '('; pos_left++);
	for (; pos_right >= 0 && str[pos_right] != ')'; pos_right--);
	char* temp = (char*)calloc(100, sizeof(char));

	for (int i = pos_left + 1, j = 0; i < pos_right; i++, j++)
	{
		temp[j] = str[i];
	}
	cur->type = NodeType::Logic;
	cur->value = (double)Logic1::Condition;
	
    Parser_Tree pt(temp, number_of_func, namesfunc);
	cur->AddChild(pt.export(number_of_vars, namesvar, number_of_func, namesfunc));

    return cur;
}



void Parser::ReadFile (const char* name)
{
    char* temp = (char*)calloc (1000, sizeof(char));
    FILE *f1 = fopen(name, "r");
    while (fgets(temp, 1000, f1) != NULL)
    {
        SearchVars(temp);
        if (SearchFuncs (temp))
        {
            bool tmp_flag;
            Tree* tree_func = Parse_Body (f1, tmp_flag);
            if(strcmp(namesfunc[number_of_func - 1], "main") == 0)
            {
                tree_func->type = NodeType::Main;
            }
            else
            {
                tree_func->type = NodeType::User_function;
            }
            root->AddChild(tree_func);
        }

    }

    for (int i = 0; i < root->size; i++)
    {
        if(root->childrens[i]->type == NodeType::Main)
        {
            swap(root->childrens[i], root->childrens[root->size - 1]);
            break;
        }
    }


    fclose(f1);
}

bool Parser::SearchFuncs (char* str)
{
    char* tmp = (char*) calloc (1000, sizeof (char));
    char* normalized = (char*) calloc (1000, sizeof (char));
    strcpy (normalized, str);
    DeleteSpaces (normalized);

    int i = 0;
    int j = 0;

    while ( (normalized[i] != ' ') && (i < strlen (normalized)))
    {
        tmp[j] = normalized[i];
        j++;
        i++;
    }

    if (strcmp (tmp, DEF_KEYWORD) != 0)
    {
        return false;
    }

    i++;
    fill_n(tmp, 1000, 0);
    j = 0;

    while ( (normalized[i] != ' ') && (normalized[i] != '(') && (i < strlen (normalized)))
    {
        tmp[j] = normalized[i];
        j++;
        i++;
    }
    namesfunc[number_of_func] = (char *)calloc(1000, sizeof(char));
    strcpy (namesfunc[number_of_func], tmp);
    printf("NEW: %s\n", namesfunc[number_of_func]);
    number_of_func++;
    return true;
}


bool Parser::SearchVars (char* str)
{
    char* tmp = (char*) calloc (1000, sizeof (char));
    char* normalized = (char*) calloc (1000, sizeof (char));
    strcpy (normalized, str);
    DeleteSpaces (normalized);

    int i = 0;
    int j = 0;

    while ( (normalized[i] != ' ') && (i < strlen (normalized)))
    {
        tmp[j] = normalized[i];
        j++;
        i++;
    }

    if (strcmp (tmp, VAR_KEYWORD) != 0)
    {
        return false;
    }

    i++;
    fill_n(tmp, 1000, 0);
    j = 0;

    while ( (normalized[i] != ' ') && (normalized[i] != '=') && (i < strlen (normalized)))
    {
        tmp[j] = normalized[i];
        j++;
        i++;
    }

    namesvar[number_of_vars] = (char *)calloc(1000, sizeof(char));
    strcpy (namesvar[number_of_vars], tmp);

    j = 0;
    while (!isdigit(normalized[i]) && (i < strlen (normalized)))
    {
        i++;
    }

    while (i < strlen (normalized))
    {
        j = j*10 + normalized[i] - '0';
        i++;

    }

    root->childrens[0]->AddChild(NodeType::Variable, number_of_vars);
    root->childrens[0]->childrens[number_of_vars]->AddChild(NodeType::Number, (double)j);

    //printf ("%s", normalized);
    number_of_vars++;
    return true;
}

void writeTree(Tree *cur, FILE *f, int level = 0)
{
	if (cur == NULL) {
		cerr << "NULL ";
	}
	else {
		cerr << cur->type << " " << cur->value << endl;
	}
    if (cur == NULL)
    {
        return;
    }
	if (level == 0 || level != 0 && cur->type != 0)
	{
		cerr << "IN\n";
		fprintf(f, "[ %d %d ", cur->type, int(cur->value));
	}
    for (int i = 0; i < cur->size; i++)
    {
        writeTree(cur->childrens[i], f, level+1);
    }
	if (level == 0 || level != 0 && cur->type != 0)
	{
		cerr << "OUT\n";
		fprintf(f, "NULL ] ");
	}
    
}

void Parser::WriteToFile (const char* name)
{
    FILE* f1 = fopen (name, "w");

    fprintf(f1, "1 %d\n", PERSONAL_CODE);

    fprintf(f1, "FUNCS %d\n", number_of_func - 1);

    for (int i = 0; i < number_of_func; i++)
    {
        // if name is main then continue
        if (strcmp(namesfunc[i], "main") == 0)
            continue;
        fprintf (f1, "%s\n", namesfunc[i]);
    }

    fprintf(f1, "VARS %d\n", number_of_vars);

    for (int i = 0; i < number_of_vars; i++)
    {
        fprintf (f1, "%s\n", namesvar[i]);
    }

    writeTree(root, f1);

    fclose(f1);

}

int main(int argc, char **argv)
{
	try 
	{
		Parser p;
		///p.ReadFile("simple_in.txt");
		//p.WriteToFile("simple_out.txt");

		p.ReadFile(argv[1]);
		p.WriteToFile(argv[2]);
	}
	catch (...)
	{

	}

	//int x;
	//cin >> x;

    return 0;
}
