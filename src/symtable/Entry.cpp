/**********************************************
    > File Name: Entry.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 14:10:34 2019
 **********************************************/

#include <cassert>

#include "symtable/Entry.h"
using namespace symtable;

bool Entry::isGlobal(void) const {
    return _global;
}

bool Entry::isConst(void) const {
	return _const;
}

bool Entry::isInt(void) const {
	return _int;
}

int symtable::Entry::value(void) const {
	assert(isConst() || isArray());
	return _value;
}

const std::string& symtable::Entry::name(void) const {
    return _name;
}

namespace {
	const int NOT_ARRAY = -1;
	const int INVALID = -2;
}

symtable::Entry::Entry(const std::string& symName,
        const bool isGlobal,
        const bool isConst,
		const bool isInt, 
		const int value) : 
	_global(isGlobal), _const(isConst), _int(isInt), _value(value), _name(symName) {
	assert(isConst || value > 0);
}

symtable::Entry::Entry(const std::string& symName,
        const bool isGlobal,
        const bool isInt) :
	_global(isGlobal), _const(false), _int(isInt), _value(NOT_ARRAY), _name(symName) {}

symtable::Entry::Entry(const std::string& symName) :
	_global(false), _const(false), _int(true), _value(INVALID), _name(symName) {}

bool symtable::Entry::isInvalid(void) const {
	return !isConst() && _value == INVALID;
}

bool symtable::Entry::isArray(void) const { 
	return !isConst() && _value !=  NOT_ARRAY && 
		_value != INVALID; 
}
