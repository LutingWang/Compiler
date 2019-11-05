/**********************************************
    > File Name: Database.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include <string>
#include "symtable.h"
#include "compiler.h"
#include "error.h"
using namespace std;

symtable::Database table;

void symtable::Database::pushFunc(const string& funcName, FuncTable* ft) {
	FuncTable*& ftp = _func[funcName];
	if (ftp != nullptr) { 
		err << error::Code::REDEF << endl; 
		return; 
	}
	ftp = ft;
	_cur = ftp;
	logger << "symtable: insert " << *ft << endl;
}

symtable::Database::~Database(void) {
	for (map<string, FuncTable*>::iterator it = _func.begin(); 
			it != _func.end(); it++) {
		delete it->second;
	}
	logger << "symtable: deconstruction succeeded" << endl;
}

symtable::FuncTable* symtable::Database::curFunc(void) const {
	assert(_cur != &_global && _cur != &_main);
	return (FuncTable*) _cur;
}

symtable::Entry* symtable::Database::findSym(const string& symName) {
	Entry* result = _cur->find(symName);
	return result == nullptr ? _global.find(symName) : result;
}

void symtable::Database::pushSym(const string& symName, const bool isConst, const bool isInt, const int value) {
	_cur->push(symName, isConst, isInt, value);
}

