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
    // `Entry` depicts 7 classes of symbols in the source code. The
    // classes and their corresponding value tuples are listed below
    //     + type           _const      _int        _value
    //     - const int      (true,      true,       int_val)
    //     - const char     (true,      false,      char_val)
    //     - int            (false,     true,       NOT_ARRAY)
    //     - int[]          (false,     true,       array_len)
    //     - char           (false,     false,      NOT_ARRAY)
    //     - char[]         (false,     false,      array_len)
    //     - invalid        (false,     true,       INVALID)
    // Among them, `invalid` is reserved for error handling. `NOT_ARRAY`
    // and `INVALID` are predefined constants to flag specific cases.
    // Their exact values are not concerned as long as they are negative.
    //
    // It would be helpful to introduce some terminologies here.
    //     - const          const int / const char
    //     - array          int[] / char[]
    //     - non-array var  int / char
    //     - var            non-array var / array
	class Entry {
		friend class Table;

		const bool _global, _const, _int;
		const int _value; 
		const std::string _name; // renamed names
	public:
		bool isGlobal(void) const;
		bool isConst(void) const;
		bool isInt(void) const;
		int value(void) const;
		const std::string& name(void) const;

	private:
		// const or array
		Entry(const std::string&, 
				const bool isGlobal,
				const bool isConst, 
				const bool isInt, 
				const int);
		
		// non-array var
		Entry(const std::string&, 
				const bool isGlobal,
				const bool isInt); 

		// invalid
		Entry(const std::string&); 

	public:
		bool isInvalid(void) const;
		bool isArray(void) const;
	};
}

#endif /* ENTRY_H */
