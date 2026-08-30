#include"ast.h"
#include<charconv>
#include<cmath>


//Returns the string equivalent of the Unary operators
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

//Returns the string version of the Unary operation
std::string UnaryNode::to_string() const
{
    const std::string sym = symbol();
    return '(' + (sym) +  down_->to_string()+ ')';
}


//Evaluates the result of the Unary operation
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

//Returns the string version of the identifier
std::string IdentifierNode::to_string() const
{
    if (str_ != "") {
        return str_;
    }
    throw std::runtime_error("Parser Error: Identifier not initialized.");
}

//Currently no evaluation criteria for Identifiers
double IdentifierNode::eval() const
{
    throw std::invalid_argument("Unknown identifier 'x'.");
}

//Returns the string version of the Number stored
std::string NumberNode::to_string() const
{
    char buffer[64];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), number_);
    
    if (ec == std::errc{}) {
        return std::string(buffer, ptr - buffer);
    }
    throw std::runtime_error("Parser Error: Number too long to format.");
}

//Returns the value of the number
double NumberNode::eval() const
{
    return number_;
}


//Returns the string equivalent of the Binary operators
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

//Returns the string equivalent of the Binary operations
std::string BinaryNode::to_string() const
{
    const std::string sym = symbol();
    return '(' + left_->to_string() + ' ' + (sym) + ' ' + right_->to_string()+ ')';
}

//Evaluates the binary operation
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