/**********************************************
    > File Name: Database.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include "symtable.h"
#include "compiler.h"
#include "error.h"
using namespace std;

symtable::Database table;

void symtable::Database::pushFunc(const string& funcName, FuncTable* ft) {
	FuncTable*& ftp = _func[funcName];
	if (ftp != nullptr) { 
		err << error::REDEF << endl; 
		return; 
	}
	ftp = ft;
	_cur = ftp;
	log << "symtable: insert " << *ft << ' ' << funcName << endl;
}

symtable::Database::~Database(void) {
	for (map<string, FuncTable*>::iterator it = _func.begin(); 
			it != _func.end(); it++) {
		delete it->second;
	}
	log << "symtable: deconstruction succeeded" << endl;
}

symtable::FuncTable* symtable::Database::curFunc(void) const {
	assert(_cur != &_global && _cur != &_main);
	return (FuncTable*) _cur;
}

const symtable::Entry* symtable::Database::findSym(const string& symName) {
	const Entry* result = _cur->find(symName);
	return result == nullptr ? _global.find(symName) : result;
}

void symtable::Database::pushSym(const string& symName, const bool isConst, const bool isInt, const int value) {
	_cur->push(symName, isConst, isInt, value);
}
