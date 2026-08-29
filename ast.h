#include"token.h"
#include<memory>
#include<string>
#include<stdexcept>



#ifndef AST_H
#define AST_H


struct Node {
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
    virtual double eval() const = 0;
};

struct NumberNode : Node 
{
    double number_;
    
    NumberNode(double d):number_(d) {}
    std::string to_string() const override;

    double eval() const override;
}; 

struct IdentifierNode : Node 
{
    std::string str_ = "";
    TokenKind kind_ = TokenKind::Identifier;
    
    IdentifierNode(const std::string& s):str_(s) {}
    std::string to_string() const override;

    double eval() const override;
}; 

struct BinaryNode : Node
{
    TokenKind kind_;
    std::unique_ptr<Node> left_;
    std::unique_ptr<Node> right_;

    BinaryNode(TokenKind o, std::unique_ptr<Node>&& l, std::unique_ptr<Node>&& r)
        : kind_(o), left_(std::move(l)), right_(std::move(r)) {}
    
    std::string symbol() const;

    std::string to_string() const override;

    double eval() const override;
};




struct UnaryNode : Node
{
    TokenKind kind_;
    std::unique_ptr<Node> down_;

    UnaryNode(TokenKind o, std::unique_ptr<Node>&& d)
        : kind_(o), down_(std::move(d)){}
    
    std::string symbol() const;

    std::string to_string() const override;

    double eval() const override;
};

#endif

