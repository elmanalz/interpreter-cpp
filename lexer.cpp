#include"lexer.h"
#include<charconv>
#include<exception>
#include<stdexcept>
#include<unordered_map>


namespace calc 
{
    namespace
    {
        bool isOperator(const char c)
        {
            return (c == '*' || c == '+' || c == '-' || c == '/' || c == '%' || c == '^' || 
                c == '(' || c == ')' || c == '=' || c == '<' || c == '>' || c == '!');
        }

        double read_number(const std::string& s, size_t& pos)
        {
            size_t initial = pos;
            bool dot_flag = false;
            double num = 0.0;
            std::string number;
            for (; pos < s.length(); pos++)
            {
                if(isdigit(static_cast<unsigned char> (s.at(pos))))
                {
                    number.push_back(s.at(pos));
                }
                else if (s.at(pos) == '.')
                {
                    if(!dot_flag)
                    {
                        number.push_back('.');
                        dot_flag = true;
                    }
                    else
                    {
                        throw std::invalid_argument("Lexer Error: Invalid Number, contains more than one point!");
                    }
                }
                else
                    break;
            }
            auto [ptr, ec] = std::from_chars(number.data(), number.data() + pos - initial, num);

            if (ec == std::errc() && ptr == (number.data() + number.size()))
                return num;
            else 
                throw std::invalid_argument("Lexer Error: No number in the specified position");
        }

        std::string read_identifier(const std::string& s, size_t& pos)
        {
            size_t initial = pos;
            std::string str;
            for (; pos < s.length(); pos++)
            {
                if(isalpha(static_cast<unsigned char> (s.at(pos))) || (pos > initial && isdigit(static_cast<unsigned char> (s.at(pos)))) || s.at(pos) == '_')
                    str.push_back(s.at(pos));
                else
                    break;
            }

            if(pos == initial)
                throw std::invalid_argument("Lexer Error: No identifier in the specified position");
            
            return str;
        }


        TokenKind read_operator(const std::string& s, size_t& pos)
        {
            static const std::unordered_map<std::string, TokenKind> operator_map = 
                {{"*", TokenKind::Star}, {"+", TokenKind::Plus}, {"-", TokenKind::Minus}, {"/", TokenKind::Slash}, 
                {"%", TokenKind::Percent}, {"(", TokenKind::LParen}, {")", TokenKind::RParen}, {"=", TokenKind::Assign},
                {"^", TokenKind::Caret}, {"==", TokenKind::Equal}, {"!=", TokenKind::NotEqual}, {"<", TokenKind::Less},
                {">", TokenKind::Greater}, {"<=", TokenKind::LessEqual}, {">=", TokenKind::GreaterEqual}};

            
            std::string str;
            TokenKind kind;
            size_t longest = 0;
            for(size_t i = pos; i < s.length(); i++)
            {
                if(isOperator(s.at(i)))
                {
                    str.push_back(s.at(i));
                    if(operator_map.find(str) != operator_map.end())
                    {
                        longest = str.length();
                        kind = operator_map.at(str);
                    }
                }
                else 
                    break;
            }
            
            

            if(longest == 0)
                throw std::invalid_argument("Lexer Error: No operator in the specified position");
            pos += longest;
            return kind;
        }
    }

    std::vector<Token> lex(const std::string& a)
    {
        std::vector<Token> tokens;
        size_t i = 0;
        
        for (; i < a.length();)
        {
            size_t index = i;
            if(std::isspace(static_cast<unsigned char>(a.at(i))))
            {
                i++;
                continue;
            }
            if(isdigit(static_cast<unsigned char> (a[i])) || a[i] == '.')
            {
                double v = 0.0;
                v = calc::read_number(a, i);
                Token tkn(TokenKind::Number, v, index);
                tokens.push_back(tkn);
            }
            else if(isalpha(static_cast<unsigned char>(a.at(i))) || a.at(i) == '_')
            {
                std::string v;
                v = calc::read_identifier(a, i);
                Token tkn(TokenKind::Identifier, v, index);
                tokens.push_back(tkn);
            }
            else if(calc::isOperator(a.at(i)))
            {
                TokenKind t;
                t = calc::read_operator(a, i);
                Token tkn(t, index);
                tokens.push_back(tkn);
            }
            else if(a.at(i) == ',')
            {
                TokenKind t = TokenKind::Comma;
                Token tkn(t, index);
                tokens.push_back(tkn);
                i++;
            }
            else
                throw std::invalid_argument("Lexer Error: Unsupported characters.");
        }
        Token tkn(TokenKind::End, i);
        tokens.push_back(tkn);
        
        return tokens;
    }
}