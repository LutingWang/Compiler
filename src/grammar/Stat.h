/**********************************************
    > File Name: Stat.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:40:51 2019
 **********************************************/

#ifndef STAT_H
#define STAT_H

#include "symtable.h"

// The return values indicate whether the current
// <stat> has <ret> covering all the paths. All 
// non-terminals with <stat> inside (except for 
// <block>) should return. 
//
// Since <block> can only show up in <func def>, it 
// is obligated to check whether <ret> has covered 
// all paths.
class Stat {
	class Cond {
		static void cond(void);
	public:
		static bool _if(void);
		static bool _while(void);
		static bool _do(void);
		static bool _for(void);
	};
	static void read(void);
	static void write(void);
	static void ret(void);
	static void assign(const symtable::Entry*);
	static bool stat(void);
public:
	static void block(void);
};

#endif /* STAT_H */
