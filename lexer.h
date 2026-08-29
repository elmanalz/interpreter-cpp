#include"token.h"
#include<string>
#include<vector>


#ifndef LEXER_H
#define LEXER_H


namespace calc 
{
    std::vector<Token> lex(const std::string& a);
}

#endif
