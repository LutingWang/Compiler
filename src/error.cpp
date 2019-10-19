/**********************************************
    > File Name: error.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Oct 18 23:15:00 2019
 **********************************************/

#include "error.h"

std::ostream& error::operator << (std::ostream& output, Code c) {
	output << input.line() << ' ' << (char) (c + 'a');
	return output;
}
