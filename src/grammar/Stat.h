/**********************************************
    > File Name: Stat.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:40:51 2019
 **********************************************/

#ifndef STAT_H
#define STAT_H

#include <string>

namespace symtable {
	class Entry;
}

// The return values indicate whether the current
// <stat> has <ret> covering all the paths. All 
// non-terminals with <stat> inside (except for 
// <block>) should return. 
class Stat {
	class Cond {
		static void cond(const bool, const std::string&);
	public:
		static bool _if(void);
		static void _while(void);
		static bool _do(void);
		static void _for(void);
	};
	static void read(void);
	static void write(void);
	static void ret(void);
	static void assign(symtable::Entry* const);
	static bool stat(void);
public:
	// Since <block> can only show up in <func def>, it 
	// is obligated to check whether <ret> has covered 
	// all paths. Thus, it does not have a return value.
	static void block(void);
};

#endif /* STAT_H */
