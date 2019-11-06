/**********************************************
    > File Name: Printer.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov  6 11:08:21 2019
 **********************************************/

#ifndef PRINTER_H
#define PRINTER_H

namespace symtable {
	class Entry;
	class Table;
	class FuncTable;

	class Printer {
	public:
		static void print(Entry&);
		static void print(const Table&);
		static void print(const FuncTable&);
	};
}

#endif /* PRINTER_H */
