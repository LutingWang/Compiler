/**********************************************
    > File Name: Database.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include "symtable.h"
using namespace std;

symtable::Database::~Database(void) {
	for (map<string, FuncTable*>::iterator it = _func.begin(); it != _func.end(); it++) {
		delete it->second;
	}
}

symtable::Entry* symtable::Database::findSym(const string& symName) {
	Entry* result = _cur->find(symName);
	return result == nullptr ? _global.find(symName) : result;
}

void symtable::Database::pushFunc(const string& funcName, const RetType type) {
	_cur = new FuncTable(type);
	_func[funcName] = (FuncTable*) _cur;
}

void symtable::Database::pushSym(const string& symName, const SymCat cat, const bool isInt, const int value) {
	assert(cat != VAR || value >= -1);
	assert(cat != ARG || value == -1);
	_cur->push(symName, cat, isInt, value);
}
