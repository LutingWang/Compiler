/**********************************************
    > File Name: lexer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 10:24:07 2019
 **********************************************/

#include <algorithm>
#include <vector>
#include <cassert>
#include "compiler.h"
#include "lexer.h"
#include "debug.h"
using namespace std;

struct lexer::Symbol sym;

namespace {
	vector<lexer::Symbol> tracebackStack;

	// tool class as overloading of cctype
	class CType {
	public:
		static bool isalpha(char c) {
			return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
		}
	
		static bool isdigit(char c) {
			return '0' <= c && c <= '9';
		}
	
		static bool isalnum(char c) {
			return isalpha(c) || isdigit(c);
		}
	};

	// dependent on enum Reserved
	vector<string> reserved = {
		"const", "int", "char", "void", "main", "if", "else",
		"do", "while", "for", "scanf", "printf", "return"
	};
	
	// dependent on enum Delim
	string delim = "=;,()[]{}";
	
	// dependent on enum Oper
	string oper = "+-*/";
	
	string buf;
	int counter = 0;
	
	// parse for identifiers or reserved words
	void parseToken(void) {
		// ensures that buf[counter] is letter
		int i; // end index of token (not included)
		for (i = counter + 1; i < buf.size() && CType::isalnum(buf[i]); i++);
		string token = buf.substr(counter, i - counter); 
		vector<string>::iterator it = find(reserved.begin(), reserved.end(), token);
		if (it == reserved.end()) {
			sym.id = lexer::IDENFR;
			sym.str = token;
		} else {
			sym.id = lexer::RESERVED;
			sym.num = (it - reserved.begin());
		}
		counter = i;
	}
	
	// parse for int constants
	void parseInt(void) {
		// ensures that buf[counter] is digit
		sym.id = lexer::INTCON;
		sym.num = buf[counter] - '0';
		counter++;
		if (sym.num == 0) return;
		while (counter < buf.size() && CType::isdigit(buf[counter])) {
			sym.num = sym.num * 10 + (buf[counter] - '0');
			counter++;
		}
	}
	
	// parse for char constants
	void parseChar(void) {
		// ensures that buf[counter] is '\''
		counter++;
		sym.id = lexer::CHARCON;
		sym.ch = buf[counter];
		counter++;
		assert(buf[counter] == '\'');
		counter++;
	}
	
	// parse for string constants
	void parseStr(void) {
		// ensures that buf[counter] is '"'
		sym.id = lexer::STRCON;
		counter++;
		int i;
		for (i = counter; i < buf.size() && buf[i] != '"'; i++);
		sym.str = buf.substr(counter, i - counter);
		if (i == buf.size()) { // string contains space
			for (char c = input.get(); c != '"'; c = input.get()) {
				sym.str += c;
			}
			counter = i;
		} else { counter = i + 1; }
	}
	
	// set the corresponding members of a symbol
	void setSymbol(const lexer::Type id, const int num) {
		sym.id = id;
		sym.num = num;
	}
}

bool lexer::getsym() {
	if (!tracebackStack.empty()) {
		sym = tracebackStack.back();
		tracebackStack.pop_back();
		v.push_back(print()); // <
		if (sym.id == INTCON) v.push_back(print(UNSIGNED_INT));
		if (sym.id == STRCON) v.push_back(print(STR)); // >
		return true;
	}
	assert(counter <= buf.size());
	if (counter >= buf.size()) { // buffer requires update
		if (input >> buf) { counter = 0; } // update success
		else { return false; } // reaches EOF of input
	}

	if (CType::isalpha(buf[counter])) { parseToken(); } 
	else if (CType::isdigit(buf[counter])) { parseInt(); } 
	else switch (buf[counter]) {
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
		counter++;
		assert(buf[counter == '=']);
		counter++;
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
		} else {
			assert(0);
		}
		counter++;
	}
	v.push_back(print()); // <
	if (sym.id == INTCON) v.push_back(print(UNSIGNED_INT));
	if (sym.id == STRCON) v.push_back(print(STR)); // >
	return true;
}

void lexer::traceback(const Symbol& lastSymbol) {
	v.pop_back(); // <
	if (sym.id == INTCON || sym.id == STRCON) v.pop_back(); // >
	tracebackStack.push_back(sym);
	sym = lastSymbol;
}

