/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include "symtable.h"
using namespace std;

symtable::Table::~Table(void) {
	for (map<string, Entry*>::iterator it = _syms.begin(); it != _syms.end(); it++) {
		delete it->second;
	}
}

void symtable::Table::push(const string& symName, const SymCat cat, const bool isInt, const int value) {
	_syms[symName] = new Entry(cat, isInt, value);
}
