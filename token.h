#include<variant>
#include<string>
#include<cstddef>

#ifndef TOKEN_H
#define TOKEN_H


//1. Kind of tokens that the lexer is able to process
enum class TokenKind {
    Number, Identifier, Comma,
    Plus, Minus, Star, Slash, Percent, Caret,
    LParen, RParen, Assign,
    Equal, Greater, Less, GreaterEqual, LessEqual, NotEqual,
    End
};  



//2. A token, that includes a type of token (See 1. struct TokenKind), a value (either string or double), and a position
struct Token {
    TokenKind   kind;
    std::variant<double, std::string> value;
    size_t      pos;

    //Construction without value e.g operators
    Token(TokenKind tkn, size_t p):kind(tkn),pos(p) {}  

    //Construction with a value of double e.g numeric literals
    Token(TokenKind tkn, double d, size_t p):kind(tkn), pos(p) {value = d;}
    
    //Construction with a value of std::string e.g identifiers
    Token(TokenKind tkn, std::string s, size_t p):kind(tkn), pos(p) {value = s;}
};  
#endif
