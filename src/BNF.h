/*
* @Author: sxf
* @Date:   2014-12-31 08:38:50
* @Last Modified by:   sxf
* @Last Modified time: 2015-11-10 19:44:06
*/

#ifndef BNF_H
#define BNF_H

#include "State.h"
#include <vector>
#include <map>
#include <string>
#include "VMap.h"
#include <cereal/types/vector.hpp>

using namespace std;

class BNF
{
public:
    BNF() { scriptcode = 0; }
    ~BNF() {}

    // 通过EBNF得出的root节点，遍历所有可能的BNF范式
    static vector<BNF*> BuildAllBNF(State*,VMap&);

    void print_bnf() const;
    void print_bnf(int) const;
    string get_graphviz_bnf(int) const;

    void addBNFdata(State* _s) { BNFdata.push_back(_s); }

    // ======= setter and getter ========

    void setRoot(State* _s) { root = _s; }
    State* getRoot() const { return root; }
    const vector<State*>& getBNFdata() const { return BNFdata; }
    void setID(int _id) { id = _id; }
    int getID() const { return id; }
    char* getScript() { return bnf_script; }
    int& getScriptCode() { return scriptcode; }




protected:
    static void BuildFromNode(State*,VMap&);


private:
    int id;
    // 一条BNF范式
    State* root;
    vector<State*> BNFdata;
    char* bnf_script = NULL;
    int scriptcode = -1;

    static int temp_size;
    static vector<BNF*> bnfs;



    // 串行化

    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &ar)
    {
        // serialize things by passing them to the archive
        // 特殊处理可有可无的script
        string script;
        if (bnf_script != NULL) script = bnf_script;
        else script = "";
        ar( id, script, *root );
        for (auto p : BNFdata) {
            ar(*p);
        }
    }
};


#endif // BNF_H
