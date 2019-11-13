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
#include "midcode.h"
#include "InputFile.h"
#include "Mips.h"
#include "Optim.h"
using namespace std;

// TODO: save ra once entering function
// TODO: change midcode naming to avoid conflict
// TODO: scan iterations, add comment of type

// relative to this file, for debug use
#define TESTFILE_PATH "../test/testfile1"

#if judge
InputFile input("testfile.txt");
#else
InputFile input(PROJECT_SRC_DIR TESTFILE_PATH);
#endif /* judge */

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
ofstream mips_output;

int main() {
	bool OUTPUT_error = true;
	bool OUTPUT_symtable = true;
	bool OUTPUT_lexer = true;
	bool OUTPUT_midcode = true;
	bool OUTPUT_mips = true;

#define OPEN(id) if (OUTPUT_##id) {							\
		id##_output.open(judge ?							\
				PROJECT_SRC_DIR TESTFILE_PATH "." #id :		\
				#id ".txt");								\
		id##_output << std::left;							\
	} else { id##_output.setstate(iostream::failbit); }

	OPEN(error); OPEN(symtable); OPEN(lexer); OPEN(midcode); OPEN(mips);
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

	Mips::getInstance().output();

exit:
	error_output.close();
	symtable_output.close();
	lexer_output.close();
	midcode_output.close();
	mips_output.close();
	return 0;
}
