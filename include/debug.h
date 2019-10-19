/**********************************************
    > File Name: debug.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sun Sep 29 21:59:58 2019
 **********************************************/

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <fstream>
#include <string>

class Debugger {
	bool _isFile;
	std::ostream _output;
public:
	Debugger() : _isFile(false), _output(std::cout.rdbuf()) {}

	Debugger(const std::string& path) : 
		_isFile(true), _output(std::ofstream(path).rdbuf()) {}

	~Debugger() { 
		if (_isFile) { 
			((std::ofstream&) _output).close(); 
		} 
	}

	template<typename T> std::ostream& operator << (const T& e) {
		_output << e;
		return _output;
	}

	std::ostream& operator << (std::ostream& (*op)(std::ostream&)) {
		return (*op)(_output);
	}
};

#endif /* DEBUG_H */
