#include"ast.h"
#include<vector>

#ifndef PARSER_H
#define PARSER_H

//Parser class for parsing the tokens created by the lexer
class Parser {
    private:
        size_t pos_ = 0;    //Position of the last parsed token
        const std::vector<Token> tokens_;   //Tokens that need to be parsed
        const Token& expect(TokenKind, const std::string&); //expects a certain tokenkind at the current position and returns it
                                                            //if not present, throws

        const Token& peek() {return tokens_.at(pos_);}      //Returns the token at the current position
        const Token& advance() {return tokens_.at(pos_++);} //Returns the token at the current position and increments the position by 1
        bool match(TokenKind kind);                         //Compares the token at the current position, 
                                                            //if they match, adnvances and returns true. Otherwise, returns false

        std::unique_ptr<Node> parse_expression();   //First layer in parsing, looks for Binary + and - and consumes them
        std::unique_ptr<Node> parse_term();         //Second layer in parsing, looks for *, /, and % operatoers and consumes them
        std::unique_ptr<Node> parse_factor();       //Fourth layer in parsing, looks for ^ and consumes them 
        std::unique_ptr<Node> parse_primary();      //Fifth and last layer (highest precendence) in parsing
                                                    //Looks for expression inside parentheses, numbers, and identifiers and consumes them
        std::unique_ptr<Node> parse_unary();        //Third layer in parsing, looks for Unary - and + and consumes them
    public:
        //Default constructor, takes the tokens and moves them inside the class
        Parser(const std::vector<Token> &tokens):tokens_(std::move(tokens)){}

        //Main Parsing Algorithm
        std::unique_ptr<Node> parse(); 
};

#endif
