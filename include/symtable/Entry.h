/**********************************************
    > File Name: Entry.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 15:14:25 2019
 **********************************************/

#ifndef ENTRY_H
#define ENTRY_H

#include <string>

namespace symtable {
	class Entry {
		friend class Table;

		const bool _const, _int;
		const int _value; 
		const std::string _name;
	public:
		bool isConst(void) const;
		bool isInt(void) const;
		int value(void) const;
		const std::string& name(void) const;

	private:
		// const or array
		Entry(const std::string&, 
				const bool isConst, 
				const bool isInt, 
				const int);
		
		// single var
		Entry(const std::string&, 
				const bool isInt); 

		// invalid var
		Entry(const std::string&); 

	public:
		bool isInvalid(void) const;
		bool isArray(void) const;
	};
}

#endif /* ENTRY_H */
