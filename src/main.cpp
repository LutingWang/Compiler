/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include "compilerConfig.h"
#include "error.h"
#include "frontend.h"
#include "midcode.h"
#include "mips.h"
#include "Optim.h"
using namespace std;

// Latent streams for corresponding classes to use.
// Do not expose in the headers!
#define OUTPUT_STREAM(id) id##_output

#if judge
    std::ofstream OUTPUT_STREAM(error),
        OUTPUT_STREAM(mips);

    #define OUTPUT_PATH(id) #id ".txt"
    #define OPEN(id) \
        OUTPUT_STREAM(id).open(OUTPUT_PATH(id)); \
        OUTPUT_STREAM(id) << std::left;
    void open(void) {
        OPEN(error);
        OPEN(mips);
    }
#else
    std::ofstream OUTPUT_STREAM(error),
        OUTPUT_STREAM(symtable),
        OUTPUT_STREAM(lexer),
        OUTPUT_STREAM(midcode),
        OUTPUT_STREAM(mips);

    #define OUTPUT_PATH(id) testfile_path + "." #id
    #define OPEN(id) \
        OUTPUT_STREAM(id).open(OUTPUT_PATH(id)); \
        OUTPUT_STREAM(id) << std::left;
    void open(const std::string& testfile_path) {
        OPEN(error);
        OPEN(symtable);
        OPEN(lexer);
        OPEN(midcode);
        OPEN(mips);
    }
#endif /* judge */

void close(void) {
#if !judge
    OUTPUT_STREAM(error).close();
    OUTPUT_STREAM(symtable).close();
    OUTPUT_STREAM(lexer).close();
    OUTPUT_STREAM(midcode).close();
#endif /* judge */
    OUTPUT_STREAM(mips).close();
}

#if judge
int main(void) {
    const string testfile_path = "testfile.txt";
    open();
#else
int main(int argc, char* argv[]) {
	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR
        << "." << COMPILER_VERSION_MINOR << endl;

    assert(argc == 2);
    const string testfile_path = PROJECT_BASE_DIR + string(argv[1]);
    open(testfile_path);
#endif /* judge */
	
#if !judge
	cout << "grammar analysis processing ... ";
#endif /* judge */
    grammar::parse(testfile_path);
#if !judge
	cout << "finished" << endl;
#endif /* judge */

    if (error::happened) {
#if !judge
		cout << "WARNING: error detected, aborting compilation" << endl;
#endif /* judge */
		goto exit; 
	}

#if !judge
    cout << "midcode generating ... ";
    MidCode::output();
    cout << "finished" << endl;
#endif /* judge */

#if !judge
    cout << "optimization processing ... ";
#endif /* judge */
    for (bool updated = true; updated; ) {
        updated = false;
        Optim::inlineExpan(updated);
        // Optim::commonExprElim(updated);
        Optim::symProp(updated);
        Optim::deadCodeDel(updated);
        Optim::peephole(updated);
        Optim::clean();
#if !judge
        midcode_output << "<!--anchor-->" << endl;
        MidCode::output();
#endif /* judge */
    }
#if !judge
    cout << "finished" << endl;
#endif /* judge */

#if !judge
	cout << "mips generating ... ";
#endif /* judge */
    mips::init();
    mips::output();
    mips::deinit();
#if !judge
	cout << "finished" << endl;
#endif /* judge */

exit:
    close();
	return 0;
}
