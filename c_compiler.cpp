#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
#include <cctype>
#include <iostream>

#include "standart.h"

#define MAX_STRLEN 1000
const int DEF_FUNC_BUFFER2 = 9;
const char* DefinedFunctions3[DEF_FUNC_BUFFER2] = { "subzero", "scanf", "printf", "sin", "cos", "getch", "call", "differentiate", "sqrt" };
const char* DefinedOperators2[] = { "scorpion", "=", "+", "-", "*", "/", "^", "==", "!=", "<", ">", ">=", "<=", "!", "|", "&", "+=", "-=", "*=", "/=" };

//const char* DefinedFunctions[] = { "subzero", "read", "print", "sin", "cos", "getch", "call", "differentiate", "sqrt" };

class TreeReader2
{
public:

	void ParseTree(const char * fname_in, const char * fname_out);

private:

	bool Unknown(FILE* f_in, FILE* f_out, int level = 0, bool var_init = false, bool call = false);

	int number_of_func;
	int number_of_vars;
	char** namesfunc;
	char** namesvar;
};

void TreeReader2::ParseTree(const char * fname_in, const char * fname_out)
{
	FILE* f1 = fopen(fname_in, "r");
	int tempint = 0;
	fscanf(f1, "%d", &tempint);
	fscanf(f1, "%d\n", &tempint);

	fscanf(f1, "FUNCS %d\n", &number_of_func);
	namesfunc = (char**)calloc(number_of_func, sizeof(char*));

	for (int i = 0; i < number_of_func; i++)
	{
		namesfunc[i] = (char*)calloc(MAX_STRLEN, sizeof(char));
		fscanf(f1, "%s\n", namesfunc[i]);
	}

	fscanf(f1, "VARS %d\n", &number_of_vars);
	namesvar = (char**)calloc(number_of_vars, sizeof(char*));

	for (int i = 0; i < number_of_vars; i++)
	{
		namesvar[i] = (char*)calloc(MAX_STRLEN, sizeof(char));
		fscanf(f1, "%s\n", namesvar[i]);
	}

	FILE* f_out = fopen(fname_out, "w");

	fprintf(f_out, "#include <stdio.h>\n\n");
	fprintf(f_out, "using namespace std;\n\n");

	Unknown(f1, f_out);

	
	fclose(f1);
	fclose(f_out);
}


bool TreeReader2::Unknown(FILE* f_in, FILE* f_out, int level, bool var_init, bool call)
{
	char c = 0;
	int type = 0;
	double value = 0;
	fscanf(f_in, "%c", &c);
	bool is_operator = false;
	if (c == '[')
	{
		char *buf = (char *)calloc(MAX_STRLEN, sizeof(char));
		fscanf(f_in, "%d %lf ", &type, &value);
		int value_int = int(value + 0.0001);
		bool flag_call = false;
		// Write standart
		if (type == NodeType::Root)
		{
			// Nothing to do
		}
		if (type == NodeType::Logic)
		{
			if (value_int == Logic1::If)
			{
				//print_align(f_out, level);
				fprintf(f_out, "if ");
				sprintf(buf, "}\n");
			}
			if (value_int == Logic1::Condition)
			{
				//print_align(f_out, level);
				fprintf(f_out, "(");
				sprintf(buf, ") {\n");
			}
			if (value_int == Logic1::Condition_met)
			{
				// Nothing to do
			}
			if (value_int == Logic1::Else)
			{

				//print_align(f_out, level);
				fprintf(f_out, "} else {\n");
			}
			if (value_int == Logic1::While)
			{
				//print_align(f_out, level);
				fprintf(f_out, "while ");
				sprintf(buf, "}\n");
			}
		}
		if (type == NodeType::Main)
		{
			fprintf(f_out, "int main(){\n");
			sprintf(buf, "}\n");
		}
		if (type == NodeType::Number)
		{
			fprintf(f_out, "%.0lf", value);
		}
		if (type == NodeType::Operator)
		{
			if (value_int != 1 && value_int < 16)
				fprintf(f_out, "(");
			Unknown(f_in, f_out, level + 1);
			//fprintf(f_out, "+");
			//
			fprintf(f_out, DefinedOperators2[value_int]);
			//
			Unknown(f_in, f_out, level + 1);
			is_operator = true;
			if (value_int != 1 && value_int < 16)
			{
				fprintf(f_out, ")");
			}
			else
			{
				fprintf(f_out, "\n");
			}

		}
		if (type == NodeType::Service)
		{
			// Nothing to do
		}
		if (type == NodeType::Standart_function)
		{
			if (value_int == 1)
			{
				fprintf(f_out, "%s(\"%%lf\", &", DefinedFunctions3[value_int]);

			}
			else if (value_int == 9 || value_int == 10) 
			{
				// 
			}
			else
			{
				fprintf(f_out, "%s(\"%%lf\", ", DefinedFunctions3[value_int]);
			}
			sprintf(buf, ");\n");
			if (value_int == 6)
				flag_call = true;
		}
		if (type == NodeType::User_function)
		{
			if (call)
			{
				fprintf(f_out, "%s", namesfunc[value_int]);
				//sprintf(buf, "\n");
			}
			else
			{
				fprintf(f_out, "def %s()\n", namesfunc[value_int]);
				sprintf(buf, "end\n");
			}
		}
		if (type == NodeType::Variable)
		{
			if (var_init)
			{
				fprintf(f_out, "double ");
			}
			fprintf(f_out, "%s", namesvar[value_int]);
			if (var_init)
			{

				fprintf(f_out, " = ");
			}
		}
		if (type == NodeType::Var_init)
		{
			while (Unknown(f_in, f_out, level + 1, 1))
			{
				fprintf(f_out, ";\n");
			}
		}
		printf("%d %lf\n", type, value);
		while (Unknown(f_in, f_out, level + 1, 0, call));
		fprintf(f_out, "%s", buf);
		free(buf);
		return true;
	}

	else
	{
		fscanf(f_in, "ULL] ");
		return false;
	}

}

int main2()
{
	TreeReader2 tr;
	tr.ParseTree("simple_out.txt", "test.cpp");

	//system("C:\\mingw\\bin\\g++ test.cpp -o test.exe");
	//system("test.exe");

	return 0;
}

