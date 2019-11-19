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
		std::map<std::string, const Entry*> _syms;
	public:
		const std::string& name(void) const;
		void syms(std::set<const Entry*>&) const;

	protected:
		Table(const std::string&);
		virtual ~Table(void);

	private:
		std::string _rename(const std::string&) const;
        bool _contains(const std::string&) const;
        const Entry* _find(const std::string&) const;
	public: 
        bool contains(const std::string&) const;
        const Entry* find(const std::string&) const;

	private:
		const Entry* _push(const Entry* const);
		const Entry* _pushInvalid(const std::string&);
	public:
		const Entry* pushConst(const std::string&, const bool, const int);
		const Entry* pushArray(const std::string&, const bool, const int);
		const Entry* pushVar(const std::string&, const bool);

	private:
		// migration
		void operator << (const Table&);

	protected:
		bool _const = false;
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
		bool _inline = true;
	public:
		bool isVoid(void) const;
		bool isInt(void) const;
		const std::vector<const Entry*>& argList(void) const;
		const std::vector<const MidCode*>& midcodes(void) const;
		bool hasRet(void) const;
		bool isInline(void) const;

	protected:
		FuncTable(const std::string&);
		FuncTable(const std::string&, const bool);
		virtual ~FuncTable(void);

	public:
		void setHasRet(void); // can only be called by `Stat::block`
		void setRecursive(void);
		const Entry* pushArg(const std::string&, const bool isInt);
	};
}

#endif /* TABLE_H */
