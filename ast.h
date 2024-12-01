
#ifndef COMPILER_AST
#define COMPILER_AST
#include <vector>
#include "lexer.h"

enum NodeType
{
    UNKNOWN,
    STMT,
    EXPR,
    CONSTANT,
    IDENTIFIER,
    ACCESS,
    LOG,
    REL,
    ARI,
    UNARY,
    SEQ,
    ASSIGN,
    IF_STMT,
    WHILE_STMT,
    DOWHILE_STMT,
    FOR_STMT,
    FUNC_STMT,
    TEMP,
    FUNC_CALL
};

enum ExprType
{
    VOID,
    INT,
    FLOAT,
    BOOL
};

struct Node
{
    int node_type;
    static unsigned labels;

    Node();
    Node(int t);
    virtual string ToString();
    unsigned NewLabel();
};

struct Statement : public Node
{
    Statement();
    Statement(int type);
    virtual void Gen();
};

struct Expression : public Node
{
    int type;
    Token * token;
        
    Expression(Token *t);
    Expression(int ntype, int etype, Token *t);
    virtual string ToString();
    string Type();
};

struct Temp : public Expression
{
    static int count;
    int number;
    Temp(int etype);
    string ToString();
};

struct Constant : public Expression
{
    Constant(int etype, Token *t);
};

struct Identifier : public Expression
{
    Identifier(int etype, Token *t);
};

struct Access : public Expression
{
    Expression * id;
    Expression * indexX;
    Expression * indexY;
    Access(int etype, Token * t, Expression * i, Expression * e);
    Access(int etype, Token *t, Expression *i, Expression *e1, Expression *e2);
    string ToString();
};

struct Logical : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Logical(Token *t, Expression *e1, Expression *e2);
};

struct Relational : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Relational(Token *t, Expression *e1, Expression *e2);
};

struct Arithmetic : public Expression
{
    Expression *expr1;
    Expression *expr2;
    Arithmetic(int etype, Token *t, Expression *e1, Expression *e2);
};

struct UnaryExpr : public Expression
{
    Expression *expr;
    UnaryExpr(int etype, Token *t, Expression *e);
};

struct Seq : public Statement
{
    Statement *stmt;
    Statement *stmts;
    Seq(Statement *s, Statement *ss);
    void Gen();
};

struct Assign : public Statement
{
    Expression *id;
    Expression *expr;
    Assign(Expression *i, Expression *e);
    void Gen();
};

struct If : public Statement
{
    unsigned after;
    Expression *expr;
    Statement *stmt;
    If(Expression *e, Statement *s);
    void Gen();
};

struct While : public Statement
{
    unsigned before;
    unsigned after;
    Expression *expr;
    Statement *stmt;
    While(Expression *e, Statement *s);
    void Gen();
};

struct For : public Statement
{
    unsigned before;
    unsigned after;
    Assign *for_init;
    Expression *for_condition;
    Assign *for_increment;
    Statement *stmt;
    For(Assign *init, Expression *condition, Assign *increment, Statement *s);
    void Gen();
};

struct DoWhile : public Statement
{
    unsigned before;
    Statement *stmt;
    Expression *expr;
    DoWhile(Statement *s, Expression *e);
    void Gen();
};

struct Func : public Statement
{
    int after;
    string funcName;
    int returnType;
    string ret;

    std::vector<string> paramTypes;
    std::vector<string> paramNames;

    Statement *body;
    Statement *stmt;
    Expression *expr;
    Func(std::string funcName, int returnType, std::vector<string> paramTypes, std::vector<string> paramNames, Statement *body, string ret);
    void Gen();
};
struct FuncCall : public Statement {
    string function;               // Nome da função (identificador)
    std::vector<string> args;      // Lista de argumentos
    std::string functionName;
    bool isFunction = false;
    std::string ret;


    FuncCall(string function, std::vector<string> arguments, std::string ret);

    void Gen();
};
struct CallParam {
    std::vector<string> arguments; // Argumentos da função
    bool isFunction;                    // Indica se é uma função
    std::string name;                   // Nome da variável ou função
    Expression* left;                   // Lado esquerdo da expressão (opcional)

    // Construtor com inicializações
    CallParam(std::vector<string> args = {}, 
              bool isFunc = false, 
              std::string funcName = "test", 
              Expression* lhs = nullptr)
        : arguments(std::move(args)), isFunction(isFunc), name(std::move(funcName)), left(lhs) {}

    // Métodos 'set' para cada variável

    void setIsFunction(bool newIsFunction) {
        isFunction = newIsFunction;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    void setLeft(Expression* newLeft) {
        left = newLeft;
    }
    void setArgs(const std::vector<std::string>& newArgs) {
        // Liberar memória dos argumentos antigos
        arguments.clear();

        // Copiar os novos argumentos
        for (const auto& arg : newArgs) {
            arguments.push_back(arg); // Copiar o conteúdo de cada string
        }
    }

    // Construtor de cópia
    CallParam(const CallParam& other)
        : arguments(other.arguments), 
          isFunction(other.isFunction), 
          name(other.name),
          left(other.left ? new Expression(*other.left) : nullptr) {}

    // Operador de atribuição
    CallParam& operator=(const CallParam& other) {
        if (this == &other) return *this; // Evitar auto-atribuição

        arguments = other.arguments;
        isFunction = other.isFunction;
        name = other.name;

        // Gerenciar o ponteiro
        delete left;
        left = other.left ? new Expression(*other.left) : nullptr;

        return *this;
    }

    // Construtor de movimentação
    CallParam(CallParam&& other) noexcept
        : arguments(std::move(other.arguments)),
          isFunction(other.isFunction),
          name(std::move(other.name)),
          left(other.left) {
        other.left = nullptr; // Garantir que o recurso original não será destruído
    }

    // Operador de movimentação
    CallParam& operator=(CallParam&& other) noexcept {
        if (this == &other) return *this;

        arguments = std::move(other.arguments);
        isFunction = other.isFunction;
        name = std::move(other.name);
        
        // Gerenciar o ponteiro
        delete left;
        left = other.left;
        other.left = nullptr;

        return *this;
    }
};
#endif