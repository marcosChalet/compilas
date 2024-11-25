#ifndef COMPILER_PARSER
#define COMPILER_PARSER

#include "lexer.h"
#include "symtable.h"
#include "ast.h"

class Parser
{
private:
	Token * lookahead;
	
	Statement * Program();
	Statement * Block(string &str = *(new string("op")));
	void Decls();
	void Decl();
	Statement * Stmts();
	Statement * Stmt();
	Expression * Local();
	Expression * Bool();
	Expression * Join();
	Expression * Equality();
	Expression * Rel();
	Expression * Ari();
	Expression * Term();
	Expression * Unary();
	Expression * Factor();
	bool Match(int tag);

public:
	Parser();
	Statement * Start();
	static int LineNo();
};

#endif
