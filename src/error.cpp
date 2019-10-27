/**********************************************
    > File Name: error.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Oct 18 23:15:00 2019
 **********************************************/

#include "error.h"

std::ostream& error::operator << (std::ostream& output, Code c) {
	switch (c) {
	case MISSING_SEMICN: 
	case MISSING_RPARENT:
	case MISSING_RBRACK:
	case MISSING_WHILE:
		output << sym.lastLine;
		break;
	default:
		output << input.line();
	}
	output << ' ' << (char) (c + 'a');
	return output;
}
