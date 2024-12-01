#include <iostream>
#include <sstream>
#include "ast.h"
#include "error.h"
#include "gen.h"
using std::cout;
using std::endl;
using std::stringstream;

extern Lexer *scanner;

// ----
// Node
// ----

unsigned Node::labels = 0;

Node::Node() : 
    node_type(NodeType::UNKNOWN) 
{

}

Node::Node(int t) : 
    node_type(t) 
{

}

string Node::ToString() 
{ 
    return ""; 
}

unsigned Node::NewLabel() 
{ 
    return ++labels; 
}

// ---------
// Statement
// ---------

Statement::Statement() : 
    Node(NodeType::STMT) 
{

}

Statement::Statement(int type) : 
    Node(type) 
{

}

void Statement::Gen() 
{
    
}

// ----------
// Expression
// ----------

Expression::Expression(Token *t) : 
    Node(NodeType::EXPR),
    type(ExprType::VOID),
    token(t)
{

}

Expression::Expression(int ntype, int etype, Token *t) : 
    Node(ntype), 
    type(etype), 
    token(t) 
{

}

string Expression::ToString()
{
    return token->lexeme;
}

string Expression::Type()
{
    switch (type)
    {
    case ExprType::INT:
        return "int";
        break;
    case ExprType::FLOAT:
        return "float";
        break;
    case ExprType::BOOL:
        return "bool";
        break;
    default:
        return "void";
    }
}

// ----
// Temp
// ----

int Temp::count = 0;

Temp::Temp(int etype) : 
    Expression(NodeType::TEMP, etype, nullptr), 
    number(++count)
{
}

string Temp::ToString()
{
    stringstream ss;
    ss << "t" << number;
    return ss.str();
}

// --------
// Constant
// --------

Constant::Constant(int etype, Token *t) : 
    Expression(NodeType::CONSTANT, etype, t) 
{

}

// ----------
// Identifier
// ----------

Identifier::Identifier(int etype, Token *t) : 
    Expression(NodeType::IDENTIFIER, etype, t) 
{

}

// ------
// Access
// ------

Access::Access(int etype, Token *t, Expression *i, Expression *e) : 
    Expression(NodeType::ACCESS, etype, t), 
    id(i), 
    indexX(e) 
{

}

Access::Access(int etype, Token *t, Expression *i, Expression *e1, Expression *e2)
    : Expression(NodeType::ACCESS, etype, t), id(i), indexX(e1), indexY(e2)
{

}


// string Access::ToString()
// {
//     stringstream ss;
//     ss << id->ToString() << "[" << expr->ToString() << "]";
//     return ss.str();
// }

string Access::ToString() {
    if (indexY)
        return id->ToString() + "[" + indexX->ToString() + ":"+ indexY->ToString() + "]";
    else
        return id->ToString() + "[" + indexX->ToString() + "]";
}

// -------
// Logical
// -------

Logical::Logical(Token *t, Expression *e1, Expression *e2) : 
    Expression(NodeType::LOG, ExprType::BOOL, t), 
    expr1(e1), 
    expr2(e2)
{
    // verificação de tipos
    if (expr1->type != ExprType::BOOL || expr2->type != ExprType::BOOL)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado com operandos não booleanos ("
           << expr1->ToString() << ":" << expr1->Type() << ") ("
           << expr2->ToString() << ":" << expr2->Type() << ") ";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ----------
// Relational
// ----------

Relational::Relational(Token *t, Expression *e1, Expression *e2) : 
    Expression(NodeType::REL, ExprType::BOOL, t), 
    expr1(e1), 
    expr2(e2)
{
    // verificação de tipos
    if (expr1->type != expr2->type)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado com operandos de tipos diferentes ("
           << expr1->ToString() << ":" << expr1->Type() << ") ("
           << expr2->ToString() << ":" << expr2->Type() << ") ";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ----------
// Arithmetic
// ----------

Arithmetic::Arithmetic(int etype, Token *t, Expression *e1, Expression *e2) : 
    Expression(NodeType::ARI, etype, t), 
    expr1(e1), 
    expr2(e2)
{
    // verificação de tipos
    if (expr1->type != expr2->type)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado com operandos de tipos diferentes ("
           << expr1->ToString() << ":" << expr1->Type() << ") ("
           << expr2->ToString() << ":" << expr2->Type() << ") ";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ---------
// UnaryExpr
// ---------

UnaryExpr::UnaryExpr(int etype, Token *t, Expression *e) : 
    Expression(NodeType::UNARY, etype, t), 
    expr(e)
{
    // verificação de tipos
    if (t->tag == '!' && expr->type != ExprType::BOOL)
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado com operando não booleano ("
           << expr->ToString() << ":" << expr->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }

    if (t->tag == '-' && (expr->type != ExprType::INT && expr->type != ExprType::FLOAT))
    {
        stringstream ss;
        ss << "\'" << token->lexeme << "\' usado com operando não numérico ("
           << expr->ToString() << ":" << expr->Type() << ")";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

// ----
// Seq
// ----

Seq::Seq(Statement *s, Statement *ss) : 
    Statement(NodeType::SEQ), 
    stmt(s), 
    stmts(ss) 
{

}

void Seq::Gen()
{
    stmt->Gen();

    if (stmts)
        stmts->Gen();
}

// ------
// Assign
// ------

Assign::Assign(Expression *i, Expression *e) : 
    Statement(NodeType::ASSIGN), 
    id(i), 
    expr(e)
{
    // verificação de tipos
    if (id->type != expr->type)
    {
        stringstream ss;
        ss << "\'=\' usado com operandos de tipos diferentes ("
           << id->ToString() << ":" << id->Type() << ") ("
           << expr->ToString() << ":" << expr->Type() << ") ";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
}

void Assign::Gen()
{ 
    Expression * left = Lvalue(id);
    Expression * right = Rvalue(expr);
    cout << '\t' << left->ToString() << " = " << right->ToString() << endl;
}

// ----
// If
// ----

If::If(Expression *e, Statement *s) : 
    Statement(NodeType::IF_STMT), 
    expr(e), 
    stmt(s)
{
    // verificação de tipos
    if (expr->type != ExprType::BOOL)
    {
        stringstream ss;
        ss << "expressão condicional \'" << expr->ToString() << "\' não booleana";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }

    // cria novo rótulo
    after = NewLabel();
}

void If::Gen()
{
    Expression * n = Rvalue(expr);
    cout << "\tifFalse " << n->ToString() << " goto L" << after << endl;
    stmt->Gen();
    cout << 'L' << after << ':' << endl;
}

// -----
// While
// -----

While::While(Expression *e, Statement *s) : 
    Statement(NodeType::WHILE_STMT), 
    expr(e), 
    stmt(s) 
{
    before = NewLabel();
    after = NewLabel();
}

void While::Gen()
{
    cout << 'L' << before << ':' << endl;
    Expression * n = Rvalue(expr);
    cout << "\tifFalse " << n->ToString() << " goto L" << after << endl;
    stmt->Gen();
    cout << "\tgoto L" << before << endl;
    cout << 'L' << after << ':' << endl;     
}

// --------
// Do-While
// --------

DoWhile::DoWhile(Statement *s, Expression *e) : 
    Statement(NodeType::DOWHILE_STMT), 
    stmt(s), 
    expr(e) 
{
    // cria novo rótulo
    before = NewLabel();
}

void DoWhile::Gen()
{
    cout << 'L' << before << ':' << endl;
    stmt->Gen();
    Expression * n = Rvalue(expr);
    cout << "\tifTrue " << n->ToString() << " goto L" << before << endl;
}

// --------
// FOR
// --------

For::For(Assign *init, Expression *condition, Assign *increment, Statement *s) : 
    Statement(NodeType::FOR_STMT),
    for_init(init),
    for_condition(condition),
    for_increment(increment),
    stmt(s)
{
    before = NewLabel();
    after = NewLabel();
}

void For::Gen(){

    for_init->Gen();

    cout << 'L' << before << ':' << endl;
    Expression * n = Rvalue(for_condition);
    cout << "\tifFalse " << n->ToString() << " goto L" << after << endl;
    stmt->Gen();
    Expression *inc_reg = Rvalue(for_increment->expr);  //Registrador que guarda o incremento
    cout << "\t" << for_increment->id->ToString() << " = " <<  inc_reg->ToString() << endl;
    cout << "\tgoto L" << before << endl;
    cout << 'L' << after << ':' << endl;     
}

// --------
// Func
// --------
Func::Func(std::string funcName, int returnType, std::vector<string> paramTypes, std::vector<string> paramNames, Statement *body, string ret) : 
    Statement(NodeType::FUNC_STMT),
    stmt(body),
    funcName(funcName), 
    returnType(returnType), 
    paramTypes(paramTypes), 
    paramNames(paramNames), 
    body(body), 
    ret(ret)
{
    after = NewLabel();
}
void Func::Gen(){
    std::cout << funcName << ":" << endl;
        // Declaração dos parâmetros
    // Corpo da função
    stmt->Gen();
    cout << "\t" << "return" << " " << ret << endl;
    cout << "\t" << endl;
}
//, std::vector<string> arguments
FuncCall::FuncCall(string function, std::vector<string> arguments, std::string ret)
    : Statement(NodeType::FUNC_STMT),
      function(function),
      args(arguments),
      ret(ret)
    //args(std::move(arguments))
{
}

void FuncCall::Gen(){
        // Gera código intermediário para a chamada da função
    for (size_t i = 0; i < args.size(); ++i) {
        cout << "\t" << "param " << args[i] << std::endl;
    }
    cout << "\t"<< ret << " = call " << function<< std::endl;
}