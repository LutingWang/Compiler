/**********************************************
    > File Name: lexer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 10:24:07 2019
 **********************************************/

#include <algorithm>
#include <vector>
#include <cctype>
#include "compiler.h"
#include "lexer.h"
using namespace std;

struct lexer::Symbol sym;

// dependent on enum Reserved
static vector<string> reserved = {
	"const", "int", "char", "void", "main", "if", "else",
	"do", "while", "for", "scanf", "printf", "return"
};

// dependent on enum Delim
static string delim = "=;,()[]{}";

// dependent on enum Oper
static string oper = "+-*/";

static string buf;
static int counter = 0;

// extended version of `isalpha`
// do not use `isalpha` any more
static bool isletter(char c) {
	return c == '_' || isalpha(c);
}

// parse for identifiers or reserved words
static void parseToken(void) {
	int i;
	for (i = counter + 1; i < buf.size() && 
			(isletter(buf[i]) || isdigit(buf[i])); i++);
	string s = buf.substr(counter, i - counter);
	vector<string>::iterator it = find(reserved.begin(), reserved.end(), s);
	if (it == reserved.end()) {
		sym.id = lexer::IDENFR;
		sym.str = s;
	} else {
		sym.id = lexer::RESERVED;
		sym.num = (it - reserved.begin());
	}
	counter = i;
}

// parse for int constants
static void parseInt(void) {
	sym.id = lexer::INTCON;
	sym.num = buf[counter] - '0';
	counter++;
	if (sym.num == 0) return;
	while (counter < buf.size() && isdigit(buf[counter])) {
		sym.num = sym.num * 10 + (buf[counter] - '0');
		counter++;
	}
}

// parse for char constants
static void parseChar(void) {
	sym.id = lexer::CHARCON;
	sym.ch = buf[counter + 1];
	counter += 3;
}

// parse for string constants
static void parseStr(void) {
	sym.id = lexer::STRCON;
	counter++;
	int i;
	for (i = counter; buf[i] != '"'; i++);
	sym.str = buf.substr(counter, i - counter);
	counter = i + 1;
}

// set the corresponding members of a symbol
static void setSymbol(lexer::Type id, int num) {
	sym.id = id;
	sym.num = num;
}

bool lexer::getsym() {
	if (counter >= buf.size()) {
		input >> buf;
		if (input.eof()) return false;
		counter = 0;
	}
	if (isletter(buf[counter])) {
		parseToken();
	} else if (isdigit(buf[counter])) {
		parseInt();
	} else switch (buf[counter]) {
		case '\'': 
			parseChar();
			break;
		case '"':
			parseStr();
			break;
		case '<':
			if (buf[counter + 1] == '=') {
				setSymbol(COMP, LEQ);
				counter += 2;
			} else {
				setSymbol(COMP, LSS);
				counter++;
			}
			break;
		case '>':
			if (buf[counter + 1] == '=') {
				setSymbol(COMP, GEQ);
				counter += 2;
			} else {
				setSymbol(COMP, GRE);
				counter++;
			}
			break;
		case '!':
			setSymbol(COMP, NEQ);
			counter += 2;
			break;
		case '=':
			if (buf[counter + 1] == '=') {
				setSymbol(COMP, EQL);
				counter += 2;
			} else {
				setSymbol(DELIM, ASSIGN);
				counter++;
			}
			break;
		default:
			int pos;
			if ((pos = delim.find(buf[counter])) != delim.npos) {
				setSymbol(DELIM, pos);
			} else if ((pos = oper.find(buf[counter])) != oper.npos) {
				setSymbol(OPER, pos);
			}
			counter++;
	}
	return true;
}

void lexer::test(ofstream& output) {
	while (getsym()) {
		switch (sym.id) {
		case IDENFR:
			output << "IDENFR " << sym.str << endl;
			break;
		case INTCON:
			output << "INTCON " << sym.num << endl;
			break;
		case CHARCON:
			output << "CHARCON " << sym.ch << endl;
			break;
		case STRCON:
			output << "STRCON " << sym.str << endl;
			break;
		case RESERVED:
			switch (sym.num) {
			case CONSTTK: 
				output << "CONSTTK const" << endl;
				break;
			case INTTK: 
				output << "INTTK int" << endl;
				break;
			case CHARTK: 
				output << "CHARTK char" << endl;
				break;
			case VOIDTK: 
				output << "VOIDTK void" << endl;
				break;
			case MAINTK: 
				output << "MAINTK main" << endl;
				break;
			case IFTK: 
				output << "IFTK if" << endl;
				break;
			case ELSETK: 
				output << "ELSETK else" << endl;
				break;
			case DOTK: 
				output << "DOTK do" << endl;
				break;
			case WHILETK: 
				output << "WHILETK while" << endl;
				break;
			case FORTK: 
				output << "FORTK for" << endl;
				break;
			case SCANFTK: 
				output << "SCANFTK scanf" << endl;
				break;
			case PRINTFTK: 
				output << "PRINTFTK printf" << endl;
				break;
			case RETURNTK:
				output << "RETURNTK return" << endl;
				break;
			default:
				throw "unknown reserved word category";
			}
			break;
		case DELIM:
			switch (sym.num) {
			case ASSIGN: 
				output << "ASSIGN =" << endl;
				break;
			case SEMICN: 
				output << "SEMICN ;" << endl;
				break;
			case COMMA: 
				output << "COMMA ," << endl;
				break;
			case LPARENT: 
				output << "LPARENT (" << endl;
				break;
			case RPARENT: 
				output << "RPARENT )" << endl;
				break;
			case LBRACK: 
				output << "LBRACK [" << endl;
				break;
			case RBRACK: 
				output << "RBRACK ]" << endl;
				break;
			case LBRACE: 
				output << "LBRACE {" << endl;
				break;
			case RBRACE:
				output << "RBRACE }" << endl;
				break;
			default:
				throw "unknown delimiter category";
			}
			break;
		case OPER:
			switch (sym.num) {
			case PLUS:
				output << "PLUS +" << endl;
				break;
			case MINU: 
				output << "MINU -" << endl;
				break;
			case MULT: 
				output << "MULT *" << endl;
				break;
			case DIV:
				output << "DIV /" << endl;
				break;
			default:
				throw "unknown operator category";
			}
			break;
		case COMP:
			switch (sym.num) {
			case LSS: 
				output << "LSS <" << endl;
				break;
			case LEQ: 
				output << "LEQ <=" << endl;
				break;
			case GRE: 
				output << "GRE >" << endl;
				break;
			case GEQ:
				output << "GEQ >=" << endl;
				break;
			case EQL: 
				output << "EQL ==" << endl;
				break;
			case NEQ:
				output << "NEQ !=" << endl;
				break;
			default:
				throw "unknown comparator category";
			}
			break;
		default:
			throw "unknown symbol type id";
		}
	}
}
