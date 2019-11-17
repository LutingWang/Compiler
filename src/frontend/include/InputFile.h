/**********************************************
    > File Name: InputFile.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Oct  2 15:30:48 2019
 **********************************************/

#ifndef INPUT_FILE_H
#define INPUT_FILE_H

#include <fstream>
#include <sstream>

class InputFile {
	std::ifstream _input;
	std::stringstream _buf;
	int _nline = 0;

	void updateBuf(void);
public:
	InputFile(std::string);

	~InputFile(void) { _input.close(); }

	int line(void) const { return _nline; }

	void operator >> (std::string&);

	char get(void);
};

#endif /* INPUT_FILE_H */
