#ifndef COMPILER_SYMTABLE
#define COMPILER_SYMTABLE

#include <unordered_map>
#include <string>
#include <vector>
#include "ast.h"
using std::unordered_map;
using std::string;


// modelo para símbolos
struct Symbol {
    std::string var;                        
    std::string type;                       
    int valX;
    int valY;
    
	bool isFunction = false;
    std::vector<string> paramTypes;
	std::vector<string> paramNames;
	Statement *body;
	std::vector<Expression*> arguments;
    bool funcCall = false;
	std::string ret;
};


// tabela de símbolos
class SymTable
{
private: 
   	unordered_map<string,Symbol> table;    
   	SymTable * prev;   

public:
	SymTable();
	SymTable(SymTable * t);
	
	bool Insert(string s, Symbol symb);
	Symbol * Find(string s); 
};

#endif