/**********************************************
    > File Name: VarMatch.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:56 2019
 **********************************************/

#include <cassert>
#include "symtable/Entry.h"

#include "./VarMatch.h"

VarMatch::VarMatch(void) {} // TODO: allow initialization from data flow

bool VarMatch::contains(const symtable::Entry* const entry) const {
    assert(entry == nullptr || !entry->isInvalid());
    return _matches.count(entry);
}

const symtable::Entry* VarMatch::map(const symtable::Entry* const entry) const {
    assert(entry == nullptr || !entry->isInvalid());
    return contains(entry) ? _matches.at(entry) : entry;
}

void VarMatch::update(const symtable::Entry* const lhs, const symtable::Entry* const rhs) {
    assert(lhs != nullptr && rhs != nullptr);
    assert(!lhs->isConst() && !lhs->isArray() && !lhs->isInvalid());
    assert(!rhs->isArray() && !rhs->isInvalid());
    _matches[lhs] = rhs;
}

void VarMatch::erase(const symtable::Entry* const entry) {
    assert(entry == nullptr || (!entry->isConst() && !entry->isInvalid()));
    _matches.erase(entry);
}
