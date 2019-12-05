/**********************************************
    > File Name: Lexer.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 15 15:56:49 2019
 **********************************************/

#ifndef LEXER_H
#define LEXER_H

#include <string>

namespace symbol {
	class Symbol;
}

class InputFile;

class Lexer {
	static InputFile* _input;
public:
	static const InputFile& input(void);

	static void init(const std::string&);
	static void deinit(void);

private:
	static void _parseToken(void);
	static void _parseInt(void);
	static void _parseChar(void);
	static void _parseStr(void);
public:
	static void getsym(void);
	static void traceback(const symbol::Symbol&);
};

#endif /* LEXER_H */
