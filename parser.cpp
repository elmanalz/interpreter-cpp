#include"parser.h"
#include<cassert>


std::unique_ptr<Node> Parser::parse_primary()
{
    size_t entry_pos = pos_;    //Recording the initial position to make sure we have advanced
    std::unique_ptr<Node> ptr;

    //If the current token is a left parentheses, parse the expression inside them separately
    if (match(TokenKind::LParen))
    {
        ptr = parse_expression();
        expect(TokenKind::RParen, "Closing Parenthesis");
    }

    //If the current token is an identifier, make an identifier node
    else if (peek().kind == TokenKind::Identifier)
    {
        ptr = std::make_unique<IdentifierNode>(std::get<std::string>(expect(TokenKind::Identifier, "Identifier").value));
    }

    //If the current tokem is a number, make and identifier node
    else
    {
        ptr = std::make_unique<NumberNode>(std::get<double>(expect(TokenKind::Number, "Number, parenthese, or identifier").value));
    }

    //The function must advance the index at least by 1
    assert((pos_ > entry_pos) && "Parsing failed at parse primary.");

    //Return the sub-tree or leaf node
    return ptr;
}

std::unique_ptr<Node> Parser::parse_unary()
{
    size_t entry_pos = pos_;    //Recording the initial position to make sure we have advanced
    std::unique_ptr<Node> ptr;

    //Continue until the token kinds are Unary +/-
    if(peek().kind == (TokenKind::Minus) || peek().kind == (TokenKind::Plus))   
    {
        TokenKind t = advance().kind;       //Keeping track of the operator kind
        ptr = std::make_unique<UnaryNode>(t, parse_unary());    //Making a tree of the sub-tree by assigning a root of UnaryNode
    }
    else
    {
        //If the token at the current position cannot be consumed by parse_unary, one level below should parse it
        return parse_factor();
    }

    //The function must advance the index at least by 1
    assert((pos_ > entry_pos) && "Parsing failed at parse unary.");

    //Return the sub-tree or leaf node
    return ptr;
}


std::unique_ptr<Node> Parser::parse_factor()
{
    size_t entry_pos = pos_;        //Recording the initial position to make sure we have advanced
    std::unique_ptr<Node> left = parse_primary();   //Parsing the first element of the operation
    while (peek().kind == (TokenKind::Caret))       //continue until the operators are ^
    {
        TokenKind t = advance().kind;                   //Keep track of the operator
        std::unique_ptr<Node> right = parse_unary();    //Parse the right side of the ^
                                            //This order of operation is designed to preserve right associativity
                                            //It recursively continues to parse the rightmost operation first
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
                                            //Concatinate both the right and left sub-tree as a tree with BinaryNode as its root
    }

    //The function must advance the index at least by 1
    assert((pos_ > entry_pos) && "Parsing failed at parse factor.");

    //Return the sub-tree or leaf node
    return left;
}


std::unique_ptr<Node> Parser::parse_term()
{
    size_t entry_pos = pos_;            //Recording the initial position to make sure we have advanced
    std::unique_ptr<Node> left = parse_unary();     //Parsing the left side of the operation


    //Continue until the operator is either *, /, or  %
    while (peek().kind == TokenKind::Star || peek().kind == TokenKind::Slash || peek().kind == TokenKind::Percent)
    {
        TokenKind t = advance().kind;                   //Consume the operator and keep track of the type of it
        std::unique_ptr<Node> right = parse_unary();    //Parse the right side of operator, by one level below

        //Update the sub-tree with a BinaryNode root and the right side of the operation
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
    }

    //The function must advance the index at least by 1
    assert((pos_ > entry_pos) && "Parsing failed at parse term.");

    //Return the sub-tree or leaf node
    return left;
}

std::unique_ptr<Node> Parser::parse_expression()
{
    size_t entry_pos = pos_;    //Recording the initial position to make sure we have advanced
    std::unique_ptr<Node> left = parse_term();  //Parsing the left side of the operation

    //Continue until the operator is either + and -
    while (peek().kind == (TokenKind::Plus) || peek().kind == (TokenKind::Minus))
    {
        TokenKind t = advance().kind;                   //Consume the operator and keep track of the type of it
        std::unique_ptr<Node> right = parse_term();     //Parse the right side of operator, by one level below 

        //Update the sub-tree with a BinaryNode root and the right side of the operation
        left = std::make_unique<BinaryNode>(t, std::move(left), std::move(right));
    }

    //The function must advance the index at least by 1
    assert((pos_ > entry_pos) && "Parsing failed at parse expression.");

    //Return the sub-tree or leaf node
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
    //Calling parse_expression
    std::unique_ptr<Node> ptr = parse_expression();
    //Expecting the End of the tokens
    expect(TokenKind::End, "End");

    //Returning the pointer to the root of the AST tree
    return ptr;
}