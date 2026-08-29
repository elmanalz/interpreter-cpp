#include"ast.h"
#include<vector>

#ifndef PARSER_H
#define PARSER_H

class Parser {
    private:
        size_t pos_ = 0;
        const std::vector<Token> tokens_;
        const Token& expect(TokenKind, const std::string&);
        const Token& peek() {return tokens_.at(pos_);}
        const Token& advance() {return tokens_.at(pos_++);}
        std::unique_ptr<Node> parse_expression();
        std::unique_ptr<Node> parse_term();
        std::unique_ptr<Node> parse_factor();
        std::unique_ptr<Node> parse_primary();
        std::unique_ptr<Node> parse_unary();
        bool match(TokenKind kind);
    public:
        Parser(const std::vector<Token> &tokens):tokens_(std::move(tokens)){}
        std::unique_ptr<Node> parse(); 
};

#endif
