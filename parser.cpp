#include"parser.h"
#include<cassert>

std::unique_ptr<Node> Parser::parse_primary()
{
    size_t entry_pos = pos_;
    std::unique_ptr<Node> ptr;
    if (match(TokenKind::LParen))
    {
        ptr = parse_expression();
        expect(TokenKind::RParen, "Closing Parenthesis");
    }
    else if (peek().kind == TokenKind::Identifier)
    {
        ptr = std::make_unique<IdentifierNode>(std::get<std::string>(expect(TokenKind::Identifier, "Identifier").value));
    }
    else
    {
        ptr = std::make_unique<NumberNode>(std::get<double>(expect(TokenKind::Number, "Number, parenthese, or identifier").value));
    }

    assert((pos_ > entry_pos) && "Parsing failed at parse primary.");
    return ptr;
}

std::unique_ptr<Node> Parser::parse_unary()
{
    size_t entry_pos = pos_;
    std::unique_ptr<Node> ptr;
    if(peek().kind == (TokenKind::Minus) || peek().kind == (TokenKind::Plus))
    {
        TokenKind t = advance().kind;
        ptr = std::make_unique<UnaryNode>(t, parse_unary());
    }
    else
    {
        return parse_factor();
    }
    assert((pos_ > entry_pos) && "Parsing failed at parse unary.");
    return ptr;
}


std::unique_ptr<Node> Parser::parse_factor()
{
    size_t entry_pos = pos_;
    std::unique_ptr<Node> left = parse_primary();
    while (peek().kind == (TokenKind::Caret))
    {
        TokenKind t = advance().kind;
        std::unique_ptr<Node> right = parse_unary();
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
    }
    assert((pos_ > entry_pos) && "Parsing failed at parse factor.");
    return left;
}


std::unique_ptr<Node> Parser::parse_term()
{
    size_t entry_pos = pos_;
    std::unique_ptr<Node> left = parse_unary();

    while (peek().kind == TokenKind::Star || peek().kind == TokenKind::Slash || peek().kind == TokenKind::Percent)
    {
        TokenKind t = advance().kind;
        std::unique_ptr<Node> right = parse_unary();
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
    }
    assert((pos_ > entry_pos) && "Parsing failed at parse term.");
    return left;
}

std::unique_ptr<Node> Parser::parse_expression()
{
    size_t entry_pos = pos_;
    std::unique_ptr<Node> left = parse_term();
    while (peek().kind == (TokenKind::Plus) || peek().kind == (TokenKind::Minus))
    {
        TokenKind t = advance().kind;
        std::unique_ptr<Node> right = parse_term();
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
    }
    assert((pos_ > entry_pos) && "Parsing failed at parse expression.");
    return left;
}



const Token& Parser::expect(TokenKind kind, const std::string& what)
{
    if (peek().kind != kind)
        throw std::invalid_argument("Parser Error: Expected "+ what + " at " + std::to_string(peek().pos) + '.');
    return advance();
    
}

bool Parser::match(TokenKind kind)
{
    if(peek().kind == kind)
    {
        advance();
        return true;
    }
    return false;
}

std::unique_ptr<Node> Parser::parse() 
{
    std::unique_ptr<Node> ptr = parse_expression();
    expect(TokenKind::End, "End");
    return ptr;
}