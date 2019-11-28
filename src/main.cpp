/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <iostream>
#include <fstream>
#include <string>
#include "compilerConfig.h"
#include "error.h"
#include "frontend.h"
#include "midcode/MidCode.h"
#include "mips.h"
#include "Optim.h"
using namespace std;

// TODO: change midcode naming to avoid conflict
// TODO: scan iterations, add comment of type

#if judge
	#define TESTFILE_PATH "testfile.txt"
#else
	#define TESTFILE_PATH PROJECT_BASE_DIR "test/mips/qsort/qsort"
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
		id##_output.open(judge ? #id ".txt" :				\
				TESTFILE_PATH "." #id);		\
		id##_output << std::left;							\
	} else { id##_output.setstate(iostream::failbit); }

	OPEN(error); OPEN(symtable); OPEN(lexer); OPEN(midcode); OPEN(mips);
#undef OPEN

	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR 
		<< "." << COMPILER_VERSION_MINOR << endl;

	grammar::parse(TESTFILE_PATH);

	if (error::happened) { goto exit; }

	MidCode::output();

	Optim::inlineExpan();
	// Optim::commonExprElim();
    Optim::peephole();

	MidCode::output();

	mips::init();
	mips::output();
	mips::deinit();

exit:
	error_output.close();
	symtable_output.close();
	lexer_output.close();
	midcode_output.close();
	mips_output.close();
	return 0;
}
