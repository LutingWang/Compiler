/**********************************************
    > File Name: symtable.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Mon Sep 30 19:31:25 2019
 **********************************************/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <map>
#include <string>

namespace symtable {
	enum RetType {
		VOID, INT, CHAR
	};

	enum SymCat {
		ARG, CONST, VAR
	};
	
	struct Entry {
		SymCat cat;
		bool isInt;
	
		// for const, value of int or ASCII of char
		// otherwise, length of array or -1 for single value
		int value; 

		Entry(SymCat cat, bool isInt, int value) : 
			cat(cat), isInt(isInt), value(value) {}
	};
	
	class Table {
	protected:
		std::map<std::string, Entry*> _syms;
		virtual ~Table(void);
		Entry* find(const std::string& symName) { return _syms[symName]; }
		void push(const std::string&, const SymCat, const bool isInt, const int);
		friend class Database;
	};
	
	class FuncTable : public Table {
		FuncTable(const RetType& type) : type(type) {}
	public:
		RetType type;
		friend class Database;
	};
	
	class Database {
		Table _global;
		std::map<std::string, FuncTable*> _func;
		Table _main;
		Table* _cur = &_global;
	public:
		~Database(void);
		FuncTable* findFunc(const std::string& funcName) { return _func[funcName]; }
		Entry* findSym(const std::string&);
		void pushFunc(const std::string&, const RetType);
		void pushFunc(void) { _cur = &_main; }
		void pushSym(const std::string&, const SymCat, const bool isInt, const int = -1);
	};
}

extern symtable::Database table;

#endif /* SYMTABLE_H */
