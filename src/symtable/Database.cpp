/**********************************************
    > File Name: Database.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include <fstream>
#include <string>
#include "compiler.h"
#include "error.h"

#include "Printer.h"

#include "symtable.h"
using namespace std;

symtable::Database table;

void symtable::Database::pushFunc(void) { 
	_cur = &_main; 
	Printer::print(_main);
}

void symtable::Database::pushFunc(const string& funcName, FuncTable* const ft) {
	FuncTable*& ftp = _func[funcName];
	if (ftp != nullptr) { 
		error::raise(error::Code::REDEF); 
		return; 
	}
	ftp = ft;
	_cur = ftp;
	Printer::print(*ft);
}

symtable::Database::~Database(void) {
	for (auto& e : _func) { delete e.second; }
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

