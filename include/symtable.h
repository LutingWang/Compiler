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

namespace symtable {
	// One record in the database. A record represents 
	// a variable (argument) or constant in the scope.
	// It is necessary for the record to save whether
	// the symbol is 
	//		const or not	- isConst
	//		int or char		- isInt
	// and also an extra field `value`.
	//
	// For a constant, value represents the magnitude of 
	// a int or the ASCII of a char. For a variable,
	// value is used to distinguish array, where value 
	// is the length of this array, from single values
	// where value takes -1.
	struct Entry {
		bool isConst, isInt;
		int value; 

		Entry(bool isConst, bool isInt, int value) : 
			isConst(isConst), isInt(isInt), value(value) {}
	};
	
	std::ostream& operator << (std::ostream&, const Entry&);

	// One table in the database. A table represents a
	// scope in the program. In this scenario where scopes
	// cannot be nested, scope has the same meaning as 
	// functions, except for the global scope which is 
	// seen as a sepcial outer function.
	//
	// Inside each table, the primary key is set as the
	// name of a symbol. Thus, the integrity constraints
	// helps to ensure that no symbol is redefined. 
	class Table {
		friend class Database;
	protected:
		std::map<std::string, Entry*> _syms;

		~Table(void);

		const Entry* find(const std::string& symName) { return _syms[symName]; }

		const Entry* push(const std::string&, const bool isConst, const bool isInt, const int = -1);
	};
	
	class FuncTable : public Table {
		friend class Database;
		friend std::ostream& operator << (std::ostream&, const FuncTable&);
	protected:
		std::vector<const Entry*> _argList; // arg is int or not

		FuncTable(void) : isVoid(true), isInt(false) {}

		FuncTable(const bool isInt) : isVoid(false), isInt(isInt) {}

		void pushArg(const std::string& symName, const bool isInt) {
			_argList.push_back(push(symName, false, isInt));
		}
	public:
		const bool isVoid, isInt;

		const std::vector<const Entry*>& argList(void) const { return _argList; }
	};
	
	class Database {
		Table _global;
		std::map<std::string, FuncTable*> _func;
		Table _main;
		Table* _cur = &_global;

		void pushFunc(const std::string&, FuncTable*);
	public:
		~Database(void);

		bool isMain(void) const { return _cur == &_main; }

		// can only be called when parsing a function except for main
		FuncTable* curFunc(void) const;

		// if the symbol is not defined, return nullptr
		// the caller is obligated to check
		const FuncTable* findFunc(const std::string& funcName) { return _func[funcName]; }
		const Entry* findSym(const std::string&);

		// automatically check for re-definition
		void pushFunc(const std::string& funcName) { 
			pushFunc(funcName, new FuncTable()); 
		}

		void pushFunc(const std::string& funcName, const bool isInt) {
			pushFunc(funcName, new FuncTable(isInt));
		}

		void pushFunc(void) { _cur = &_main; }

		void pushArg(const std::string& symName, const bool isInt) {
			curFunc()->pushArg(symName, isInt);
		}

		void pushSym(const std::string&, const bool isConst, const bool isInt, const int = -1);
	};
}

extern symtable::Database table;

#endif /* SYMTABLE_H */
