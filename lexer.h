#include"token.h"
#include<string>
#include<vector>


#ifndef LEXER_H
#define LEXER_H

//Use of namespace for the purpose of encapsulation
namespace calc 
{
    //The main lexer function
    std::vector<Token> lex(const std::string& a);
}

#endif
