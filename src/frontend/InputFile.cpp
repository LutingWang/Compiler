/**********************************************
    > File Name: InputFile.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Oct  2 15:40:00 2019
 **********************************************/

#include <cassert>
#include <string>
#include "compilerConfig.h"

#include "./include/errors.h"
#include "./include/InputFile.h"
using namespace std;

InputFile input(TESTFILE_PATH);

InputFile::InputFile(string path) : _input(path) {
	assert(_input);
}

// try to update file buffer
// if reaching EOF, throw error
void InputFile::updateBuf(void) {
	assert(!_buf); // ensured by outer function
	if (!_input) { throw error::Ueof(); }
	_buf.clear();
	_nline++;
	string line;
	getline(_input, line);
	if (!_input) { throw error::Ueof(); }
	_buf.str(line);
}

void InputFile::operator >> (string& s) {
	for (_buf >> s; !_buf; _buf >> s) {
		updateBuf();
	}
}

char InputFile::get(void) {
	char result = _buf.get();
	if (!_buf) {
		updateBuf();
		return '\n';
	}
	return result;
}
