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

symtable::Database::~Database(void) {
	for (auto& e : _func) { delete e.second; }
}

symtable::FuncTable* symtable::Database::curFunc(void) const {
	assert(_cur != nullptr);
	return _cur;
}

const symtable::FuncTable* symtable::Database::findFunc(const std::string& funcName) { 
	if (funcName == "main") { 
		assert(_cur = &_main);
		return &_main; 
	}
	return _func[funcName]; 
}

symtable::Entry* symtable::Database::findSym(const string& symName) {
	if (_cur != nullptr) {
		Entry* result = _cur->find(symName);
		if (result != nullptr) { return result; }
	}
	return _global.find(symName);
}

const symtable::Table& symtable::Database::global(void) const {
	return _global;
}

void symtable::Database::funcs(std::vector<const FuncTable*>& functables) const {
	assert(functables.empty());
	functables.push_back(&_main);
	for (auto& pair : _func) {
		functables.push_back(pair.second);
	}
}

void symtable::Database::pushFunc(const string& funcName, FuncTable* const ft) {
	FuncTable*& ftp = _func[funcName];
	if (ftp != nullptr) { error::raise(error::Code::REDEF); }
	ftp = ft;
	_cur = ft;
	Printer::print(*ft);
}

void symtable::Database::pushFunc(void) { 
	assert(_cur != &_main);
	_cur = &_main; 
	Printer::print(_main);
}

void symtable::Database::pushArg(const std::string& symName, const bool isInt) {
	assert(_cur != nullptr && !isMain());
	_cur->pushArg(symName, isInt);
}

void symtable::Database::pushSym(const string& symName, const bool isConst, const bool isInt, const int value) {
	if (_cur == nullptr) {
		_global.push(symName, isConst, isInt, value);
	} else {
		_cur->push(symName, isConst, isInt, value);
	}
}

