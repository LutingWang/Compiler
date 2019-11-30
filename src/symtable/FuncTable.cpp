/**********************************************
    > File Name: FuncTable.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 14:10:50 2019
 **********************************************/

#include <cassert>
#include "midcode/MidCode.h"

#include "symtable/table.h"
using namespace symtable;

bool FuncTable::isGlobal(void) const {
    return false;
}

bool FuncTable::isVoid(void) const {
	return _void;
}

bool FuncTable::isInt(void) const {
	return _int;
}

const std::vector<const symtable::Entry*>& FuncTable::argList(void) const { 
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
    for (auto midcode : _midcodes) {
        if (midcode->is(MidCode::Instr::CALL) && midcode->labelName() == name()) {
            return false;
        }
    }
    return true;
}

FuncTable::FuncTable(const std::string& name) : 
	Table(name), _void(true), _int(false) {}

FuncTable::FuncTable(const std::string& name, const bool isInt) : 
	Table(name), _void(false), _int(isInt) {}

FuncTable::~FuncTable(void) {
	assert(_const);
	for (auto midcode : midcodes()) { 
		delete midcode; 
	}
}

void FuncTable::operator << (const FuncTable& source) {
    assert(_const);
    for (auto& /* <renamed_symName: string, const Entry*> */ pair : source._syms) {
        if (_contains(pair.first)) {
            assert(_find(pair.first) == pair.second);
        }
    }
    _syms.insert(source._syms.begin(), source._syms.end());
}

void FuncTable::setHasRet(void) {
	assert(!_const);
	_hasRet = true;
}

const Entry* FuncTable::pushArg(const std::string& symName, const bool isInt) {
	const Entry* const entry = pushVar(symName, isInt);
	_argList.push_back(entry);
	return entry;
}

