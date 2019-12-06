/**********************************************
    > File Name: main.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 23 09:22:09 2019
 **********************************************/

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "compilerConfig.h"
#include "error.h"
#include "frontend.h"
#include "midcode/MidCode.h"
#include "mips.h"
#include "Optim.h"

#include "./files.h"
using namespace std;

// TODO: change midcode naming to avoid conflict
// TODO: scan iterations, add comment of type

#if judge
int main(void) {
    const string testfile_path = "testfile.txt";
    files::open();
#else
int main(int argc, char* argv[]) {
	// print compiler version info
	cout << "compiler version " << COMPILER_VERSION_MAJOR
        << "." << COMPILER_VERSION_MINOR << endl;

    assert(argc == 2);
    const string testfile_path = PROJECT_BASE_DIR + string(argv[1]);
    files::open(testfile_path);
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
#endif /* judge */
    MidCode::output();
#if !judge
    cout << "finished" << endl;
#endif /* judge */

#if !judge
    cout << "optimization processing ... ";
#endif /* judge */
    for (bool updated = true; updated; ) {
        updated = false;
        updated = Optim::inlineExpan() || updated;
        // updated = Optim::commonExprElim() || updated;
        updated = Optim::symProp() || updated;
        // updated = Optim::deadCodeDel() || updated;
        updated = Optim::peephole() || updated;
        Optim::clean();
    }
#if !judge
    cout << "finished" << endl;
#endif /* judge */

#if !judge
    cout << "optimized midcode generating ... ";
#endif /* judge */
    MidCode::output();
#if !judge
    cout << "finished" << endl;
#endif /* judge */

#if !judge
	cout << "mips code generating ... ";
#endif /* judge */
    mips::init();
    mips::output();
    mips::deinit();
#if !judge
	cout << "finished" << endl;
#endif /* judge */

exit:
    files::close();
	return 0;
}
