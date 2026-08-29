#include"ast.h"
#include<charconv>
#include<cmath>


std::string UnaryNode::symbol() const
{
    switch (kind_)
    {
    case TokenKind::Minus:
        return "-";
    case TokenKind::Plus:
        return "+";   
    default:
        break;
    };
    throw std::invalid_argument("Evaluation Error: Unsupported Operator.");
}

std::string UnaryNode::to_string() const
{
    const std::string sym = symbol();
    return '(' + (sym) +  down_->to_string()+ ')';
}

double UnaryNode::eval() const
{
    switch (kind_)
    {
    case TokenKind::Minus:
        return (- down_->eval());
    case TokenKind::Plus:
        return (down_->eval());   
    default:
        break;
    };
    throw std::invalid_argument("Evaluation Error: Unsupported Operator.");
}


std::string IdentifierNode::to_string() const
{
    if (str_ != "") {
        return str_;
    }
    throw std::runtime_error("Parser Error: Identifier not initialized.");
}

double IdentifierNode::eval() const
{
    throw std::invalid_argument("Unknown identifier 'x'.");
}


std::string NumberNode::to_string() const
{
    char buffer[64];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), number_);
    
    if (ec == std::errc{}) {
        return std::string(buffer, ptr - buffer);
    }
    throw std::runtime_error("Parser Error: Number too long to format.");
}

double NumberNode::eval() const
{
    return number_;
}



std::string BinaryNode::symbol() const 
{
    switch (kind_)
    {
    case TokenKind::Minus:
        return "-";
    case TokenKind::Plus:
        return "+";
    case TokenKind::Star:
        return "*";
    case TokenKind::Slash:
        return "/";
    case TokenKind::Percent:
        return "%";
    case TokenKind::Caret :
        return "^";        
    default:
        break;
    };
    throw std::invalid_argument("Evaluation Error: Unsupported Operator.");
}

std::string BinaryNode::to_string() const
{
    const std::string sym = symbol();
    return '(' + left_->to_string() + ' ' + (sym) + ' ' + right_->to_string()+ ')';
}

double BinaryNode::eval() const
{
    switch (kind_)
    {
    case TokenKind::Minus:
        return (left_->eval() - right_->eval());
    case TokenKind::Plus:
        return (left_->eval() + right_->eval());
    case TokenKind::Star:
        return (left_->eval() * right_->eval());
    case TokenKind::Slash:
        if(right_->eval() == 0)
            throw std::invalid_argument("Evaluation Error: Division by zero not allowed!");
        return (left_->eval() / right_->eval());
    case TokenKind::Percent:
        if(right_->eval() == 0)
            throw std::invalid_argument("Evaluation Error: Division by zero not allowed!");
        return (std::fmod(left_->eval(), right_->eval()));
    case TokenKind::Caret :
        return (std::pow(left_->eval(), right_->eval()));        
    default:
        break;
    };
    throw std::invalid_argument("Evaluation Error: Unsupported Operator.");
}