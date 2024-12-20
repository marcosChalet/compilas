#include "parser.h"
#include "error.h"
#include <iostream>
#include <sstream>
#include <cctype>

using std::cin;
using std::cout;
using std::endl;
using std::stringstream;
using namespace std;
using std::cout;


extern Lexer * scanner;
extern SymTable * symtable;

Statement * Parser::Program()
{

    // ------------------------------------
    // tabela de símbolos para Program
    // ------------------------------------
    symtable = new SymTable(symtable);
    // ------------------------------------

    Decls();
    return Stmts();
}

Statement * Parser::Block(string &str)
{
    // block -> { decls stmts }
    if (!Match('{'))
        throw SyntaxError(scanner->Lineno(), "\'{\' esperado");
    
    // ------------------------------------
    // nova tabela de símbolos para o bloco
    // ------------------------------------
    SymTable *saved = symtable;
    symtable = new SymTable(symtable);
    // ------------------------------------
    
    Decls();
    Statement * sts = Stmts();
    if (Match(Tag::RETURN)){
        str = lookahead->lexeme;
        Match(Tag::ID);
    };
    if (!Match('}'))
        throw SyntaxError(scanner->Lineno(), "\'}\' esperado");

    // ------------------------------------------------------
    // tabela do escopo envolvente volta a ser a tabela ativa
    // ------------------------------------------------------
    delete symtable;
    symtable = saved;
    // ------------------------------------------------------

    return sts;
}

void Parser::Decls()
{
    // decls -> decl decls
    //        | empty
    // decl  -> type id index;
    //
    // index -> [ integer ]
    //        | empty

    while (lookahead->tag == Tag::TYPE)
    {
        // captura nome do tipo
        string type{lookahead->lexeme};
        Match(Tag::TYPE);

        // captura nome do identificador
        string name{lookahead->lexeme};
        Match(Tag::ID);

        // inicializa dimensões como não preenchidas
        int valX = -1;
        int valY = -1;

        // verifica se é uma declaração de arranjo
        if (Match('['))
        {
            if (lookahead->tag != Tag::INTEGER)
            {
                stringstream ss;
                ss << "o índice ou intervalo de um arranjo deve ser de valores inteiros";
                throw SyntaxError(scanner->Lineno(), ss.str());
            }
            valX = stoi(lookahead->lexeme);
            if (!Match(Tag::INTEGER))
            {
                stringstream ss;
                ss << "o índice ou intervalor de um arranjo deve ser de valores inteiros";
                throw SyntaxError{scanner->Lineno(), ss.str()};
            }

            // verifica se há uma segunda dimensão
            if (Match(':'))
            {
                if (lookahead->tag != Tag::INTEGER) {
                    stringstream ss;
                    ss << "o índice ou intervalo de um arranjo deve ser de valores inteiros";
                    throw SyntaxError(scanner->Lineno(), ss.str());
                }

                valY = stoi(lookahead->lexeme);

                if (!Match(Tag::INTEGER))
                {
                    stringstream ss;
                    ss << "o índice ou intervalor de um arranjo deve ser de valores inteiros";
                    throw SyntaxError{scanner->Lineno(), ss.str()};
                }

                if (!Match(']'))
                {
                    stringstream ss;
                    ss << "esperado ']' no lugar de  \'" << lookahead->lexeme << "\'";
                    throw SyntaxError{scanner->Lineno(), ss.str()};
                }
            }
        }

        Symbol s{name, type, valX, valY};

        // insere variável na tabela de símbolos
        if (!symtable->Insert(name, s))
        {
            // a inserção falha quando a variável já está na tabela
            stringstream ss;
            ss << "variável \"" << name << "\" já definida";
            throw SyntaxError(scanner->Lineno(), ss.str());
        }
    }
}


Statement *Parser::Stmts()
{
    // stmts -> stmt stmts
    //        | empty

    Statement *seq = nullptr;
    
    switch (lookahead->tag)
    {
    // stmts -> stmt stmts
    case Tag::ID:
    case Tag::IF:
    case Tag::WHILE:
    case Tag::FOR:
    case Tag::DO:
    case Tag::FUNC:

    case '{':
    {
        Statement *st = Stmt();
        Statement *sts = Stmts();
        seq = new Seq(st, sts);
    }
    }

    // stmts -> empty
    return seq;
}

Statement *Parser::Stmt()
{
    // stmt  -> local = bool;
    //        | if (bool) stmt
    //        | while (bool) stmt
    //        | do stmt while (bool);
    //        | block

    Statement *stmt = nullptr;
    switch (lookahead->tag)
    {
    // stmt -> local = bool;
    case Tag::ID:
    {
        Expression *left = Local();

        if (!Match('='))
        {
            stringstream ss;
            ss << "esperado = no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Symbol *s = symtable->Find(lookahead->lexeme);
        CallParam callReturn;
        //cout<< s->paramNames.empty()<< "callreturn" << endl;
        callReturn = Call();

        if(callReturn.isFunction){
            stmt = new FuncCall(callReturn.name, callReturn.arguments, left->token->lexeme);
        }else{
            Expression *right = Bool();
            stmt = new Assign(left, right);
        }
        return stmt;
    }
    // stmt -> if (bool) stmt
    case Tag::IF:
    {
        Match(Tag::IF);
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        // criação adiantada do if para pegar erros 
        // da expressão condicional na linha correta
        stmt = new If(cond, nullptr);
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Statement *inst = Stmt();
        // modifica nó If que foi criado apenas com a expressão condicional
        ((If*)stmt)->stmt = inst;
        return stmt;
    }
    case Tag::FOR:
    {
        Match(Tag::FOR);
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        
        Decls();
        Expression *left = Local();
        if (!Match('='))
        {
            stringstream ss;
            ss << "esperado = no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *right = Ari();
        Assign *init = new Assign(left, right);

        if (!Match(';')){
            stringstream ss;
            ss << "esperado ; no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
    
        Expression *cond = Bool();

        if (!Match(';')){
            stringstream ss;
            ss << "esperado ; no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }

        Expression *left_increment = Local();
        if (!Match('='))
        {
            stringstream ss;
            ss << "esperado = no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *right_increment = Ari();
        Assign *increment = new Assign(left_increment, right_increment);

        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Statement *inst = Stmt();
        stmt = new For(init, cond, increment, inst);

        return stmt;
    }
    // stmt -> while (bool) stmt
    case Tag::WHILE:
    {
        Match(Tag::WHILE);
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Statement *inst = Stmt();
        stmt = new While(cond, inst);
        return stmt;
    }

    // stmt -> do stmt while (bool);
    case Tag::DO:
    {
        Match(Tag::DO);
        Statement *inst = Stmt();
        if (!Match(Tag::WHILE))
        {
            stringstream ss;
            ss << "esperado \'while\' no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        Expression *cond = Bool();
        stmt = new DoWhile(inst, cond);
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        return stmt;
    }
    // stmt -> func
    case Tag::FUNC:
    {
        Match(Tag::FUNC);

        if (lookahead->tag != Tag::ID)
        {
            stringstream ss;
            ss << "esperado um identificador para o nome da função, mas encontrado: \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }

        std::string funcName = lookahead->lexeme;
        string name{lookahead->lexeme};
        Match(Tag::ID);

        if (!Match('('))
        {
            stringstream ss;
            ss << "esperado ( após o nome da função, mas encontrado: \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        std::vector<string> paramTypes;
        std::vector<string> paramNames;
        if (lookahead->tag != ')') // Verificar se não está vazio
        {
            do
            {
                Decls();
            } while (Match(',')); // Permitir lista separada por vírgulas
        }

        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) para fechar a lista de parâmetros, mas encontrado: \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        if (!Match(':'))
        {
            stringstream ss;
            ss << "esperado : antes do tipo de retorno da função, mas encontrado: \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        if (lookahead->tag != Tag::TYPE)
        {
            stringstream ss;
            ss << "esperado um tipo de retorno, mas encontrado: \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        int returnType = lookahead->tag;
        string type{lookahead->lexeme};
        Match(Tag::TYPE);

        Statement *body;
        // Corpo da função (espera um bloco de instruções)
        string ret;
        body = Block(ret);

        Symbol s;
        s.isFunction = true;
        s.var = name;
        s.type = type;
        s.paramTypes = paramTypes;
        s.paramNames = paramNames;
        s.ret = ret;
        s.body = body;

        // insere variável na tabela de símbolos
        if (!symtable->Insert(name, s))
        {
            // a inserção falha quando a variável já está na tabela
            stringstream ss;
            ss << "variável \"" << name << "\" já definida";
            throw SyntaxError(scanner->Lineno(), ss.str());
        }

        stmt = new Func(funcName, returnType, paramTypes, paramNames, body, ret);
        // Criar o nó da função
        return stmt;
    }
    // stmt -> block

    case '{':
    {
        stmt = Block();
        return stmt;
    }
    default:
    {
        stringstream ss;
        ss << "\'" << lookahead->lexeme << "\' não inicia uma instrução válida";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
    }
}

CallParam Parser::Call(){
    CallParam left;

    Expression *expr = nullptr;
    string name;
    std::vector<string> args;
    bool isFunction = false;
    Symbol *s = symtable->Find(lookahead->lexeme);
    if (!s)
    {
        return left;
    }
    if (s->isFunction) { // Identificar chamada de função
        Match(Tag::ID); // Nome da função

        //Identifier *funcName = new Identifier(ExprType::VOID, lookahead);

        Match('('); // Abre parêntese
        isFunction = true;
        if (lookahead->tag != ')') { // Lista de argumentos não vazia
            do {
                // Verifica se o token é um identificador ou tipo válido
                if (lookahead->tag == Tag::ID || lookahead->tag == Tag::TYPE) {
                    args.push_back(lookahead->lexeme); // Adiciona o argumento à lista

                    Match(lookahead->tag); // Avança para o próximo token
                    
                } else {
                    // Caso o token não seja válido, lança um erro de sintaxe
                    throw SyntaxError(scanner->Lineno(), "Argumento inválido na chamada de função.");
                }

                // Se houver uma vírgula, consome-a e continua para o próximo argumento
            } while (lookahead->tag == ',' && Match(','));
        }
        Match(')'); // Fecha parêntese
        // Criar a chamada de função na AST
        name = s->var;


    }else{
        left.setIsFunction(false);
    }
    left.setName(name);
    left.setLeft(expr);
    left.setArgs(args);
    left.setIsFunction(isFunction);
    return left;
}
Expression *Parser::Local()
{
    // local -> local[bool]
    //        | id

    Expression *expr = nullptr;

    switch (lookahead->tag)
    {
    case Tag::ID:
    {
        // verifica tipo da variável na tabela de símbolos
        Symbol *s = symtable->Find(lookahead->lexeme);
        if (!s)
        {
            stringstream ss;
            ss << "variável \"" << lookahead->lexeme << "\" não declarada";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }

        // identifica o tipo da expressão
        int etype = ExprType::VOID;
        if (s->type == "int")
            etype = ExprType::INT;
        else if (s->type == "float")
            etype = ExprType::FLOAT;
        else if (s->type == "bool")
            etype = ExprType::BOOL;

        // identificador
        expr = new Identifier(etype, new Token{*lookahead});
        Match(Tag::ID);
        if (Match('[')) {
            Expression *index1 = Bool();
            if (Match(':')) {
                // Acesso a matriz bidimensional
                Expression *index2 = Bool();
                expr = new Access(etype, new Token{Tag::ID, "[:]"}, expr, index1, index2);
            } else {
                // Acesso a vetor unidimensional
                expr = new Access(etype, new Token{Tag::ID, "[]"}, expr, index1);
            }

            if (!Match(']')) {
                stringstream ss;
                ss << "esperado ] no lugar de  \'" << lookahead->lexeme << "\'";
                throw SyntaxError{scanner->Lineno(), ss.str()};
            }
        }
        break;
    }
    default:
    {
        stringstream ss;
        ss << "esperado um local de armazenamento (variável ou arranjo)";
        throw SyntaxError{scanner->Lineno(), ss.str()};
    }
    }

    return expr;
}

Expression *Parser::Bool()
{
    // bool -> join lor
    // lor  -> || join lor
    //       | empty

    Expression *expr1 = Join();

    // função Lor()
    while (true)
    {
        Token t = *lookahead;

        if (Match(Tag::OR))
        {
            Expression *expr2 = Join();
            expr1 = new Logical(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Join()
{
    // join -> equality land
    // land -> && equality land
    //       | empty

    Expression *expr1 = Equality();

    // função Land()
    while (true)
    {
        Token t = *lookahead;
        if (Match(Tag::AND))
        {
            Expression *expr2 = Equality();
            expr1 = new Logical(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Equality()
{
    // equality -> rel eqdif
    // eqdif    -> == rel eqdif
    //           | != rel eqdif
    //           | empty

    Expression *expr1 = Rel();

    // função Eqdif()
    while (true)
    {
        Token t = *lookahead;

        if (lookahead->tag == Tag::EQ)
        {
            Match(Tag::EQ);
            Expression *expr2 = Rel();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::NEQ)
        {
            Match(Tag::NEQ);
            Expression *expr2 = Rel();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Rel()
{
    // rel  -> ari comp
    // comp -> < ari comp
    //       | <= ari comp
    //       | > ari comp
    //       | >= ari comp
    //       | empty

    Expression *expr1 = Ari();

    // função Comp()
    while (true)
    {
        Token t = *lookahead;

        if (lookahead->tag == '<')
        {
            Match('<');
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::LTE)
        {
            Match(Tag::LTE);
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == '>')
        {
            Match('>');
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else if (lookahead->tag == Tag::GTE)
        {
            Match(Tag::GTE);
            Expression *expr2 = Ari();
            expr1 = new Relational(new Token{t}, expr1, expr2);
        }
        else
        {
            // empty
            break;
        }
    }

    return expr1;
}

Expression *Parser::Ari()
{
    // ari  -> term oper
    // oper -> + term oper
    //       | - term oper
    //       | empty

    Expression *expr1 = Term();

    // função Oper()
    while (true)
    {
        Token t = *lookahead;

        // oper -> + term oper
        if (lookahead->tag == '+')
        {
            Match('+');
            Expression *expr2 = Term();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // oper -> - term oper
        else if (lookahead->tag == '-')
        {
            Match('-');
            Expression *expr2 = Term();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // oper -> empty
        else
            break;
    }

    return expr1;
}

Expression *Parser::Term()
{
    // term -> unary calc
    // calc -> * unary calc
    //       | / unary calc
    //       | empty

    Expression *expr1 = Unary();

    // função Calc()
    while (true)
    {
        Token t = *lookahead;

        // calc -> * unary calc
        if (lookahead->tag == '*')
        {
            Match('*');
            Expression *expr2 = Unary();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // calc -> / unary calc
        else if (lookahead->tag == '/')
        {
            Match('/');
            Expression *expr2 = Unary();
            expr1 = new Arithmetic(expr1->type, new Token{t}, expr1, expr2);
        }
        // calc -> empty
        else
            break;
    }

    return expr1;
}

Expression *Parser::Unary()
{
    // unary -> !unary
    //        | -unary
    //        | factor

    Expression *unary = nullptr;

    // unary -> !unary
    if (lookahead->tag == '!')
    {
        Token t = *lookahead;
        Match('!');
        Expression *expr = Unary();
        unary = new UnaryExpr(ExprType::BOOL, new Token{t}, expr);
    }
    // unary -> -unary
    else if (lookahead->tag == '-')
    {
        Token t = *lookahead;
        Match('-');
        Expression *expr = Unary();
        unary = new UnaryExpr(expr->type, new Token{t}, expr);
    }
    else
    {
        unary = Factor();
    }

    return unary;
}

Expression *Parser::Factor()
{
    // factor -> (bool)
    //         | local
    //         | integer
    //         | floating
    //         | true
    //         | false

    Expression *expr = nullptr;

    switch (lookahead->tag)
    {
    // factor -> (bool)
    case '(':
    {
        Match('(');
        expr = Bool();
        if (!Match(')'))
        {
            stringstream ss;
            ss << "esperado ) no lugar de  \'" << lookahead->lexeme << "\'";
            throw SyntaxError{scanner->Lineno(), ss.str()};
        }
        break;
    }

    // factor -> local
    case Tag::ID:
    {
        expr = Local();
        break;
    }

    // factor -> integer
    case Tag::INTEGER:
    {
        expr = new Constant(ExprType::INT, new Token{*lookahead});
        Match(Tag::INTEGER);
        break;
    }

    // factor -> floating
    case Tag::FLOATING:
    {
        expr = new Constant(ExprType::FLOAT, new Token{*lookahead});
        Match(Tag::FLOATING);
        break;
    }

    // factor -> true
    case Tag::TRUE:
    {
        expr = new Constant(ExprType::BOOL, new Token{*lookahead});
        Match(Tag::TRUE);
        break;
    }

    // factor -> false
    case Tag::FALSE:
    {
        expr = new Constant(ExprType::BOOL, new Token{*lookahead});
        Match(Tag::FALSE);
        break;
    }

    default:
    {
        stringstream ss;
        ss << "uma expressão é esperada no lugar de  \'" << lookahead->lexeme << "\'";
        throw SyntaxError{scanner->Lineno(), ss.str()};
        break;
    }
    }

    return expr;
}

bool Parser::Match(int tag)
{   
    
    if (tag == lookahead->tag)
    {
        lookahead = scanner->Scan();
        return true;
    }

    return false;
}

Parser::Parser()
{
    lookahead = scanner->Scan();
    symtable = nullptr;
}

Statement * Parser::Start()
{
    return Program();
}

