/**********************************************
    > File Name: Printer.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 11:10:49 2019
 **********************************************/

#include <fstream>
#include <iomanip>
#include "compilerConfig.h"
#include "symtable/Entry.h"
#include "symtable/table.h"

#include "Printer.h"

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
