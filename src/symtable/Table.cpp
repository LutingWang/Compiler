/**********************************************
    > File Name: Table.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Tue Oct  1 12:10:59 2019
 **********************************************/

#include <typeinfo>
#include "compiler.h"
#include "symtable.h"
#include "error.h"
using namespace std;

ostream& symtable::operator << (ostream& output, const Entry& e) {
#if judge
	if (e.isConst) {
		output << "const ";
		if (e.isInt) {
			output << "int " << e.name() << " = " << e.value;
		} else {
			output << "char " << e.name() << " = " << (char) e.value;
		}
	} else {
		output << "var " << (e.isInt ? "int " : "char ") << e.name(); 
		if (e.value != -1) {
			output << '[' << e.value << ']';
		}
	}
#else
	output << typeid(e).name() << ':' << e._name << '<';
	if (e.isConst) {
		output << "const ";
		if (e.isInt) { output << "int("  << e.value << ')'; } 
		else { output << "char('" << (char) e.value << "')"; }
	} else {
		output << (e.isInt ? "int" : "char");
		if (e.value != -1) { output << '[' << e.value << ']'; }
	}
	output << '>';
#endif /* judge */
	return output;
}

symtable::Table::~Table(void) {
	for (auto& e : _syms) { delete e.second; }
}

symtable::Entry* symtable::Table::push(const string& symName, const bool isConst, const bool isInt, const int value) {
	Entry*& entry = _syms[symName];
	if (entry != nullptr) { err << error::Code::REDEF << endl; } 
	else {
		entry = new Entry(_name + '_' + symName, isConst, isInt, value);
		logger << "symtable: insert " << *entry << endl;
	}
	return entry;
}

ostream& symtable::operator << (ostream& output, const FuncTable& ft) {
#if judge
	if (ft.isVoid) { output << "void"; }
	else if (ft.isInt) { output << "int"; }
	else { output << "char"; }
	output << ' ' << ft.name() << "()";
#else
	output << typeid(ft).name() << ':' << ft._name << '<';
	if (ft.isVoid) { output << "void"; }
	else if (ft.isInt) { output << "int"; }
	else { output << "char"; }
	output << '>';
#endif /* judge */
	return output;
}

