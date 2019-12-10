/**********************************************
    > File Name: memory.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov 13 21:16:19 2019
 **********************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <set>
#include <vector>
#include "symtable.h"

#include "Reg.h"
#include "ObjCode.h"

// Small bss is originally a segment of memory specialized for storing global variables.
// In this project, the meaning of `Sbss` is broadened, so that the segment of memory
// storing local variables can also be described by `Sbss`. However, as a convension,
// local variables need to be described by `StackFrame`, which is a subclass of `Sbss`.
class Sbss {
    // singleton design pattern
	static const Sbss* _global;
protected:
	static const Sbss* global(void);
public:
	static void init(void); // setup `_global`
	static void deinit(void); // delete `_global`

private:
	std::map<const symtable::Entry*, int> _syms; // maps entrys to their address offset
	int _size = 0;
protected:
	virtual int size(void) const;
	
public:
	Sbss(const std::set<const symtable::Entry*>& syms);
    virtual ~Sbss(void) {}

	virtual int locate(const symtable::Entry*) const;
};

// `StackFrame` describes a segment of memory preserved for a specific function,
// whenever it is called. `StackFrame` supports the following querys
//
//     - the offset of a variable from `sp`
//     - the offset of a saved reg from `sp`
//
// Notice that only local variables can be accessed through `StackFrame`, because
// global variables are stored based on `gp`. For compilation, these query options
// should be enough if not redundent, since most operations could be done using
// `StackFrame`s' built-in functionalities listed below
//
//     1. restore/backup a reg
//     2. load/store a reg from/to a variable
//     3. load/store a reg from/to an array element
//
// Each of the functionalities are supported by one of the `_visit` functions. Check
// their comment for more info.
class StackFrame : protected Sbss {
    const objcode::CodeGen& _output; // call back function to generate objcode
	std::map<const symtable::Entry*, int> _args; // first 4 args are not mapped
	int _regBase;
	int _size;
public:
	virtual int size(void) const;

    // Since args are stored away from the other variables, `syms` cannot
    // have intersections with `argList`.
    StackFrame(const objcode::CodeGen&, std::vector<const symtable::Entry*> argList,
			const std::set<const symtable::Entry*>& syms);
    virtual ~StackFrame(void) {}

    // Only locate the local variables. Attempt to locate a global variable
    // would trigger assertion error.
	virtual int locate(const symtable::Entry* const) const;
    int locateGlobal(const symtable::Entry* const) const;
    
	int locate(const Reg) const;

public:
	void storeReg(const Reg) const;
	void loadReg(const Reg) const;
	void storeSym(const Reg, const symtable::Entry* const) const;
	void loadSym(const Reg, const symtable::Entry* const) const;
};

#endif /* MEMORY_H */
