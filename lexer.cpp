#include "lexer.h"
#include <fstream>
#include <sstream>
using std::stringstream;

extern std::ifstream fin;

// construtor 
Lexer::Lexer()
{
	// insere palavras-reservadas na tabela
	token_table["👑"]   = Token{ Tag::MAIN,     "main" };
	token_table["🔢"]    = Token{ Tag::TYPE,      "int" };
	token_table["🌊"]  = Token{ Tag::TYPE,    "float" };
	token_table["🧐"]   = Token{ Tag::TYPE,     "bool" };
	token_table["👍"]   = Token{ Tag::TRUE,     "true" };
	token_table["👎"]  = Token{ Tag::FALSE,   "false" };
	token_table["🤔"]     = Token{ Tag::IF,         "if" };
	token_table["🔁"]  = Token{ Tag::WHILE,   "while" };
	token_table["👇"]     = Token{ Tag::DO,         "do" };
	token_table["🧬"]	  = Token{ Tag::FOR,       "for" };
	token_table["👻"]	  = Token{ Tag::FUNC,     "func" };
	token_table["🦋"] = Token{ Tag::RETURN, "return" };

	
	// inicia leitura da entrada
	peek = fin.get();
}\

// retorna número da linha atual
int Lexer::Lineno()
{
	return line;
}

// retorna tokens para o analisador sintático
Token * Lexer::Scan()
{
	// ignora espaços em branco, tabulações e novas linhas
	while (isspace(peek))
	{
		if (peek == '\n')
			line += 1;
		peek = fin.get();
	}

	// ignora comentários
	while (peek == '/')
	{
		peek = fin.get();
		if (peek == '/')
		{
			// ignora caracteres até o fim da linha
			do
				peek = fin.get();
			while(peek != '\n');
			line += 1;
			peek = fin.get();
		}
		else if (peek == '*')
		{
			// ignora caracteres até achar */ ou EOF				
			while( (peek=fin.get()) != '*' ||  (peek=fin.get()) != '/' )
			{
				if (peek == '\n')
				{
					line += 1;
				}
				else if (peek == EOF)
				{
					token = Token {EOF};
					return &token;
				}
			}
			peek = fin.get();	
		}
		else
		{
			// barra encontrada não inicia um comentário
			fin.unget();
			peek = '/';
			break;
		}

		// remove espaços em branco, tabulações e novas linhas
		while (isspace(peek))
		{
			if (peek == '\n')
				line += 1;
			peek = fin.get();
		}
	}

	// retorna números
	if (isdigit(peek))
	{
		// ponto-flutuante não foi encontrado
		bool dot = false;
		
		// acumula dígitos do número
		stringstream ss;
		do 
		{
			if (peek == '.')
			{
				if (dot == false)
				{
					// primeiro ponto encontrado
					dot = true;
				}
				else
				{
					// segundo ponto encontrado
					break; 
				}
			}

			ss << peek;
			peek = fin.get();
		} 
		while (isdigit(peek) || peek == '.');

		// se o número é um ponto-flutuante
		if (dot)
		{
			token = Token{Tag::FLOATING, ss.str()};
			return &token;
		}
		else
		{
			token = Token{Tag::INTEGER, ss.str()};
			return &token;
		}
	}

	// retorna palavras-chave e identificadores
	if (isalpha(peek))
	{
		stringstream ss;
		do 
		{
			ss << peek;
			peek = fin.get();
		} 
		while (isalpha(peek));
		string s = ss.str();
		auto pos = token_table.find(s);

		// se o lexema já está na tabela
		if (pos != token_table.end())
		{
			// retorna o token da tabela
			token = pos->second;
			return &token;
		}

		// se o lexema ainda não está na tabela
		Token t {Tag::ID, s};
		token_table[s] = t;

		// retorna o token ID
		token = t;
		return &token;
	}

	// retorna operadores com mais de um caractere: >=, <=, == e !=
	switch(peek)
	{
		case '&':
		{
			char next = fin.get();
			if (next == '&')
			{
				peek = fin.get();
				token = Token{Tag::AND, "&&"};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '|':
		{
			char next = fin.get();
			if (next == '|')
			{
				peek = fin.get();
				token = Token{Tag::OR, "||"};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '>':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::GTE, ">="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '<':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::LTE, "<="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '=':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::EQ, "=="};
				return &token;
			}
			else
			{
				fin.unget();
			}
		}
		break;

		case '!':
		{
			char next = fin.get();
			if (next == '=')
			{
				peek = fin.get();
				token = Token{Tag::NEQ, "!="};
				return &token;	
			}
			else
			{
				fin.unget();
			}
		}
		break;
	}

	if ( utf8CharLength(peek) == 4 ){
		stringstream emojiStream;
		
		for (int i = 0; i < 4; i++){
			emojiStream << peek;
			peek = fin.get();
		}

		string emoji = emojiStream.str();
		emojiStream.clear();
		auto pos = token_table.find(emoji);

		if (pos != token_table.end())
		{
			//std::cout << "Emoji reservado '" << emoji << "' encontrado" << std::endl;
			token = pos->second;
			return &token;
		}

		return NULL;
	}

	// retorna caracteres não alphanuméricos isolados: (, ), +, -, etc.
	token = Token{peek};
	peek = ' ';
	return &token;
}

int Lexer::utf8CharLength(unsigned char c) {

    if ((c & 0x80) == 0) return 1;         // ASCII (1 byte)
    else if ((c & 0xE0) == 0xC0) return 2; 
    else if ((c & 0xF0) == 0xE0) return 3; 
    else if ((c & 0xF8) == 0xF0) return 4; // 4-byte UTF-8 (emojis, etc.)
    return 1; 
}