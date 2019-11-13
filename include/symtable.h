/**********************************************
    > File Name: symtable.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 30 19:31:25 2019
 **********************************************/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

class MidCode;
class Mips;
class Optim;

namespace symtable {
	// One record in the database. A record represents 
	// a variable (argument) or constant in the scope.
	class Entry {
		friend class Printer;

		const std::string _name;
	public:
		const bool isConst; 
		const bool isInt; // int or char
		
		// For a constant, value represents the magnitude of 
		// an int or the ASCII of a char. For a variable,
		// value is used to distinguish array, where value 
		// is the length of this array, from single values
		// where value takes -1.
		const int value; 

		Entry(const std::string& name, const bool isConst, const bool isInt, const int value) : 
			_name(name), isConst(isConst), isInt(isInt), value(value) {}

		bool isArray(void) const { return !isConst && value > -1; }

		std::string name(void) const { return _name; }
	};

	// One table in the database. A table represents a
	// scope in the program. In this scenario where scopes
	// cannot be nested, scope has the same meaning as 
	// functions, except for the global scope which is 
	// seen as a sepcial outer function.
	class Table {
		friend class Printer;
		friend class Database;
		friend class ::MidCode;
		friend class ::Mips;
	protected:
		const std::string _name;
	public:
		std::string name(void) const { return _name; }

	protected:
		// The primary key is set as the name of a symbol. 
		// Thus, the integrity constraints helps to ensure 
		// that no symbol is redefined. 
		std::map<std::string, Entry*> _syms;
	public:
		void syms(std::set<Entry*>&) const;

		// TODO: alter name to avoid conflict in mips
		Table(const std::string& name) : _name(name) {}

		virtual ~Table(void);

		Entry* find(const std::string& symName) { return _syms[symName]; }

		Entry* push(const std::string&, const bool isConst, const bool isInt, const int = -1);
	};
	
	class FuncTable : public Table {
		friend class Printer;
		friend class Database;
		friend class ::MidCode;
		friend class ::Mips;
		friend class ::Optim;
	public:
		const bool isVoid, isInt;

	protected:
		// Each element points to an existing `Entry` in
		// `_syms`, so the deinitializer should not deallocate
		// these pointers.
		std::vector<Entry*> _argList;
	public:
		const std::vector<Entry*>& argList(void) const { return _argList; }

	protected:
		// The elements in different `FuncTable` should be
		// distinct, so that optimizer can allocate and
		// deallocate freely. Therefore, deinitializer is
		// obligated to deallocate these pointers. 
		//
		// Any function that wants to operate on `_midcode`
		// should guarantee that the inserted elements do
		// not collide with other `FuncTable`s, since they
		// are friends.
		std::vector<MidCode*> _midcode;
	public:
		const std::vector<MidCode*>& midcodes(void) const;

	protected:
		// If a top level return has been detected, no 
		// more mid code should be generated.
		bool _hasRet = false;
	public:
		// This function can only be called by `Stat::block`
		void setHasRet(void) { _hasRet = true; }

		bool hasRet(void) const { return _hasRet; }

	protected:
		// non-recursive functions are inline by default
		bool _inline = true;
	public:
		bool isInline(void) const { return _inline; }

	protected:
		// deallocate `_midcode`
		virtual ~FuncTable(void);

		FuncTable(const std::string& name) : 
			Table(name), isVoid(true), isInt(false) {}

		FuncTable(const std::string& name, const bool isInt) : 
			Table(name), isVoid(false), isInt(isInt) {}

		void pushArg(const std::string& symName, const bool isInt) {
			_argList.push_back(push(symName, false, isInt));
		}
	};
	
	// public interface
	class Database {
		friend class ::MidCode;
		friend class ::Mips;
		friend class ::Optim;

		Table _global;
		std::map<std::string, FuncTable*> _func;
		FuncTable _main;
		FuncTable* _cur = nullptr;

	public:
		Database(void) : _global("global"), _main("main") {}

		~Database(void);

		bool isMain(void) const { return _cur == &_main; }

		FuncTable* curFunc(void) const;

		// If the symbol is not defined, return nullptr. 
		// The caller is obligated to check. Main func 
		// can also be found.
		const FuncTable* findFunc(const std::string&);

		Entry* findSym(const std::string&);

		const Table& global(void) const;

		void funcs(std::vector<const FuncTable*>&) const;

	private:
		void pushFunc(const std::string&, FuncTable* const);
	public:
		// automatically check for re-definition
		void pushFunc(const std::string& funcName) { 
			pushFunc(funcName, new FuncTable(funcName)); 
		}

		void pushFunc(const std::string& funcName, const bool isInt) {
			pushFunc(funcName, new FuncTable(funcName, isInt));
		}

		void pushFunc(void); // push main

		void pushArg(const std::string&, const bool);

		void pushSym(const std::string&, const bool isConst, const bool isInt, const int = -1);
	};
}

extern symtable::Database table;

#endif /* SYMTABLE_H */
