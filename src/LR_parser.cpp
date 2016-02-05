#include "LR_parser.h"
#include "BNFParser.h"
#include "LALRTable.h"
#include "ScriptRunner.h"
#include "Grammer_Node.h"
#include "FileUtils.h"
#include "DebugMsg.h"
#include <cereal/archives/json.hpp>

LR_parser::LR_parser()
{

}

LR_parser::~LR_parser()
{

}


void LR_parser::ExtendBNF()
{
    mainbnf = new BNF();
    State* s = new State();
    s->state_class = "<Main>";
    s->state_type = statement;
    s->id = vmap.InsertVn("<Main>");
    mainbnf->setRoot(s);
    mainbnf->setID(-1);
    mainbnf->addBNFdata(bnflist[0]->getRoot());
}

void LR_parser::MakeID()
{
    for (auto p = bnflist.begin(); p != bnflist.end(); ++p) {
        BNF* bnf = *p;
        State* s = bnf->getRoot();
        s->id = vmap.getVn(s->state_class);
        const vector<State*>& vec = bnf->getBNFdata();
        for (auto q = vec.begin(); q != vec.end(); ++q) {
            s = *q;
            if (s->state_type == constant)
            s->id = vmap.getConst(s->state_const);
            else if (s->state_type == temporality || s->state_type == statement )
            s->id = vmap.getVn(s->state_class);
            else if (s->state_type == epsilon )
            s->id = 0;
            else if (s->state_type == terminal )
            s->id = vmap.getVt(s->state_class);
        }
    }
}

void print_ItemCollection(vector<ItemCollection*> vec) {
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        printf("I%d:", items->getID());
        items->printSet();
    }
}


static const char begin_state[] = "style = \"filled, bold\" penwidth = 5 fillcolor = \"white\" fontname = \"Courier New\" shape = \"Mrecord\" ";
static const char end_state[] = "style = \"filled\" penwidth = 1 fillcolor = \"black\" fontname = \"Courier New\" shape = \"Mrecord\" ";
static const char normal_state[] = "style = \"filled\" penwidth = 1 fillcolor = \"white\" fontname = \"Courier New\" shape = \"Mrecord\" ";
void print_graphviz_ItemCollection(vector<ItemCollection*> vec, ostream& os) {
    os << "digraph g {" << endl
       << "graph [fontsize=30 labelloc=\"t\" label=\"\" splines=true overlap=false rankdir = \"LR\"];" << endl
       << "ratio = auto;" << endl;

    // 依次打印全部状态集
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        os << '\"' << "state" << items->getID() << '\"';
        os << '[' << endl;
        items->print_graphviz_Set(os);
        os << ']' << endl;
    }

    // 依次打印goto跳转

}

void LR_parser::print_GOTO(vector<ItemCollection*> vec) {
    printf("======= GOTO =======\n");
    for (int i = 0; i< vmap.constSize; ++i) {
        printf("\t%d",i);
    }
    printf("\n");
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        printf("I%d:",items->getID());
        for (int i = 0; i< vmap.constSize; ++i) {
            ItemCollection* gotoitems = items->GOTO(i);
            int gotoid = -1;
            if (gotoitems != NULL) {
                gotoid = gotoitems->getID();
            }
            printf("\t%d",gotoid);
        }
        printf("\n");
    }

    printf("======= Spread =======\n");
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        items->printSpread();
        printf("\n");
    }

}

void LR_parser::BuildParser()
{
    string save_filepath = cfg_filepath + ".lrsave";
    if (!FileUtils::isNeedUpdate(cfg_filepath, save_filepath)) {
        // 读取并加载该缓存
        printf("加载缓存\n");
        LALRTable* lalr_table = new LALRTable(vmap.constMax+1, 0, vmap.constSize, bnfparser);
        lalr_table->Load(save_filepath.c_str());
        table = (LRTable*) lalr_table;
        return;
    }

    printf("Create LR0\n");
    // 创建LR0项集族
    vector<ItemCollection*> vec = ItemCollection::MakeLR0Items(&vmap, mainbnf, bnflist);
    printf("======== print LR0 Collection ========\n");
    // print_ItemCollection(vec);
    // 构建LALR项集族
    ItemCollection::MakeLALRItems(vec,bnflist);
    printf("======== print LR1 Collection ========\n");
    // print_ItemCollection(vec);
    // print_GOTO(vec);
    // printf("test: \t %d %d %d\n",vmap.constMax+1,vec.size(),vmap.constSize);
    LALRTable* lalr_table = new LALRTable(vmap.constMax+1, vec.size(), vmap.constSize, bnfparser);
    lalr_table->BuildTable(vec);
    lalr_table->Save(save_filepath.c_str());
    table = (LRTable*) lalr_table;
}

void LR_parser::BuildParser(const char* filename) {
    AddBNF(filename);
    BuildParser();
}


void LR_parser::AddBNF(const char* filename) {
    cfg_filepath = filename;
    // ask the ID name from the lex
    int size = lex->getRuleSize();
    vmap.constSize = size-1;
    for (int i = 1; i< size; ++i) {
        vmap.InsertVt(lex->getRule(i), i);
    }
    bnfparser = new BNFParser();
    State* root = bnfparser->Analysis(filename);
    if (root == NULL) {
        printf("Error State\n");
        delete bnfparser;
        return;
    }
    // bnfparser->printTree();
    bnflist = BNF::BuildAllBNF(root,vmap);
    bnfparser->MakePrecedence(vmap);
    ExtendBNF();
    MakeID(); // for each state, make a ID for it
}

int LR_parser::Parse(Grammer_Node* root)
{
    auto& fout = DebugMsg::parser_dbg();
    // print debug message
    if (DebugMsg::isDebug()) {
        vmap.printAll();
        fout << "=========== BNF ===========" << endl;
        for (auto bnf : bnflist)
            bnf->print_bnf();
        table->printTable();
    }
    core.setLex(lex);
    core.setBnflist(&bnflist);
    core.setTable(table);
    core.setVMap(&vmap);
    core.setAst(root);
    Grammer_Node* node = core.Run();
	save_log();
    DebugMsg::parser_close();
    if (node != (void*)-1)
        return 0;
    return -1;
}

void LR_parser::setLex(LexInterface* _lex)
{
    lex = _lex;
}

void LR_parser::save_log() {
	if (DebugMsg::isDebug()) {
        {
    	    cereal::JSONOutputArchive oarchive(DebugMsg::parser_save());
    	    oarchive(
    			cereal::make_nvp("table", *((LALRTable*)table)),
    			cereal::make_nvp("vmap", vmap),
    			cereal::make_nvp("bnf", *this)
    	 	);
        } // 通过让oarchive提前析构，为文件输出流添加结尾
		DebugMsg::parser_save_close(); // 一定要确保JSONOutput输出完后再close
	}
}
