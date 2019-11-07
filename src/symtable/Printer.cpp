/**********************************************
    > File Name: Printer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 11:10:49 2019
 **********************************************/

#include <fstream>
#include <iomanip>
#include "symtable.h"

#include "Printer.h"

extern std::ofstream symtable_output;

void symtable::Printer::print(Entry& e) {
	symtable_output << std::setw(30) << typeid(e).name() << " : " 
		<< std::setw(30) << e._name << '<';
	if (e.isConst) {
		symtable_output << "const ";
		if (e.isInt) { symtable_output << "int("  << e.value << ')'; } 
		else { symtable_output << "char('" << (char) e.value << "')"; }
	} else {
		symtable_output << (e.isInt ? "int" : "char");
		if (e.value != -1) { symtable_output << '[' << e.value << ']'; }
	}
	symtable_output << '>' << std::endl;
}

void symtable::Printer::print(const Table& t) {
	symtable_output << std::setw(30) << typeid(t).name() << " : " 
		<< std::setw(30) << t.name() << std::endl;
}

void symtable::Printer::print(const FuncTable& ft) {
	symtable_output << std::setw(30) << typeid(ft).name() << " : " 
		<< std::setw(30) << ft._name << '<';
	if (ft.isVoid) { symtable_output << "void"; }
	else if (ft.isInt) { symtable_output << "int"; }
	else { symtable_output << "char"; }
	symtable_output << '>' << std::endl;
}
