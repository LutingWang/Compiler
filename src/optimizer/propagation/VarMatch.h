/**********************************************
    > File Name: VarMatch.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Fri Nov 29 15:27:47 2019
 **********************************************/

#ifndef VAR_MATCH_H
#define VAR_MATCH_H

#include <map>

namespace symtable {
	class Entry;
}

class VarMatch {
    std::map<const symtable::Entry*, const symtable::Entry*> _matches;
public:
    VarMatch(void);
    
    bool contains(const symtable::Entry* const) const;
    
    const symtable::Entry* map(const symtable::Entry* const) const;
    
    void erase(const symtable::Entry* const);
    
    void eraseGlobal(void);
    
    void match(const symtable::Entry* const, const symtable::Entry* const);
};

#endif /* VAR_MATCH_H */
