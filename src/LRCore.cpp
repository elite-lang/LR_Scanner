/*
* @Author: sxf
* @Date:   2015-01-03 18:43:13
* @Last Modified by:   sxf
* @Last Modified time: 2015-12-24 21:29:51
*/

#include "LRCore.h"
#include "DebugMsg.h"


Grammer_Node* LRCore::Run(){
    auto& fout = DebugMsg::parser_dbg();
    fout << "===== LRCore Run =====" << endl;

    script_runner->Init();
    Token* t = TokenFliter(lex->Read());

    LRStack.push_back(0); // 放入0号根
    NodeIntStack.push_back(0);
    int s;
    while (1) {
        s = LRStack.back();
//        printf("Stack Top: %d\n",s);
        char c = table->ACTION(s,t->type);
        switch (c) {
        case 's': {
            Shift(table->GOTO(s,t->type),t);
            dbg_line_vec.push_back(new DbgLine(s, t->type, 's', LRStack, NodeIntStack));
            t = TokenFliter(lex->Read());
            break;
        }
        case 'r': {
            Grammer_Node* root = ast->NewNode();
            int Vn = Reduce(table->GOTO(s,t->type),root);
            dbg_line_vec.push_back(new DbgLine(s, t->type, 'r', LRStack, NodeIntStack));
            s = LRStack.back();
//            printf("Stack Top: %d, Vn:%d\n",s,Vn);
            Shift(table->GOTO(s,Vn),root);
            NodeIntStack.push_back(Vn);
            dbg_line_vec.push_back(new DbgLine(s, Vn, 's', LRStack, NodeIntStack));
            break;
        }
        case 'a':
            printf("Accept!\n");
            dbg_line_vec.push_back(new DbgLine(s, t->type, 'a', LRStack, NodeIntStack));
            script_runner->Finished();
            return ast;
        default:
            string pointer = t->debug_line;
            for (auto p = pointer.begin(); p != pointer.end(); ++p) {
                if ((p-pointer.begin()) == t->col_num) { *p = '^'; continue; }
                if (*p != ' ' && *p != '\t') { *p = '~'; continue; }
            }
            printf("%s\n", t->debug_line);
            printf("%s\n", pointer.c_str());
            printf("LRCore error\n");
            printf("错误的Action动作：%c\n", c);
            printf("目前的状态：%d\n", s);
            printf("Token-Type: %d\n",t->type);
            printf("Token: %s\n", t->pToken);
            printf("line: %d, %d\n",t->row_num, t->col_num);
            //TODO: 需要释放本层资源
            return (Grammer_Node*)-1;
        }
    }

}


Token* LRCore::TokenFliter(Token* token) {
    int size;
    auto& fout = DebugMsg::parser_dbg();
    if (token->type != NULL && token->pToken != NULL)
        fout << "next Token: " << token->type << " " << token->pToken << endl;
    int id = vmap->getConst(token->pToken);
    if (id != -1) token->type = id;
    if (token->pToken != NULL && *(token->pToken) == '#') // 这里过滤Token，将#开头的当做元脚本进行执行
        script_runner->RunLine(token->pToken);
    return token;
}

void LRCore::Shift(int x,Token* t){
    // for debug
    auto& fout = DebugMsg::parser_dbg();
    fout << "------------------------" << endl;
    fout << "Stack: ";
    for (auto p: LRStack)
    {
        fout << p << ' ';
    }

    fout << "Shift: " << x << endl;
    LRStack.push_back(x);
    NodeIntStack.push_back(t->type);
    NodeStack.push(ast->NewNode());
    NodeStack.top()->lua_data = script_runner->MakeNewLuaTable(t);
}

void LRCore::Shift(int x,Grammer_Node* root){

    LRStack.push_back(x);
    NodeStack.push(root);
}

int LRCore::Reduce(int x,Grammer_Node*& root){
    BNF* bnf = bnflist->at(x);

    // for debug
    auto& fout = DebugMsg::parser_dbg();
    fout << "------------------------" << endl;
    fout << "Stack: ";
    for (auto p: LRStack)
    {
        fout << p << ' ';
    }
    fout << "Reduce: " << x+1 << endl;
    bnf->print_bnf();

    // 从栈顶弹出对应BNF式元素数个
    int state_sum = bnf->getBNFdata().size();

     // 特殊判断一下epsilon的情况，不进行符号处理
    if (state_sum != 0) {

        vector<Grammer_Node*> tempstack; // 临时符号栈

        for (int i = 0; i < state_sum; ++i) {
            LRStack.pop_back();
            NodeIntStack.pop_back();
            tempstack.push_back(NodeStack.top()); // 这样push后是反向的数组
            NodeStack.pop();
        }
        Grammer_Node* headnode = tempstack.back(); // 获取头节点
        root->AddChildrenNode(headnode); // 设置为子元素
        tempstack.pop_back(); // 从第二个节点开始，若没有则自动跳过
        Grammer_Node* oldnode = headnode; // 滚动的上一个节点

        script_runner->ClearEnv();// 脚本环境清理
        int i = 0;
        for (auto p = tempstack.rbegin(); p != tempstack.rend(); ++p) {
            Grammer_Node* node = *p;
            oldnode->AddBortherNode(node);
            State* state = bnf->getBNFdata()[i];
            if (state->state_var != NULL) {
                // 这里是向LUA中注册变量的部分 MakeEnv
                script_runner->MakeEnv(state->state_var,oldnode);
            }
            oldnode = node;
            ++i;
        }
        State* state = bnf->getBNFdata()[i];
        if (state->state_var != NULL) {
            // 这里是向LUA中注册变量的部分 MakeEnv
            script_runner->MakeEnv(state->state_var,oldnode);
        }

    }
    // Do the lua script and make the AST
    if (bnf->getScript() != NULL) {
        fout << "Script: " << bnf->getScript() << endl;
        script_runner->Run(bnf->getScriptCode(),bnf->getScript(),root);
    }
    return bnf->getRoot()->id;
}
