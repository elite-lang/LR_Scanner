#include <map>
#include <string>
#include "DebugMsg.h"
#include <cereal/types/map.hpp>

using namespace std;

#ifndef VMAP_H
#define VMAP_H

struct VMap {
    map<string, int> VnMap; // 用来处理非终结符
    map<string, int> VtMap; // 用来处理终结符
    map<string, int> ConstMap; // 用来处理静态终结符
    int constSize = 0;
    int constMax = 0;
    int InsertVn(const char* s) {
        auto i = VnMap.find(s);
        if (i != VnMap.end()) return i->second;
        VnMap[s] = ++constSize;
        return constSize;
    }
    int getVn(const char* s){
        return VnMap[s];
    }

    void InsertVt(const char* s, int id) {
        VtMap[s] = id;
    }
    int getVt(const char* s){
        if (VtMap.find(s) == VtMap.end()) return -1;
        return VtMap[s];
    }

    int InsertConst(const char* s) {
        auto i = ConstMap.find(s);
        if (i != ConstMap.end()) return i->second;
        ConstMap[s] = ++constSize;
        return constSize;
    }
    int getConst(const char* s){
        if (s == NULL) return -1;
        if (ConstMap.find(s) == ConstMap.end()) return -1;
        return ConstMap[s];
    }

    void print(map<string,int>& V){
        auto& fout = DebugMsg::parser_dbg();
        for (auto p = V.begin(); p != V.end(); ++p)
        {
            pair<string, int> t = *p;
            fout <<  t.first << ":" << t.second << endl;
        }
    }
    void printAll() {
        auto& fout = DebugMsg::parser_dbg();
        fout << "===== Vn =====" << endl;
        print(VnMap);
        fout << "===== Vt =====" << endl;
        print(VtMap);
        fout << "===== Const =====" << endl;
        print(ConstMap);
    }


    // from Vn id to Vn index in the vector
    int VnIDtoIndex(int p) { if (p<=constMax) return -1;  return p-constMax-1; }

    // 串行化

    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &ar)
    {
        // serialize things by passing them to the archive
        ar( constSize, constMax, CEREAL_NVP(VnMap), CEREAL_NVP(VtMap), CEREAL_NVP(ConstMap) );
    }


};

#endif
