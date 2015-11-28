/* 
* @Author: sxf
* @Date:   2015-01-01 23:48:42
* @Last Modified by:   sxf
* @Last Modified time: 2015-11-08 20:12:20
*/

#ifndef LEX_INTERFACE_H
#define LEX_INTERFACE_H

//it's a token for return
struct Token
{
	const char* pToken;
	int type;
    int col_num;
	int row_num;
};

class LexInterface 
{
public:
	// 从输入流读取下一个字符， 无需释放资源，再下一次Read时，自动释放上个资源
    virtual Token* Read() = 0;

    // 对应每个规则，询问其名称是什么，为了在语法定义中和对应的id关联
    virtual const char* getRule(int id) = 0;
    
    // 获取总共有多少个词法规则
    virtual int getRuleSize() = 0;
};

#endif // LEX_INTERFACE_H

