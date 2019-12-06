/**********************************************
    > File Name: SymTable.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include "compilerConfig.h"
#include "error.h"
#include "symtable/Entry.h"
#include "symtable/table.h"

#include "Printer.h"

#include "symtable/SymTable.h"
using namespace std;

symtable::Table& SymTable::global(void) const {
    return *_global;
}

void SymTable::funcs(std::set<symtable::FuncTable*>& functables, bool) const {
    assert(functables.empty());
    functables.insert(_main);
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        auto result = functables.insert(pair.second);
        assert(result.second);
    }
}

void SymTable::funcs(std::set<const symtable::FuncTable*>& functables) const {
    assert(functables.empty());
    functables.insert(_main);
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        auto result = functables.insert(pair.second);
        assert(result.second);
    }
}

symtable::Table& SymTable::curTable(void) const {
    return isGlobal() ? global() : curFunc();
}

symtable::FuncTable& SymTable::curFunc(void) const {
    return *_cur;
}

SymTable SymTable::table;

SymTable::SymTable(void) :
    _global(new symtable::Table("global")),
    _main(new symtable::FuncTable("main")),
    _cur(nullptr) {}

SymTable::~SymTable(void) {
	// Collect all syms while deallocating tables.
    std::set<const symtable::Entry*> syms;
    _global->syms(syms);
    delete _global;
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        std::set<const symtable::Entry*> tmp;
        pair.second->syms(tmp);
        delete pair.second;
        syms.insert(tmp.begin(), tmp.end());
    }

	// Free all syms.
    for (auto entry : syms) {
        delete entry;
    }

	// FIXME: deallocate syms in main
    delete _main;
}

SymTable& SymTable::getTable(void) {
    return table;
}

bool SymTable::isGlobal(void) const {
    return _cur == nullptr;
}

bool SymTable::isMain(void) const {
    return _cur == _main;
}

const symtable::Entry* SymTable::findSym(const std::string& symName) const {
    if (curFunc().contains(symName)) {
        return curFunc().find(symName);
    } else if (global().contains(symName)) {
        return global().find(symName);
    } else {
        return curFunc()._pushInvalid(symName);
    }
}

bool SymTable::contains(const std::string& funcName) const {
    return _funcs.count(funcName);
}

const symtable::FuncTable* SymTable::findFunc(const std::string& funcName) const {
    if (funcName == "main") {
        assert(isMain());
        return _main;
    } else if (contains(funcName)) {
        return _funcs.at(funcName);
    } else {
        error::raise(error::Code::NODEF);
        return nullptr;
    }
}

void SymTable::_pushFunc(const string& funcName, symtable::FuncTable* const functable) {
    assert(functable != nullptr);
    if (contains(funcName)) {
        error::raise(error::Code::REDEF);
        delete findFunc(funcName);
    }
    _funcs[funcName] = functable;
    
    curTable()._makeConst();
    _cur = functable;
#if !judge
	symtable::Printer::print(*functable);
#endif /* judge */
}

void SymTable::pushFunc(const std::string& funcName) {
    _pushFunc(funcName, new symtable::FuncTable(funcName));
}

void SymTable::pushFunc(const std::string& funcName, const bool isInt) {
    _pushFunc(funcName, new symtable::FuncTable(funcName, isInt));
}

void SymTable::pushFunc(void) {
	assert(!isMain());
    curTable()._makeConst();
	_cur = _main;
#if !judge
	symtable::Printer::print(*_main);
#endif /* judge */
}

bool SymTable::_isConst(void) const {
    return isMain() && _main->_const;
}

void SymTable::makeConst(void) {
    assert(!_isConst() && isMain());
    _main->_makeConst();
}
