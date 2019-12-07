/**********************************************
    > File Name: symtable.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 11:09:08 2019
 **********************************************/

#include <cassert>
#include <fstream>
#include <iomanip>
#include "compilerConfig.h"
#include "error.h"
#include "midcode/MidCode.h"

#include "symtable.h"

namespace symtable {
    class Printer {
    public:
        static void print(const Entry&);
        static void print(const Table&);
        static void print(const FuncTable&);
    };
}

using namespace symtable;

/* Entry */

bool Entry::isGlobal(void) const {
    return _global;
}

bool Entry::isConst(void) const {
    return _const;
}

bool Entry::isInt(void) const {
    return _int;
}

int Entry::value(void) const {
    assert(isConst() || isArray());
    return _value;
}

const std::string& Entry::name(void) const {
    return _name;
}

namespace {
    const int NOT_ARRAY = -1;
    const int INVALID = -2;
}

Entry::Entry(const std::string& symName,
        const bool isGlobal,
        const bool isConst,
        const bool isInt,
        const int value) :
    _global(isGlobal), _const(isConst), _int(isInt), _value(value), _name(symName) {
    assert(isConst || value > 0);
}

Entry::Entry(const std::string& symName,
        const bool isGlobal,
        const bool isInt) :
    _global(isGlobal), _const(false), _int(isInt), _value(NOT_ARRAY), _name(symName) {}

Entry::Entry(const std::string& symName) :
    _global(false), _const(false), _int(true), _value(INVALID), _name(symName) {}

bool Entry::isInvalid(void) const {
    return !isConst() && _value == INVALID;
}

bool Entry::isArray(void) const {
    return !isConst() && _value !=  NOT_ARRAY &&
        _value != INVALID;
}

/* Table */

const std::string& Table::name(void) const {
    return _name;
}

void Table::syms(std::set<const Entry*>& symList) const {
    assert(_const);
    assert(symList.empty());
    for (auto& /* <renamed_symName: string, const Entry*> */ pair : _syms) {
        auto result = symList.insert(pair.second);
        assert(result.second);
    }
    assert(symList.count(nullptr) == 0);
}

bool Table::isGlobal(void) const {
    return true;
}

Table::Table(const std::string& name) :
    _name(name) {}

Table::~Table(void) {
    assert(_const);
}

std::string Table::_rename(const std::string& symName) const {
    return name() + '_' + symName;
}

bool Table::_contains(const std::string& symName) const {
    return _syms.count(symName);
}

const Entry* Table::_find(const std::string& symName) const {
    return _syms.at(symName);
}

bool Table::contains(const std::string& symName) const {
    return _contains(_rename(symName));
}

const Entry* Table::find(const std::string& symName) const {
    return _find(_rename(symName));
}

const Entry* Table::_push(const Entry* const entry) {
    if (_contains(entry->name())) {
        error::raise(error::Code::REDEF);
        delete _find(entry->name());
    }
    _syms[entry->name()] = entry;
#if !judge
    Printer::print(*entry);
#endif /* judge */
    return entry;
}

const Entry* Table::_pushInvalid(const std::string& symName) {
    error::raise(error::Code::NODEF);
    return _push(new Entry(_rename(symName)));
}

const Entry* Table::pushConst(const std::string& symName,
        const bool isInt, const int value) {
    return _push(new Entry(_rename(symName), isGlobal(), true, isInt, value));
}

const Entry* Table::pushArray(const std::string& symName,
        const bool isInt, const int value) {
    return _push(new Entry(_rename(symName), isGlobal(), false, isInt, value));
}

const Entry* Table::pushVar(const std::string& symName, const bool isInt) {
    return _push(new Entry(_rename(symName), isGlobal(), isInt));
}

void Table::_makeConst(void) {
    assert(!_const);
    _const = true;
}

/* FuncTable */

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

/* SymTable */

Table& SymTable::global(void) const {
    return *_global;
}

void SymTable::funcs(std::set<FuncTable*>& functables, bool) const {
    assert(functables.empty());
    functables.insert(_main);
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        auto result = functables.insert(pair.second);
        assert(result.second);
    }
}

void SymTable::funcs(std::set<const FuncTable*>& functables) const {
    assert(functables.empty());
    functables.insert(_main);
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        auto result = functables.insert(pair.second);
        assert(result.second);
    }
}

Table& SymTable::curTable(void) const {
    return isGlobal() ? global() : curFunc();
}

FuncTable& SymTable::curFunc(void) const {
    return *_cur;
}

SymTable SymTable::table;

SymTable::SymTable(void) :
    _global(new Table("global")),
    _main(new FuncTable("main")),
    _cur(nullptr) {}

SymTable::~SymTable(void) {
	// Collect all syms while deallocating tables.
    std::set<const Entry*> syms;
    _global->syms(syms);
    delete _global;
    for (auto& /* <string, const FuncTable*> */ pair : _funcs) {
        std::set<const Entry*> tmp;
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

const Entry* SymTable::findSym(const std::string& symName) const {
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

const FuncTable* SymTable::findFunc(const std::string& funcName) const {
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

void SymTable::_pushFunc(const std::string& funcName, FuncTable* const functable) {
    assert(functable != nullptr);
    if (contains(funcName)) {
        error::raise(error::Code::REDEF);
        delete findFunc(funcName);
    }
    _funcs[funcName] = functable;
    
    curTable()._makeConst();
    _cur = functable;
#if !judge
	Printer::print(*functable);
#endif /* judge */
}

void SymTable::pushFunc(const std::string& funcName) {
    _pushFunc(funcName, new FuncTable(funcName));
}

void SymTable::pushFunc(const std::string& funcName, const bool isInt) {
    _pushFunc(funcName, new FuncTable(funcName, isInt));
}

void SymTable::pushFunc(void) {
	assert(!isMain());
    curTable()._makeConst();
	_cur = _main;
#if !judge
	Printer::print(*_main);
#endif /* judge */
}

bool SymTable::_isConst(void) const {
    return isMain() && _main->_const;
}

void SymTable::makeConst(void) {
    assert(!_isConst() && isMain());
    _main->_makeConst();
}

/* Printer */

#if !judge
extern std::ofstream symtable_output;

void symtable::Printer::print(const Entry& entry) {
    symtable_output << std::setw(30) << typeid(entry).name() << " : "
        << std::setw(30) << entry.name() << '<';
    if (entry.isInvalid()) {
        symtable_output << "invalid";
    } else if (entry.isConst()) {
        symtable_output << "const ";
        if (entry.isInt()) { symtable_output << "int("  << entry.value() << ')'; }
        else { symtable_output << "char('" << (char) entry.value() << "')"; }
    } else {
        symtable_output << (entry.isInt() ? "int" : "char");
        if (entry.isArray()) { symtable_output << '[' << entry.value() << ']'; }
    }
    symtable_output << '>' << std::endl;
}

void symtable::Printer::print(const Table& table) {
    symtable_output << std::setw(30) << typeid(table).name() << " : "
        << std::setw(30) << table.name() << std::endl;
}

void symtable::Printer::print(const FuncTable& functable) {
    symtable_output << std::setw(30) << typeid(functable).name() << " : "
        << std::setw(30) << functable.name() << '<';
    if (functable.isVoid()) { symtable_output << "void"; }
    else if (functable.isInt()) { symtable_output << "int"; }
    else { symtable_output << "char"; }
    symtable_output << '>' << std::endl;
}
#endif /* judge */
