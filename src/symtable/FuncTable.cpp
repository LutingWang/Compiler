/**********************************************
    > File Name: FuncTable.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 14:10:50 2019
 **********************************************/

#include "midcode/MidCode.h"

#include "symtable/table.h"
using namespace symtable;

bool FuncTable::isVoid(void) const {
	return _void;
}

bool FuncTable::isInt(void) const {
	return _int;
}

const std::vector<const symtable::Entry*>& FuncTable::argList(void) const { 
	assert(_const);
	return _argList; 
}

const std::vector<const MidCode*>& FuncTable::midcodes(void) const {
	assert(_const);
	return _midcodes;
}

bool FuncTable::hasRet(void) const {
	assert(!_const);
	return _hasRet;
}

bool FuncTable::isInline(void) const { 
	assert(_const);
	return _inline; 
}

FuncTable::FuncTable(const std::string& name) : 
	Table(name), _void(true), _int(false) {}

FuncTable::FuncTable(const std::string& name, const bool isInt) : 
	Table(name), _void(false), _int(isInt) {}

symtable::FuncTable::~FuncTable(void) {
	assert(_const);
	for (auto& midcode : midcodes()) { 
		delete midcode; 
	}
}

void symtable::FuncTable::setHasRet(void) {
	assert(!_const);
	_hasRet = true;
}

void symtable::FuncTable::setRecursive(void) {
	assert(!_const);
	_inline = false;
}

const Entry* FuncTable::pushArg(const std::string& symName, const bool isInt) {
	const Entry* const entry = pushVar(symName, isInt);
	_argList.push_back(entry);
	return entry;
}

