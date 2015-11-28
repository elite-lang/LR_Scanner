/* 
* @Author: sxf
* @Date:   2015-04-17 10:30:02
* @Last Modified by:   sxf
* @Last Modified time: 2015-11-28 10:29:02
*/

#include "BNFParser.h"
#include "parser.hpp"
#include <stdio.h>
extern FILE* yyin;
State* root = NULL;
BNFParser* bnfparser = NULL;

State* BNFParser::Analysis(const char* filename) {
	bnfparser = this;
	/* open the file and change the yyin stream. */
	FILE* file_in;
	if ((file_in=fopen(filename,"r"))==NULL) {
		printf("error on open %s file!",filename);
		getchar();
		return NULL;
	}
	yyin = file_in;
	yyparse();
	state_root = root;
	/* you should close the file. */
	fclose(file_in);
	return root;
}

void BNFParser::NowLeft() {
	++now_precedence;
	now_associativity = true;
}

void BNFParser::NowRight() {
	++now_precedence;
	now_associativity = false;
}

void BNFParser::AddToken(const char* token) {
	precedence_map.insert(make_pair(token, now_precedence));
	associativity_map.insert(make_pair(token, now_associativity));
}



void BNFParser::printNode(State* s,int d)
{
    if (s == NULL) return;
    
    for (int i = 0; i<d; ++i)
		printf("    ");
    if (s->state_type == statement || s->state_type == terminal)
        printf("%s %s",s->state_class,s->state_var);
    if (s->state_type == constant)
        printf("%s",s->state_const);
    if (s->state_type == temporality)
		printf("temp node");
    if (!s->isList)
		printf(" (not list)");
    if (s->Repeatable >0 )
    {
		if (s->Repeatable == 1) printf(" ?");
		if (s->Repeatable == 2) printf(" +");
		if (s->Repeatable == 3) printf(" *");
    }
   
	
    printf("\n");
    printNode(s->children,d+1);
    printNode(s->brother,d);
}



void BNFParser::printTree()
{
    printNode(state_root,0);
}