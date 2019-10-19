/**********************************************
    > File Name: lexer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 10:24:07 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <vector>
#include "compiler.h"
#include "error.h"
using namespace std;

namespace {
	vector<symbol::Symbol> tracebackStack;

	// tool class as overloading of cctype
	class CType {
	public:
		// <alpha> ::= _|a-z|A-Z
		static bool isalpha(char c) {
			return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
		}
	
		// <digit> ::= 0-9
		static bool isdigit(char c) { return '0' <= c && c <= '9'; }
	
		static bool isalnum(char c) { return isalpha(c) || isdigit(c); }
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
	
	// input file buffer
	string buf;
	int counter = 0;
	
	// parse for identifiers or reserved words
	// <iden> ::= <alpha>{<alpha>|<digit>}
	void parseToken(void) {
		assert(CType::isalpha(buf[counter])); // ensured by outer function
		int i; // end index of token (not included)
		for (i = counter + 1; i < buf.size() && CType::isalnum(buf[i]); i++);
		string token = buf.substr(counter, i - counter); 
		vector<string>::iterator it = find(reserved.begin(), reserved.end(), token);
		if (it == reserved.end()) {
			sym.id = symbol::IDENFR;
			sym.str = token;
		} else {
			sym.set(symbol::RESERVED, 1 << (it - reserved.begin()));
		}
		counter = i;
	}
	
	// <unsigned int> ::= <non-zero digit>{<digit>}|0
	void parseInt(void) {
		assert(CType::isdigit(buf[counter])); // ensured by outer function
		sym.set(symbol::INTCON, buf[counter] - '0');
		counter++;
		if (sym.num == 0) return;
		while (counter < buf.size() && CType::isdigit(buf[counter])) {
			sym.num = sym.num * 10 + (buf[counter] - '0');
			counter++;
		}
	}
	
	// <char> ::= '(+|-|*|/|<alpha>|<digit>)'
	void parseChar(void) {
		assert(buf[counter] == '\''); // ensured by outer function
		sym.id = symbol::CHARCON;
		counter++;
		assert(counter < buf.size());
		sym.ch = buf[counter];
		counter++;
		assert (counter < buf.size());
		counter++;

		if (sym.ch != '+' && sym.ch != '-' && sym.ch != '*' && 
				sym.ch != '/' && !CType::isalnum(sym.ch)) {
			err << error::LEX << endl;
		}
	}
	
	// <string> ::= "{ ASCII 32, 33, 35-126 }"
	// note : ASCII 34 is '"'
	// after the starting '"', function tries to find the corresponding
	// ending '"' till the end of line
	void parseStr(void) {
		assert(buf[counter] == '"'); // ensured by outer function
		sym.id = symbol::STRCON;
		counter++;
		int i;
		for (i = counter; i < buf.size() && buf[i] != '"'; i++);
		sym.str = buf.substr(counter, i - counter);
		if (i == buf.size()) { // string contains space
			counter = buf.size();
			for (char c = input.get(); c != '"'; c = input.get()) {
				if (c == '\n') {
					err << error::LEX << endl;
					return;
				}
				sym.str += c;
			}
		} else { counter = i + 1; }

		// non-structural error
		for (char c : sym.str) {
			if (c < 32 || c > 126) {
				err << error::LEX << endl;
				break;
			}
		}
	}
}

void lexer::getsym(void) {
	// check if traceback is available
	if (!tracebackStack.empty()) {
		sym = tracebackStack.back();
		tracebackStack.pop_back();
		log << "lexer: retracting " << sym << endl;
		return;
	}

start:
	// check if buffer requires update
	assert(counter <= buf.size());
	if (counter >= buf.size()) { 
		input >> buf;
		counter = 0; // update success
	}

	// main body of the parser
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
			sym.set(symbol::COMP, symbol::LEQ);
			counter += 2;
		} else {
			sym.set(symbol::COMP, symbol::LSS);
			counter++;
		}
		break;
	case '>':
		if (buf[counter + 1] == '=') {
			sym.set(symbol::COMP, symbol::GEQ);
			counter += 2;
		} else {
			sym.set(symbol::COMP, symbol::GRE);
			counter++;
		}
		break;
	case '!':
		sym.set(symbol::COMP, symbol::NEQ);
		counter++;
		assert(buf[counter == '=']);
		counter++;
		break;
	case '=':
		if (buf[counter + 1] == '=') {
			sym.set(symbol::COMP, symbol::EQL);
			counter += 2;
		} else {
			sym.set(symbol::DELIM, symbol::ASSIGN);
			counter++;
		}
		break;
	default:
		int pos;
		if ((pos = delim.find(buf[counter])) != delim.npos) {
			sym.set(symbol::DELIM, 1 << pos);
		} else if ((pos = oper.find(buf[counter])) != oper.npos) {
			sym.set(symbol::OPER, 1 << pos);
		} else {
			err << error::LEX << endl;
			counter++;
			goto start;
		}
		counter++;
	}
	log << sym << endl;
}

void lexer::traceback(const symbol::Symbol& lastSymbol) {
	tracebackStack.push_back(sym);
	sym = lastSymbol;
	log << "lexer: tracing back to " << sym << endl;
}

