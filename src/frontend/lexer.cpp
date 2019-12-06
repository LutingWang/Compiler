/**********************************************
    > File Name: lexer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 10:24:07 2019
 **********************************************/

#include <algorithm>
#include <cassert>
#include <vector>
#include "compilerConfig.h"
#include "error.h"

#include "./InputFile.h"
#include "./Symbol.h"

#include "./Lexer.h"

InputFile* Lexer::_input = nullptr;

const InputFile& Lexer::input(void) {
    return *_input;
}

void Lexer::init(const std::string& inputPath) {
    _input = new InputFile(inputPath);
}

void Lexer::deinit(void) {
    delete _input;
}

namespace {
	std::vector<symbol::Symbol> tracebackStack;

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
	std::vector<std::string> reserved = {
		"const", "int", "char", "void", "main", "if", "else",
		"do", "while", "for", "scanf", "printf", "return"
	};
	
	// dependent on enum Delim
	std::string delim = "=;,()[]{}";
	
	// dependent on enum Oper
	std::string oper = "+-*/";
	
	// input file buffer
	std::string buf;
	int counter = 0;
}
	
// <iden> ::= <alpha>{<alpha>|<digit>}
// parse for identifiers or reserved words
void Lexer::_parseToken(void) {
	assert(CType::isalpha(buf[counter])); // ensured by outer function
	int i; // end index of token (not included)
	for (i = counter + 1; i < buf.size() && CType::isalnum(buf[i]); i++);
	std::string token = buf.substr(counter, i - counter); 
	auto it = find(reserved.begin(), reserved.end(), token);
	if (it == reserved.end()) { // not in the list of reserved words
		sym._id = symbol::Type::IDENFR;
		sym._str = token;
	} else {
		sym.set(symbol::Type::RESERVED, 1 << (it - reserved.begin()));
	}
	counter = i;
}

// <unsigned int> ::= <non-zero digit>{<digit>}|0
void Lexer::_parseInt(void) {
	assert(CType::isdigit(buf[counter])); // ensured by outer function
	sym.set(symbol::Type::INTCON, buf[counter] - '0');
	counter++;
	if (sym.num() == 0) return;
	while (counter < buf.size() && CType::isdigit(buf[counter])) {
		sym._num = sym._num * 10 + (buf[counter] - '0');
		counter++;
	}
}

// <char> ::= '(+|-|*|/|<alpha>|<digit>)'
void Lexer::_parseChar(void) {
	assert(buf[counter] == '\''); // ensured by outer function
	sym._id = symbol::Type::CHARCON;
	counter++;
	if (counter >= buf.size()) {
		error::raise(error::Code::LEX);
		return;
	}
	sym._ch = buf[counter];
	counter++;
	if (counter >= buf.size() || buf[counter] == '\'') { counter++; } 
	else { error::raise(error::Code::LEX); }

	if (sym.ch() != '+' && sym.ch() != '-' && sym.ch() != '*' &&
			sym.ch() != '/' && !CType::isalnum(sym.ch())) {
		error::raise(error::Code::LEX);
	}
}

// <string> ::= "{ ASCII 32, 33, 35-126 }"
// note : ASCII 34 is '"'
// after the starting '"', function tries to find the corresponding
// ending '"' till the end of line
void Lexer::_parseStr(void) {
	assert(buf[counter] == '"'); // ensured by outer function
	sym._id = symbol::Type::STRCON;
	counter++;
	int i;
	for (i = counter; i < buf.size() && buf[i] != '"'; i++);
	sym._str = buf.substr(counter, i - counter);
	if (i == buf.size()) { // string contains space
		counter = buf.size();
		for (char c = _input->get(); c != '"'; c = _input->get()) {
			if (c == '\n') {
				error::raise(error::Code::LEX);
				return;
			}
			sym._str += c;
		}
	} else { counter = i + 1; }

	for (int i = 0; i < sym.str().size(); i++) {
		if (sym.str()[i] == '\\') {
            i++;
            sym._str.insert(i, "\\");
		}
	}

	// non-structural error
	for (char c : sym.str()) {
		if (c < 32 || c > 126) {
			error::raise(error::Code::LEX);
			break;
		}
	}
}

#if !judge
extern std::ofstream lexer_output;
#endif /* judge */

void Lexer::getsym(void) {
	// check if traceback is available
	if (!tracebackStack.empty()) {
		sym = tracebackStack.back();
		tracebackStack.pop_back();
#if !judge
		lexer_output << "lexer: retracting " << sym << std::endl;
#endif /* judge */
		return;
	}

start:
	sym._lastLine = input().line();

	// check if buffer requires update
	assert(counter <= buf.size());
	if (counter >= buf.size()) { 
		*_input >> buf;
		counter = 0; // update success
	}

	// main body of the parser
	if (CType::isalpha(buf[counter])) { _parseToken(); } 
	else if (CType::isdigit(buf[counter])) { _parseInt(); } 
	else switch (buf[counter]) {
	case '\'': 
		_parseChar();
		break;
	case '"':
		_parseStr();
		break;
	case '<':
		if (buf[counter + 1] == '=') {
			sym.set(symbol::Type::COMP, symbol::LEQ);
			counter += 2;
		} else {
			sym.set(symbol::Type::COMP, symbol::LSS);
			counter++;
		}
		break;
	case '>':
		if (buf[counter + 1] == '=') {
			sym.set(symbol::Type::COMP, symbol::GEQ);
			counter += 2;
		} else {
			sym.set(symbol::Type::COMP, symbol::GRE);
			counter++;
		}
		break;
	case '!':
		sym.set(symbol::Type::COMP, symbol::NEQ);
		counter++;
		if (buf[counter] == '=') { counter++; } 
		else { error::raise(error::Code::LEX); }
		break;
	case '=':
		if (buf[counter + 1] == '=') {
			sym.set(symbol::Type::COMP, symbol::EQL);
			counter += 2;
		} else {
			sym.set(symbol::Type::DELIM, symbol::ASSIGN);
			counter++;
		}
		break;
	default:
		int pos;
		if ((pos = delim.find(buf[counter])) != delim.npos) {
			sym.set(symbol::Type::DELIM, 1 << pos);
		} else if ((pos = oper.find(buf[counter])) != oper.npos) {
			sym.set(symbol::Type::OPER, 1 << pos);
		} else {
			error::raise(error::Code::LEX);
			counter++;
			goto start;
		}
		counter++;
	}
#if !judge
	lexer_output << sym << std::endl;
#endif /* judge */
}

void Lexer::traceback(const symbol::Symbol& lastSymbol) {
	tracebackStack.push_back(sym);
	sym = lastSymbol;
#if !judge
	lexer_output << "lexer: tracing back to " << sym << std::endl;
#endif /* judge */
}

