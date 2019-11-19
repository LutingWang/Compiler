/**********************************************
    > File Name: SymTable.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 18:48:22 2019
 **********************************************/

#ifndef SYM_TABLE_H
#define SYM_TABLE_H

#include <map>
#include <set>
#include <string>

class Optim;

namespace symtable {
	class Entry;
	class Table;
	class FuncTable;
}

class SymTable {
	friend class Optim;

	symtable::Table* const _global;
	std::map<std::string, symtable::FuncTable*> _funcs;
	symtable::FuncTable* const _main;
	symtable::FuncTable* _cur;
public:
	symtable::Table& global(void) const;
private:
	void funcs(std::set<symtable::FuncTable*>&, bool) const;
public:
	void funcs(std::set<const symtable::FuncTable*>&) const;
	symtable::Table& curTable(void) const;
	symtable::FuncTable& curFunc(void) const;

private:
	static SymTable table;
	SymTable(void);
	~SymTable(void);
public:
	static SymTable& getTable(void);

	bool isGlobal(void) const;
	bool isMain(void) const;
	const symtable::Entry* findSym(const std::string&) const;

	bool contains(const std::string&) const;
	const symtable::FuncTable* findFunc(const std::string&) const;

private:	
	void _pushFunc(const std::string&, symtable::FuncTable* const);
public:
	void pushFunc(const std::string&);
	void pushFunc(const std::string&, const bool);
	void pushFunc(void); // push main
	
private:
	bool _isConst(void) const;
public:
	void makeConst(void);
};

#endif /* SYM_TABLE_H */
