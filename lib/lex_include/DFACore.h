#ifndef DFACORE_H
#define DFACORE_H
#include "DFA.h"
#include "Lex.h"
#include "EquivalenceClass.h"

class DFACore
{
public:
    DFACore() {}
    ~DFACore() { if (t != NULL) delete t; }
    
    DFACore(DFA* _dfa, const char* _d, EquivalenceClass* _pEClass) { 
        Init(_dfa,_d,_pEClass); 
    }

    void Init(DFA* _dfa, const char* _d, EquivalenceClass* _pEClass) {
        dfa = _dfa; data = _d; pEClass = _pEClass;
        point = 0; row_point =1; line_point = 0;
    }

    Token* Read();

private:
    DFA* dfa;
    int state;
    Glib::ustring tokendata; // 当前的token数据
    Glib::ustring data; // 全文件数据
    int point, row_point, line_point; //处理位置指针
    Token* t = NULL; // 上一次的token
    EquivalenceClass* pEClass;
};

#endif // DFACORE_H
