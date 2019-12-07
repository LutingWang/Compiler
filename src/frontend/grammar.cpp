/**********************************************
    > File Name: grammar.cpp
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Thu Sep 26 21:58:02 2019
 **********************************************/

#include <cassert>
#include <string>
#include "error.h"
#include "midcode/MidCode.h"
#include "symtable.h"

#include "./Lexer.h"
#include "./Symbol.h"

#include "frontend.h"

/* forward declarations */

namespace basics {
    bool add(bool&);
    bool mult(bool&);
    bool typeId(bool&);
}

class Const {
    static void def(void);
public:
    static void dec(void);
};

class Var {
    static unsigned int index(void);
    static bool def(const bool);
public:
    static void dec(void);
};

// For `factor`, `item`, and `expr`
//     output : mid code target t0
//
// The output of these three functions are ensured
// to be not null. Callers can determine whether
// the `expr` is int by checking `isInt` field of
// the output.
//
// Note that the return value in `integer` has
// different meanings. Check the source code for
// further information.
class Expr {
    static const symtable::Entry* factor(void);
    static const symtable::Entry* item(void);
public:
    static bool integer(int&);
    static const symtable::Entry* expr(void);
};

// The return values indicate whether the current
// <stat> has <ret> covering all the paths. All
// non-terminals with <stat> inside (except for
// <block>) should return.
class Stat {
    class Cond {
        static void cond(const bool, const std::string&);
    public:
        static bool _if(void);
        static void _while(void);
        static bool _do(void);
        static void _for(void);
    };
    static void read(void);
    static void write(void);
    static void ret(void);
    static void assign(const symtable::Entry* const);
    static bool stat(void);
public:
    // Since <block> can only show up in <func def>, it
    // is obligated to check whether <ret> has covered
    // all paths. Thus, it does not have a return value.
    static void block(void);
};

class Func {
    static void args(void);
    static void def(void);
public:
    static void dec(void);
    static const symtable::Entry* argValues(const symtable::FuncTable* const);
};

/* basics */

// template para : type of desired and oppo symbol
// input :
//        - result : inout parameter indicating whether symbol is desired
//        - desired : desired symbol
//        - oppo : undesired symbol
// output : whether the symbol is either desired or oppo
template<const symbol::Type type>
bool _template(bool& result, const unsigned int desired, const unsigned int oppo) {
    if (!sym.is(type)) {
        result = false;
        return false;
    }
    result = sym.numIs(desired);
    if (result || sym.numIs(oppo)) {
        Lexer::getsym();
        return true;
    } else { return false; }
}

// <add op> ::= +|-
bool basics::add(bool& isNeg) {
    return _template<symbol::Type::OPER>(isNeg, symbol::MINU, symbol::PLUS);
}

// <mult op> ::= *|/
bool basics::mult(bool& isMult) {
    return _template<symbol::Type::OPER>(isMult, symbol::MULT, symbol::DIV);
}

// <type id> ::= int|char
bool basics::typeId(bool& isInt) {
    return _template<symbol::Type::RESERVED>(isInt, symbol::INTTK, symbol::CHARTK);
}

/* Const */

// <const def> ::= int<iden>=<integer>{,<iden>=<integer>}|char<iden>=<char>{,<iden>=<char>}
void Const::def(void) {
    // do not use `basics::typeId` or it will read a sym
    assert(sym.is(symbol::Type::RESERVED, symbol::INTTK|symbol::CHARTK));
    const bool isInt = sym.numIs(symbol::INTTK);

    // recursively identify identifier and its value
    do {
        Lexer::getsym();
        assert(sym.is(symbol::Type::IDENFR));
        const std::string idenName = sym.str();

        Lexer::getsym();
        assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));

        Lexer::getsym();
        int num = sym.ch();
        if (!isInt && sym.is(symbol::Type::CHARCON)) { Lexer::getsym(); }
        // error happens if symbol is char or the value is not an integer
        else if (!isInt || !Expr::integer(num)) {
            error::raise(error::Code::EXPECTED_LITERAL);
            // jump to the next ',' or ';'
            while (!sym.is(symbol::Type::DELIM, symbol::COMMA|symbol::SEMICN)) {
                Lexer::getsym();
            }
        }
        SymTable::getTable().curTable().pushConst(idenName, isInt, num);
    } while (sym.is(symbol::Type::DELIM, symbol::COMMA));
}

// <const dec> ::= {const<const def>;}
void Const::dec(void) {
    while (sym.is(symbol::Type::RESERVED, symbol::CONSTTK)) {
        Lexer::getsym();
        def();
        error::assertSymIsSEMICN();
    }
}

/* Var */

// <index> ::= '['<unsigned int>']'
// output : identified length of array
unsigned int Var::index(void) {
    assert(sym.is(symbol::Type::DELIM, symbol::LBRACK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::INTCON));
    const unsigned int result = sym.num();
    Lexer::getsym();
    error::assertSymIsRBRACK();
    return result;
}

// <var def> ::= <iden>[<index>]{,<iden>[<index>]}
// traceback = true
// input : type of variable is int or char
// output : returned normally or as a result of traceback
bool Var::def(const bool isInt) {
    if (!sym.is(symbol::Type::IDENFR)) { return false; }
    const symbol::Symbol lastSymbol = sym;
    Lexer::getsym();
    if (!sym.is(symbol::Type::DELIM)) {
        Lexer::traceback(lastSymbol);
        return false;
    }

    std::string idenName = lastSymbol.str();
    if (sym.numIs(symbol::LBRACK)) {
        SymTable::getTable().curTable().pushArray(idenName, isInt, index());
    } else if (sym.numIs(symbol::COMMA|symbol::SEMICN)) {
        SymTable::getTable().curTable().pushVar(idenName, isInt);
    } else {
        Lexer::traceback(lastSymbol);
        return false;
    }

    // traceback = false
    while (sym.is(symbol::Type::DELIM, symbol::COMMA)) {
        Lexer::getsym();
        assert(sym.is(symbol::Type::IDENFR));
        idenName = sym.str();
        Lexer::getsym();
        assert(sym.is(symbol::Type::DELIM));
        if (sym.numIs(symbol::LBRACK)) {
            SymTable::getTable().curTable().pushArray(idenName, isInt, index());
        } else {
            SymTable::getTable().curTable().pushVar(idenName, isInt);
        }
    }
    return true;
}

// <var dec> ::= {<type id><var def>;}
// traceback = true;
void Var::dec(void) {
    bool isInt;
    for (symbol::Symbol lastSymbol = sym; basics::typeId(isInt); lastSymbol = sym) {
        if (!def(isInt)) {
            Lexer::traceback(lastSymbol);
            return;
        }
        error::assertSymIsSEMICN();
    }
}

/* Expr */

// <integer> ::= [<add op>]<unsigned int>
// input : inout parameter for the identified integer
// output : identified an integer
bool Expr::integer(int& result) {
    const symbol::Symbol lastSymbol = sym;
    bool neg;
    if (basics::add(neg) && !sym.is(symbol::Type::INTCON)) {
        Lexer::traceback(lastSymbol);
        return false;
    }
    if (!sym.is(symbol::Type::INTCON)) { return false; }
    result = neg ? -((int) sym.num()) : sym.num();
    Lexer::getsym();
    return true;
}

// <factor> ::= <iden>['['<expr>']']|'('<expr>')'|<integer>|<char>|<func call>
const symtable::Entry* Expr::factor(void) {
    const symtable::Entry* t0 = nullptr;
    switch (sym.id()) {
    case symbol::Type::DELIM:
        assert(sym.numIs(symbol::LPARENT));
        Lexer::getsym();
        t0 = MidCode::genVar(true);
        MidCode::gen(MidCode::Instr::ASSIGN, t0, expr(), nullptr);
        error::assertSymIsRPARENT();
        break;
    case symbol::Type::OPER: case symbol::Type::INTCON:
        int num;
        assert(integer(num));
        t0 = MidCode::genConst(true, num);
        break;
    case symbol::Type::CHARCON:
        t0 = MidCode::genConst(false, sym.ch());
        Lexer::getsym();
        break;
    case symbol::Type::IDENFR: {
            std::string name = sym.str();
            Lexer::getsym();
            if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
                t0 = Func::argValues(SymTable::getTable().findFunc(name));
            } else if (sym.is(symbol::Type::DELIM, symbol::LBRACK)) {
                const symtable::Entry* const t1 = SymTable::getTable().findSym(name);
                assert(t1->isInvalid() || t1->isArray());
                Lexer::getsym();
                const symtable::Entry* const t2 = expr();
                if (!t2->isInt()) { error::raise(error::Code::ILLEGAL_IND); }
                error::assertSymIsRBRACK();
                t0 = MidCode::genVar(t1->isInt());
                MidCode::gen(MidCode::Instr::LOAD_IND, t0, t1, t2); // t0 = t1[t2];
            } else {
                t0 = SymTable::getTable().findSym(name);
                assert(t0->isInvalid() || !t0->isArray());
            }
        }
        break;
    default: assert(0);
    }
    assert(t0 != nullptr);
    return t0;
}

// <item> ::= <factor>{<mult op><factor>}
const symtable::Entry* Expr::item(void) {
    const symtable::Entry* const t1 = factor();
    bool isMult;
    if (!basics::mult(isMult)) {
        assert(t1 != nullptr);
        return t1;
    }
    const symtable::Entry* const t0 = MidCode::genVar(true);
    MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV,
            t0, t1, factor()); // t0 = t1 [*/] t2
    while (basics::mult(isMult)) {
        MidCode::gen(isMult ? MidCode::Instr::MULT : MidCode::Instr::DIV,
                t0, t0, factor()); // t0 = t0 [*/] t2
    }
    return t0;
}

// <expr> ::= [<add op>]<item>{<add op><item>}
const symtable::Entry* Expr::expr(void) {
    const symtable::Entry* t0 = nullptr;
    const symtable::Entry* t1;
    bool neg;
    if (basics::add(neg)) {
        t0 = MidCode::genVar(true);
        t1 = MidCode::genConst(true, 0);
        MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD,
                t0, t1, item()); // t0 = 0 [+-] t2
    } else {
        t1 = item();
        if (basics::add(neg)) {
            t0 = MidCode::genVar(true);
            MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD,
                    t0, t1, item()); // t0 = t1 [+-] t2
        } else {
            return t1;
        }
    }

    while (basics::add(neg)) {
        MidCode::gen(neg ? MidCode::Instr::SUB : MidCode::Instr::ADD,
                t0, t0, item()); // t0 = t0 [+-] t2
    }
    return t0;
}

/* Stat */

MidCode::Instr translate(symbol::Comp comp, bool takeNot) {
    switch (comp) {
#define CASE(id, pos, neg) \
    case symbol::id: \
        return takeNot ? MidCode::Instr::neg : MidCode::Instr::pos
    CASE(LSS, BLT, BGE);
    CASE(LEQ, BLE, BGT);
    CASE(GRE, BGT, BLE);
    CASE(GEQ, BGE, BLT);
    CASE(EQL, BEQ, BNE);
    CASE(NEQ, BNE, BEQ);
#undef CASE
    default: assert(0);
    }
}

// <cond> ::= <expr>[<comp op><expr>]
void Stat::Cond::cond(const bool branchIfNot, const std::string& labelName) {
    const symtable::Entry* const t1 = Expr::expr();
    if (!t1->isInt()) { error::raise(error::Code::MISMATCHED_COND_TYPE); }
    if (sym.is(symbol::Type::COMP)) {
        MidCode::Instr comp = translate(static_cast<symbol::Comp>(sym.num()), branchIfNot);
        Lexer::getsym();
        const symtable::Entry* const t2 = Expr::expr();
        if (!t2->isInt()) { error::raise(error::Code::MISMATCHED_COND_TYPE); }
        MidCode::gen(comp, nullptr, t1, t2, labelName);
    } else {
        MidCode::gen(branchIfNot ? MidCode::Instr::BEQ : MidCode::Instr::BNE, nullptr,
                t1, MidCode::genConst(true, 0), labelName);
    }
}

// <if stat> ::= if'('<cond>')'<stat>[else<stat>]
bool Stat::Cond::_if(void) {
    assert(sym.is(symbol::Type::RESERVED, symbol::IFTK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();
    std::string labelElse = MidCode::genLabel();
    cond(true, labelElse);
    error::assertSymIsRPARENT();

    bool hasRet = stat();
    if (sym.is(symbol::Type::RESERVED, symbol::ELSETK)) {
        std::string labelEnd = MidCode::genLabel();
        MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelEnd);
        MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
        Lexer::getsym();
        hasRet = stat() && hasRet; // ensures that stat() is executed
        MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
        return hasRet;
    } else {
        MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelElse);
        return false;
    }
}

// <while stat> ::= while'('<cond>')'<stat>
void Stat::Cond::_while(void) {
    assert(sym.is(symbol::Type::RESERVED, symbol::WHILETK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();
    std::string labelBegin = MidCode::genLabel();
    MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
    std::string labelEnd = MidCode::genLabel();
    cond(true, labelEnd);
    error::assertSymIsRPARENT();
    stat();
    MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
    MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
}

// <do stat> ::= do<stat>while'('<cond>')'
bool Stat::Cond::_do(void) {
    assert(sym.is(symbol::Type::RESERVED, symbol::DOTK)); // ensured by outer function
    Lexer::getsym();
    std::string labelBegin = MidCode::genLabel();
    MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
    bool hasRet = stat();
    if (sym.is(symbol::Type::RESERVED, symbol::WHILETK)) { Lexer::getsym(); }
    else { error::raise(error::Code::MISSING_WHILE); }
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();
    cond(false, labelBegin);
    error::assertSymIsRPARENT();
    return hasRet;
}

// <for stat> ::= for'('<iden>=<expr>;<cond>;<iden>=<iden><add op><unsigned int>')'<stat>
void Stat::Cond::_for(void) {
    const symtable::Entry* t0;
    const symtable::Entry* t1;

    // for'('
    assert(sym.is(symbol::Type::RESERVED, symbol::FORTK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();

    // <iden>=<expr>;
    assert(sym.is(symbol::Type::IDENFR));
    t0 = SymTable::getTable().findSym(sym.str());
    if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
    else { assert(t0->isInvalid() || !t0->isArray()); }
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
    Lexer::getsym();
    t1 = Expr::expr();
    MidCode::gen(MidCode::Instr::ASSIGN, t0, t1, nullptr);
    error::assertSymIsSEMICN();

    // <cond>;
    std::string labelBegin = MidCode::genLabel();
    MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelBegin);
    std::string labelEnd = MidCode::genLabel();
    cond(true, labelEnd);
    error::assertSymIsSEMICN();

    // <iden>=<iden><add op><unsigned int>')'
    assert(sym.is(symbol::Type::IDENFR));
    t0 = SymTable::getTable().findSym(sym.str());
    if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
    else { assert(t0->isInvalid() || !t0->isArray()); }
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
    Lexer::getsym();
    assert(sym.is(symbol::Type::IDENFR));
    t1 = SymTable::getTable().findSym(sym.str());
    assert(t1->isInvalid() || !t1->isArray());
    Lexer::getsym();
    bool minus;
    assert(basics::add(minus));
    assert(sym.is(symbol::Type::INTCON));
    const symtable::Entry* const stepSize = MidCode::genConst(true, sym.num());
    Lexer::getsym();
    error::assertSymIsRPARENT();

    stat();

    MidCode::gen(minus ? MidCode::Instr::SUB : MidCode::Instr::ADD, t0, t1, stepSize);
    MidCode::gen(MidCode::Instr::GOTO, nullptr, nullptr, nullptr, labelBegin);
    MidCode::gen(MidCode::Instr::LABEL, nullptr, nullptr, nullptr, labelEnd);
}

// <read stat> ::= scanf'('<iden>{,<iden>}')'
void Stat::read(void) {
    assert(sym.is(symbol::Type::RESERVED, symbol::SCANFTK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    do {
        Lexer::getsym();
        assert(sym.is(symbol::Type::IDENFR));
        const symtable::Entry* const t0 = SymTable::getTable().findSym(sym.str());
        if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
        else { assert(t0->isInvalid() || !t0->isArray()); }
        MidCode::gen(MidCode::Instr::INPUT, t0, nullptr, nullptr);
        Lexer::getsym();
    } while (sym.is(symbol::Type::DELIM, symbol::COMMA));
    error::assertSymIsRPARENT();
}

// <write stat> ::= printf'('<string>[,<expr>]')'|printf'('<expr>')'
void Stat::write(void) {
    assert(sym.is(symbol::Type::RESERVED, symbol::PRINTFTK)); // ensured by outer function
    Lexer::getsym();
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();
    
    int nfield = 0;
    if (sym.is(symbol::Type::STRCON)) {
        nfield++;
        MidCode::gen(MidCode::Instr::OUTPUT_STR, nullptr, nullptr, nullptr, sym.str());
        std::string str = sym.str();
        Lexer::getsym();
        if (sym.is(symbol::Type::DELIM, symbol::COMMA)) {
            Lexer::getsym();
        }
    }
    if (!sym.is(symbol::Type::DELIM, symbol::RPARENT)) {
        nfield++;
        const symtable::Entry* const t1 = Expr::expr();
        if (t1->isInt()) {
            MidCode::gen(MidCode::Instr::OUTPUT_INT, nullptr, t1, nullptr);
        } else {
            MidCode::gen(MidCode::Instr::OUTPUT_CHAR, nullptr, t1, nullptr);
        }
    }
    assert(nfield);
    MidCode::gen(MidCode::Instr::OUTPUT_STR, nullptr, nullptr, nullptr, "\\n");
    error::assertSymIsRPARENT();
}

// <ret stat> ::= return['('<expr>')']
void Stat::ret(void) {
    const symtable::Entry* t1 = nullptr;

    assert(sym.is(symbol::Type::RESERVED, symbol::RETURNTK)); // ensured by outer function
    Lexer::getsym();
    if (SymTable::getTable().curFunc().isVoid()) {
        if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
            error::raise(error::Code::ILLEGAL_RET_WITH_VAL);
            Lexer::getsym();
            Expr::expr();
            error::assertSymIsRPARENT();
        }
    } else if (sym.is(symbol::Type::DELIM, symbol::LPARENT)) {
        Lexer::getsym();
        t1 = Expr::expr();
        if (SymTable::getTable().curFunc().isInt() != t1->isInt()) {
            error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
        }
        error::assertSymIsRPARENT();
    } else {
        error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
    }
    MidCode::gen(MidCode::Instr::RET, nullptr, t1, nullptr);
}

// <assign> ::= <iden>['['<expr>']']=<expr>
// <iden> is provided by outer function as t0.
void Stat::assign(const symtable::Entry* const t0) {
    const symtable::Entry* t2 = nullptr;
    if (t0->isConst()) { error::raise(error::Code::ILLEGAL_ASSIGN); }
    assert(sym.is(symbol::Type::DELIM));
    if (sym.numIs(symbol::LBRACK)) {
        assert(t0->isInvalid() || t0->isArray());
        Lexer::getsym();
        t2 = Expr::expr();
        if (!t2->isInt()) { error::raise(error::Code::ILLEGAL_IND); }
        error::assertSymIsRBRACK();
    } else {
        assert(t0->isInvalid() || !t0->isArray());
    }

    assert(sym.is(symbol::Type::DELIM, symbol::ASSIGN));
    Lexer::getsym();
    MidCode::gen(t2 == nullptr ? MidCode::Instr::ASSIGN : MidCode::Instr::STORE_IND,
            t0, Expr::expr(), t2); // t0 = t1[t2];
}

// <stat> ::= <if stat>|<while stat>|<do stat>|<for stat>|'{'{<stat>}'}'|<read stat>;|<write stat>;|<ret stat>;|<assign>;|<func call>;|;
bool Stat::stat(void) {
    bool hasRet = false;
    switch (sym.id()) {
    case symbol::Type::RESERVED:
        switch (sym.num()) {
        case symbol::MAINTK: // calling to main is not exactly a function call
            assert(SymTable::getTable().isMain());
            Lexer::getsym();
            assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
            Lexer::getsym();
            error::assertSymIsRPARENT();
            MidCode::gen(MidCode::Instr::CALL, nullptr, nullptr, nullptr, "main");
            break;
        case symbol::IFTK:
            hasRet = Cond::_if();
            break;
        case symbol::WHILETK:
            Cond::_while();
            break;
        case symbol::DOTK:
            hasRet = Cond::_do();
            break;
        case symbol::FORTK:
            Cond::_for();
            break;
        case symbol::SCANFTK:
            read();
            error::assertSymIsSEMICN();
            break;
        case symbol::PRINTFTK:
            write();
            error::assertSymIsSEMICN();
            break;
        case symbol::RETURNTK:
            hasRet = true; // recursion exit
            ret();
            error::assertSymIsSEMICN();
            break;
        default: assert(0);
        }
        break;
    case symbol::Type::IDENFR: {
            std::string name = sym.str();
            Lexer::getsym();
            assert(sym.is(symbol::Type::DELIM));
            if (sym.numIs(symbol::LPARENT)) {
                Func::argValues(SymTable::getTable().findFunc(name));
            } else {
                assign(SymTable::getTable().findSym(name));
            }
            error::assertSymIsSEMICN();
        }
        break;
    case symbol::Type::DELIM:
        switch (sym.num()) {
        case symbol::LBRACE:
            Lexer::getsym();
            while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) {
                hasRet = stat() || hasRet;
            }
            Lexer::getsym();
            break;
        case symbol::SEMICN:
            Lexer::getsym();
            break;
        default: assert(0);
        }
        break;
    default: assert(0);
    }
    return hasRet;
}

// <block> ::= '{'<const dec><var dec>{<stat>}'}'
void Stat::block(void) {
    assert(sym.is(symbol::Type::DELIM, symbol::LBRACE));
    Lexer::getsym();
    Const::dec();
    Var::dec();

    while (!sym.is(symbol::Type::DELIM, symbol::RBRACE)) {
        if (stat()) {
            SymTable::getTable().curFunc().setHasRet();
        }
    }

    // function main does not have subsequent symbols
    if (!SymTable::getTable().isMain()) { Lexer::getsym(); }

    const symtable::FuncTable& functable = SymTable::getTable().curFunc();
    if (functable.hasRet()) { return; }
    // for non-void functions, the default <ret> will not fit
    if (!functable.isVoid()) {
        error::raise(error::Code::ILLEGAL_RET_WITHOUT_VAL);
    }
    MidCode::gen(MidCode::Instr::RET, nullptr, nullptr, nullptr);
}

/* Func */

// <args> ::= [<type id><iden>{,<type id><iden>}]
void Func::args(void) {
    bool isInt;
    if (!basics::typeId(isInt)) { return; } // empty is allowed
    assert(sym.is(symbol::Type::IDENFR));
    SymTable::getTable().curFunc().pushArg(sym.str(), isInt);
    for (Lexer::getsym(); sym.is(symbol::Type::DELIM, symbol::COMMA); Lexer::getsym()) {
        Lexer::getsym();
        assert(basics::typeId(isInt));
        assert(sym.is(symbol::Type::IDENFR));
        SymTable::getTable().curFunc().pushArg(sym.str(), isInt);
    }
}

// <func def> ::= '('<args>')'<block>
void Func::def(void) {
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT));
    Lexer::getsym();
    args();
    error::assertSymIsRPARENT();
    Stat::block();
}

// <func dec> ::= {(<type id>|void)<iden><func def>}void main<func def>
void Func::dec(void) {
    bool isInt;
    while (true) {
        assert(sym.is(symbol::Type::RESERVED));
        if (!basics::typeId(isInt)) {
            assert(sym.numIs(symbol::VOIDTK));
            Lexer::getsym();
            if (!sym.is(symbol::Type::IDENFR)) { break; }
            SymTable::getTable().pushFunc(sym.str());
        } else {
            assert(sym.is(symbol::Type::IDENFR));
            SymTable::getTable().pushFunc(sym.str(), isInt);
        }
        Lexer::getsym();
        def();
    }
    assert(sym.is(symbol::Type::RESERVED, symbol::MAINTK));
    SymTable::getTable().pushFunc();
    Lexer::getsym();
    def();
}

// <arg values> ::= '('[<expr>{,<expr>}]')'
// input : information on the corresponding function
// output : mid code target t0
//
// Output is set to null only when function is void. Otherwise,
// the `isInt` field of return value indicates whether the
// original function was int.
//
// This function is obligated to check whether the arg values
// match with the function declaration.
const symtable::Entry* Func::argValues(const symtable::FuncTable* const functable) {
    assert(sym.is(symbol::Type::DELIM, symbol::LPARENT)); // ensured by outer function
    Lexer::getsym();

    std::vector<const symtable::Entry*> argv;
    if (!sym.is(symbol::Type::DELIM, symbol::RPARENT|symbol::SEMICN)) { // ')' might be missing
        while (true) {
            argv.push_back(Expr::expr());
            if (!sym.is(symbol::Type::DELIM, symbol::COMMA)) { break; }
            Lexer::getsym();
        }
    }
    error::assertSymIsRPARENT();

    if (functable == nullptr) {
        return nullptr;
    }

    // check and push args
    const std::vector<const symtable::Entry*>& arglist = functable->argList();
    if (argv.size() != arglist.size()) {
        error::raise(error::Code::MISMATCHED_ARG_NUM);
    } else for (int i = 0; i < argv.size(); i++) {
        assert(!arglist[i]->isConst()); // ensured by symtable
        if (argv[i]->isInt() != arglist[i]->isInt()) {
            error::raise(error::Code::MISMATCHED_ARG_TYPE);
            break;
        }
        MidCode::gen(MidCode::Instr::PUSH_ARG, nullptr, argv[i], nullptr);
    }

    // generate mid code
    const symtable::Entry* const t0 = functable->isVoid() ? nullptr : MidCode::genVar(functable->isInt());
    MidCode::gen(MidCode::Instr::CALL, t0, nullptr, nullptr, functable->name());
    return t0;
}

// <program> ::= <const dec><var dec><func dec>
void grammar::parse(const std::string& inputPath) try {
	Lexer::init(inputPath);
	Lexer::getsym();
	Const::dec();
	Var::dec();
	Func::dec();
	Lexer::deinit();
    SymTable::getTable().makeConst();
} catch (error::Ueof& e) {
	error::raise(error::Code::UNEXPECTED_EOF);
}

