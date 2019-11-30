/**********************************************
    > File Name: table.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Nov 14 15:26:22 2019
 **********************************************/

#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <set>
#include <string>
#include <vector>

class MidCode;
class FlowChart;

class Optim;

class SymTable;

namespace symtable {
	class Entry;

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

#endif /* TABLE_H */
