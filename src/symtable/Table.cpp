/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include <typeinfo>
#include "symtable.h"
#include "error.h"
using namespace std;

ostream& symtable::operator << (ostream& output, const Entry& e) {
	output << typeid(e).name() << '<';
	if (e.isConst) {
		output << "const ";
		if (e.isInt) { output << "int("  << e.value << ')'; } 
		else { output << "char('" << (char) e.value << "')"; }
	} else {
		output << (e.isInt ? "int" : "char");
		if (e.value != -1) { output << '[' << e.value << ']'; }
	}
	output << '>';
	return output;
}

symtable::Table::~Table(void) {
	for (map<string, Entry*>::iterator it = _syms.begin(); it != _syms.end(); it++) {
		delete it->second;
	}
}

const symtable::Entry* symtable::Table::push(const string& symName, const bool isConst, const bool isInt, const int value) {
	Entry*& entry = _syms[symName];
	if (entry != nullptr) { err << error::REDEF << endl; } 
	else {
		entry = new Entry(isConst, isInt, value);
		log << "symtable: insert " << *entry << ' ' << symName << endl;
	}
	return entry;
}

ostream& symtable::operator << (ostream& output, const FuncTable& ft) {
	output << typeid(ft).name() << '<';
	if (ft.isVoid) { output << "void"; }
	else if (ft.isInt) { output << "int"; }
	else { output << "char"; }
	output << '>';
	return output;
}

