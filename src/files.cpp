/**********************************************
    > File Name: files.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Nov 30 12:50:24 2019
 **********************************************/

#include <fstream>

#include "./files.h"

// Latent streams for corresponding classes to use.
// Do not expose in the headers!
#define OUTPUT_STREAM(id) id##_output

#if judge
    std::ofstream OUTPUT_STREAM(mips);

	#define OUTPUT_PATH(id) #id ".txt"
	#define OPEN(id) \
		OUTPUT_STREAM(id).open(OUTPUT_PATH(id)); \
		OUTPUT_STREAM(id) << std::left;
	void files::open(void) {
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
	void files::open(const std::string& testfile_path) {
		OPEN(error);
		OPEN(symtable);
		OPEN(lexer);
		OPEN(midcode);
		OPEN(mips);	
	}
#endif /* judge */

void files::close(void) {
#if !judge
    OUTPUT_STREAM(error).close();
    OUTPUT_STREAM(symtable).close();
    OUTPUT_STREAM(lexer).close();
    OUTPUT_STREAM(midcode).close();
#endif /* judge */
    OUTPUT_STREAM(mips).close();
}
