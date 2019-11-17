/**********************************************
    > File Name: lexer.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 15 15:56:49 2019
 **********************************************/

#ifndef LEXER_H
#define LEXER_H

namespace symbol {
	class Symbol;
}

namespace lexer {
	void getsym(void);
	void traceback(const symbol::Symbol&);
}

#endif /* LEXER_H */
