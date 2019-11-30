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
    const symtable::Entry* result = entry;
    while (contains(result)) {
        result = _matches.at(result);
        assert(result != nullptr && !result->isArray() && !result->isInvalid());
    }
    return result;
}

void VarMatch::erase(const symtable::Entry* const entry) {
    assert(entry == nullptr || (!entry->isConst() && !entry->isInvalid()));
    _matches.erase(entry);
    for (auto it = _matches.begin(); it != _matches.end(); ) {
        if (it->second == entry) {
            it = _matches.erase(it);
        } else {
            it++;
        }
    }
}

void VarMatch::eraseGlobal(void) {
    for (auto it = _matches.begin(); it != _matches.end(); ) {
        if (it->first->isGlobal() || it->second->isGlobal()) {
            it = _matches.erase(it);
        } else {
            it++;
        }
    }
}

void VarMatch::match(const symtable::Entry* const lhs, const symtable::Entry* const rhs) {
    assert(lhs != nullptr && rhs != nullptr);
    assert(!lhs->isConst() && !lhs->isArray() && !lhs->isInvalid());
    assert(!rhs->isArray() && !rhs->isInvalid());
    erase(lhs);
    _matches[lhs] = rhs;
}

