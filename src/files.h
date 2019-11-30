/**********************************************
    > File Name: files.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov 30 12:50:34 2019
 **********************************************/

#ifndef FILES_H
#define FILES_H

#include <string>
#include "compilerConfig.h"

namespace files {
#if judge
	void open(void);
#else
	void open(const std::string&);
#endif /* judge */	

	void close(void);
}

#endif /* FILES_H */
