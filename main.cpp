#include"lexer.h"
#include"parser.h"
#include<iostream>

int main()
{
    std::string str;
    while(std::getline(std::cin, str))
    {
        std::unique_ptr<Node> root = nullptr;
        try
        {
            std::vector<Token> t = calc::lex(str);
            Parser p(t);
            root = p.parse();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Operation Could not be completed: "<<e.what() << " Please try again.\n";
            continue;
        }
        if(root != nullptr)
        {
            try
            {
                std::cout<<root->to_string()<<'\n';
                std::cout<<root->eval()<<'\n'<<'\n';
            }
            catch(const std::exception& e)
            {
                std::cerr << "Operation Could not be completed: "<<e.what() << " Please try again.\n";
                continue;
            }
        }
        
    }
    return 0;
}