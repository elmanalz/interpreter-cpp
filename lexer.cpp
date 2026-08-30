#include"lexer.h"
#include<charconv>
#include<exception>
#include<stdexcept>
#include<unordered_map>

//Use of namespace for the purpose of encapsulation
namespace calc 
{
    //A namespace including the helper functions for the lexer that need 
    //not be accessed from anywhere outside the lexer to prevent name collision
    namespace
    {
        //A helper function to check wether a character can be inside an operator or not
        bool operatorCanInclude(const char c)
        {
            return (c == '*' || c == '+' || c == '-' || c == '/' || c == '%' || c == '^' || 
                c == '(' || c == ')' || c == '=' || c == '<' || c == '>' || c == '!');
        }
        //Helper function to read a numeric literal from the string input
        double read_number(const std::string& s, size_t& pos)
        {
            size_t initial = pos;   //Keeping track of the initial position to see how many positions we have consumed
            bool dot_flag = false;  //A bolean variable to keep track of the decimal separator
            double num = 0.0;       //The variable to keep the number
            std::string number;     //Variable to keep the extracted numeric literal from the string

            //Iterating until the end of the string stream or a non-numeric value (break gaurd inside the loop)
            for (; pos < s.length(); pos++)
            {
                //if statement for checking the kind of character we are consuming
                //Digits
                if(isdigit(static_cast<unsigned char> (s.at(pos))))
                {
                    number.push_back(s.at(pos));
                }
                //Decimal Separator
                else if (s.at(pos) == '.')
                {
                    //Checking if we have already seen a decimal separator, no number can include two '.'
                    if(!dot_flag)  
                    {
                        number.push_back('.');
                        dot_flag = true;    //Updating the point tracket
                    }
                    else
                    {
                        //Throwing for numbers with more than one decimal separator
                        throw std::invalid_argument("Lexer Error: Invalid Number, contains more than one point!");
                    }
                }
                //Reaching the end of numeric literal stream
                else
                    break;
            }
            //Converting the extracted string to a double
            auto [ptr, ec] = std::from_chars(number.data(), number.data() + pos - initial, num);

            //Checking to see if no error has happened
            if (ec == std::errc() && ptr == (number.data() + number.size()))
                return num;
            else 
                //Throwing otherwise
                throw std::invalid_argument("Lexer Error: No number in the specified position");
        }

        //Helper function to read an identifier from the string input
        std::string read_identifier(const std::string& s, size_t& pos)
        {
            size_t initial = pos;   //Keeping track of the initial position to know how many characters we have consumed
            std::string str;        //The string extracted as the identifier

            //Iterating until the end of the string stream or a non-alpanumeric value (break gaurd inside the loop)
            for (; pos < s.length(); pos++)
            {
                //An identifier can contain alphabetical values or an underscore as its first character
                //and an alphanumeric value or an underscore as its following characters
                if(isalpha(static_cast<unsigned char> (s.at(pos))) || (pos > initial && isdigit(static_cast<unsigned char> (s.at(pos)))) || s.at(pos) == '_')
                    str.push_back(s.at(pos));
                else
                    break;
            }

            //Checking to see if we have consumed any characters or not
            if(pos == initial)
                throw std::invalid_argument("Lexer Error: No identifier in the specified position");
            
            return str;
        }

        //Helper function to read an operator from the string input
        TokenKind read_operator(const std::string& s, size_t& pos)
        {
            //Unordered map to keep track of allowed operators, and their corresponding kinds
            static const std::unordered_map<std::string, TokenKind> operator_map = 
                {{"*", TokenKind::Star}, {"+", TokenKind::Plus}, {"-", TokenKind::Minus}, {"/", TokenKind::Slash}, 
                {"%", TokenKind::Percent}, {"(", TokenKind::LParen}, {")", TokenKind::RParen}, {"=", TokenKind::Assign},
                {"^", TokenKind::Caret}, {"==", TokenKind::Equal}, {"!=", TokenKind::NotEqual}, {"<", TokenKind::Less},
                {">", TokenKind::Greater}, {"<=", TokenKind::LessEqual}, {">=", TokenKind::GreaterEqual}};

            
            std::string str;    //String to extract the operator to
            TokenKind kind;     //The kind of extracted operator
            size_t longest = 0; //Longest length of the operator extracted

            //Iterating until the end of the string stream or a non-operator (operatorCanInclude returns false) value (break gaurd inside the loop)
            for(size_t i = pos; i < s.length(); i++)
            {
                if(operatorCanInclude(s.at(i)))
                {
                    str.push_back(s.at(i));
                    //Extracting the operator values and keeping track of the longest operator allowed by the unordered map
                    if(operator_map.find(str) != operator_map.end())
                    {
                        longest = str.length();
                        kind = operator_map.at(str);
                    }
                }
                else 
                    break;
            }
            
            
            //Checking to see if we have consumed any characters
            if(longest == 0)
                throw std::invalid_argument("Lexer Error: No operator in the specified position");

            pos += longest; //Incrementing the position tracker by the length of the longest operator
            return kind;
        }
    }


    //Main lexer function
    //Paramters:
    //  const std::string& a:
    //  Constant reference to a string that needs to be lexed
    //Returns:
    //  a vector cotaining all the tokens extracted from the string
    std::vector<Token> lex(const std::string& a)
    {
        std::vector<Token> tokens;
        size_t i = 0;
        
        //Iterating until the end of the string stream
        for (; i < a.length();)
        {
            size_t index = i;   //recording the index to keep the position where the token starts

            //Ignore whitespaces
            if(std::isspace(static_cast<unsigned char>(a.at(i))))
            {
                i++;
                continue;
            }

            //Checking for numeric literals
            if(isdigit(static_cast<unsigned char> (a[i])) || a[i] == '.')
            {
                double v = 0.0;
                v = calc::read_number(a, i);
                Token tkn(TokenKind::Number, v, index);
                tokens.push_back(tkn);
            }
            //Checking for alphabetical characters
            else if(isalpha(static_cast<unsigned char>(a.at(i))) || a.at(i) == '_')
            {
                std::string v;
                v = calc::read_identifier(a, i);
                Token tkn(TokenKind::Identifier, v, index);
                tokens.push_back(tkn);
            }
            //Checking for operator characters
            else if(operatorCanInclude(a.at(i)))
            {
                TokenKind t;
                t = calc::read_operator(a, i);
                Token tkn(t, index);
                tokens.push_back(tkn);
            }
            //Checking for comma input e.g f(x, y)
            else if(a.at(i) == ',')
            {
                TokenKind t = TokenKind::Comma;
                Token tkn(t, index);
                tokens.push_back(tkn);
                i++;
            }
            //Throw if the character is none of the above
            else
                throw std::invalid_argument("Lexer Error: Unsupported characters.");
        }

        //End Token
        Token tkn(TokenKind::End, i);   
        tokens.push_back(tkn);
        
        return tokens;
    }
}