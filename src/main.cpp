/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <iostream>
#include <string>
#include "compiler.h"
#include "compilerConfig.h"
#include "error.h"
#include "InputFile.h"
#include "MidCode.h"
#include "Optim.h"
using namespace std;

// relative to this file, for debug use
#define TESTFILE_PATH "../test/testfile1"

InputFile input(PROJECT_SRC_DIR TESTFILE_PATH);

// Latent streams for corresponding classes to use.
// Do not expose in the headers!
//
// Naming convensions:
//     *_output		- ofstream
//     OUTPUT_*		- bool			output switch
ofstream error_output;
ofstream symtable_output;
ofstream lexer_output;
ofstream midcode_output;

int main() {
	bool OUTPUT_error = true;
	bool OUTPUT_symtable = true;
	bool OUTPUT_lexer = true;
	bool OUTPUT_midcode = true;

#ifdef OPEN
	#error macro conflict
#endif /* OPEN */
#define OPEN(id) if (OUTPUT_##id) {									\
		id##_output.open(PROJECT_SRC_DIR TESTFILE_PATH "." #id);	\
		id##_output << std::left;									\
	} else { id##_output.setstate(iostream::failbit); }

	OPEN(error); OPEN(symtable); OPEN(lexer); OPEN(midcode);
#undef OPEN

	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR 
		<< "." << COMPILER_VERSION_MINOR << endl;

	grammar::parse();

	if (error::happened) { goto exit; }

	MidCode::output();

	// Optim::inlineExpan();
	Optim::commonExprElim();

	MidCode::output();

exit:
	error_output.close();
	symtable_output.close();
	lexer_output.close();
	midcode_output.close();
	return 0;
}
