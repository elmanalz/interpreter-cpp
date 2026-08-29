#include<variant>
#include<string>
#include<cstddef>

#ifndef TOKEN_H
#define TOKEN_H

enum class TokenKind {
    Number, Identifier, Comma,
    Plus, Minus, Star, Slash, Percent, Caret,
    LParen, RParen, Assign,
    Equal, Greater, Less, GreaterEqual, LessEqual, NotEqual,
    End
};


struct Token {
    TokenKind   kind;
    std::variant<double, std::string> value;
    size_t      pos;

    Token(TokenKind tkn, size_t p):kind(tkn),pos(p) {}
    Token(TokenKind tkn, double d, size_t p):kind(tkn), pos(p) {value = d;}
    Token(TokenKind tkn, std::string s, size_t p):kind(tkn), pos(p) {value = s;}
};

#endif
