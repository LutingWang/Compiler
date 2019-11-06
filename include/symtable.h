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
#include <string>
#include <vector>

class MidCode;

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
	protected:
		const std::string _name;

		Table(const std::string& name) : _name(name) {}

		// The primary key is set as the name of a symbol. 
		// Thus, the integrity constraints helps to ensure 
		// that no symbol is redefined. 
		std::map<std::string, Entry*> _syms;

		std::vector<MidCode*> _midcode;

		// If a top level return has been detected, no 
		// more mid code should be generated.
		bool _hasRet = false;

		~Table(void);

		Entry* find(const std::string& symName) { return _syms[symName]; }

		Entry* push(const std::string&, const bool isConst, const bool isInt, const int = -1);
	public:
		std::string name(void) const { return _name; }

		bool hasRet(void) const { return _hasRet; }
	};
	
	class FuncTable : public Table {
		friend class Printer;
		friend class Database;
	protected:
		std::vector<Entry*> _argList;

		FuncTable(void) : 
			Table("unknown"), isVoid(true), isInt(false) {}

		FuncTable(const std::string& name) : 
			Table(name), isVoid(true), isInt(false) {}

		FuncTable(const std::string& name, const bool isInt) : 
			Table(name), isVoid(false), isInt(isInt) {}

		void pushArg(const std::string& symName, const bool isInt) {
			_argList.push_back(push(symName, false, isInt));
		}
	public:
		const bool isVoid, isInt;

		const std::vector<Entry*>& argList(void) const { return _argList; }
	};
	
	// public interface
	class Database {
		friend class ::MidCode;

		Table _global;
		std::map<std::string, FuncTable*> _func;
		Table _main;
		Table* _cur = &_global;

		void pushFunc(const std::string&, FuncTable* const);
	public:
		Database(void) : _global("global"), _main("main") {}

		~Database(void);

		bool isMain(void) const { return _cur == &_main; }

		// can only be called when parsing a function except for main
		FuncTable* curFunc(void) const;

		// if the symbol is not defined, return nullptr
		// the caller is obligated to check
		const FuncTable* findFunc(const std::string& funcName) { return _func[funcName]; }
		Entry* findSym(const std::string&);

		// automatically check for re-definition
		void pushFunc(const std::string& funcName) { 
			pushFunc(funcName, new FuncTable(funcName)); 
		}

		void pushFunc(const std::string& funcName, const bool isInt) {
			pushFunc(funcName, new FuncTable(funcName, isInt));
		}

		void pushFunc(void); // push main

		void pushArg(const std::string& symName, const bool isInt) {
			curFunc()->pushArg(symName, isInt);
		}

		void pushSym(const std::string&, const bool isConst, const bool isInt, const int = -1);
		
		// This function can only be called by `Stat::block`
		void setHasRet(void) { _cur->_hasRet = true; }
	};
}

extern symtable::Database table;

#endif /* SYMTABLE_H */
