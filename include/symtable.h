/**********************************************
    > File Name: symtable.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 18:48:22 2019
 **********************************************/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <map>
#include <set>
#include <string>
#include <vector>

class MidCode;
class FlowChart;

class Optim;

class SymTable;

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

    class Table {
        friend class ::SymTable;
        friend class ::Optim;
    
        const std::string _name;
    protected:
        std::map<std::string, const Entry*> _syms;
    public:
        const std::string& name(void) const;
        void syms(std::set<const Entry*>&) const; // can only be called after sealed
        virtual bool isGlobal(void) const;

    protected:
        Table(const std::string&);
        
        // Do nothing. The `SymTable` will free all syms together.
        virtual ~Table(void);

    private:
        // Add the name of this table as a prefix to the name
        // of a sym. The modified name would be a unique id of
        // the sym not only in this `Table`, but also other
        // `Table`s in this compiler. This detour would be exceedingly
        // helpful to the optimizer.
        std::string _rename(const std::string&) const;
    protected:
        // Performs query using modified names.
        bool _contains(const std::string&) const;
        const Entry* _find(const std::string&) const;
    public:
        // Modify sym name and delegate up.
        bool contains(const std::string&) const;
        const Entry* find(const std::string&) const;

    private:
        // Checks for redef while inserting.
        const Entry* _push(const Entry* const);
        const Entry* _pushInvalid(const std::string&);
    public:
        const Entry* pushConst(const std::string&, const bool, const int);
        const Entry* pushArray(const std::string&, const bool, const int);
        const Entry* pushVar(const std::string&, const bool);

    protected:
        bool _const = false;
        
        // Seal the table after parsing the corresponding code
        // segmentation. Only friend classes could modify this
        // table by then. A `Table` cannot be sealed twice.
        void _makeConst(void);
    };
    
    class FuncTable : public Table {
        friend class ::SymTable;
        friend class ::MidCode;
        friend class ::FlowChart;
        friend class ::Optim;

        bool _void, _int;
        std::vector<const Entry*> _argList;
        std::vector<const MidCode*> _midcodes;
        bool _hasRet = false;
    public:
        virtual bool isGlobal(void) const;
        bool isVoid(void) const;
        bool isInt(void) const;
        const std::vector<const Entry*>& argList(void) const;
        const std::vector<const MidCode*>& midcodes(void) const;
        bool hasRet(void) const;
        bool isInline(void) const;

    protected:
        FuncTable(const std::string&); // void
        FuncTable(const std::string&, const bool isInt);
        
        // Deallocate midcodes.
        virtual ~FuncTable(void);

    private:
        // shallow copy of `_syms`
        void operator << (const FuncTable&);
    public:
        void setHasRet(void); // can only be called by `Stat::block`
        const Entry* pushArg(const std::string&, const bool isInt);
    };
}

class SymTable {
    friend class MidCode;
	friend class Optim;

	symtable::Table* const _global;
	std::map<std::string, symtable::FuncTable*> _funcs;
	symtable::FuncTable* const _main;
	symtable::FuncTable* _cur;
public:
	symtable::Table& global(void) const;
private:
    // Interface overloading to retrieve mutable func set.
	void funcs(std::set<symtable::FuncTable*>&, bool) const;
public:
	void funcs(std::set<const symtable::FuncTable*>&) const;
	symtable::Table& curTable(void) const; // including `_global`
	symtable::FuncTable& curFunc(void) const; // excluding `_global`

    // Singleton design pattern.
private:
	static SymTable table;
	SymTable(void);
	~SymTable(void);
public:
	static SymTable& getTable(void);

	bool isGlobal(void) const;
	bool isMain(void) const;
    
    // Searches through `this` then `_global`. If none of them
    // contains the target sym, register an invalid entry for it.
	const symtable::Entry* findSym(const std::string&) const;

	bool contains(const std::string&) const;
	const symtable::FuncTable* findFunc(const std::string&) const;

private:	
	void _pushFunc(const std::string&, symtable::FuncTable* const);
public:
	void pushFunc(const std::string&); // void
	void pushFunc(const std::string&, const bool isInt);
	void pushFunc(void); // main
	
private:
	bool _isConst(void) const;
public:
	void makeConst(void); // seal the symtable
};

#endif /* SYMTABLE_H */
