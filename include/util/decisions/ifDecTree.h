#ifndef IF_DEC_TREE
#define IF_DEC_TREE

#include <string>
#include <sstream>
#include <functional>

//for node and enum
#include "decNode.h"
#include "../../ast/ast.h"

using OpFunc = std::function<bool(std::string&)>;

class ifDecTree
{
    //contain a list of 
    const std::array<std::pair<OPS, OpFunc>,9> op_checklists{
        {
            {
                OPS::ADD, [](std::string& data){ 
                return (data[0] == '+');  
            }},
            {
                OPS::AND, [](std::string& data){ 
                    return (data[0] == '&' && data[1] == '&'); 
                }},
            {
                OPS::DIV, [](std::string& data){ 
                    return (data[0] == '/'); 
                }},
            {
                OPS::EQ, [](std::string& data){ 
                    return (data[0] == '=' && data[1] == '='); 
                }},
            {
                OPS::MUL, [](std::string& data){ 
                    return(data[0] == '*'); 
                }},
            {
                OPS::NEQ, [](std::string& data){ 
                    return (data[0] == '!' && data[1] == '='); 
                }},
            {
                OPS::OR, [](std::string& data){ 
                    return (data[0] == '|' && data[1] == '|'); 
                }},
            {
                OPS::SUB, [](std::string& data){ 
                    return (data[0] == '-'); 
                }},
            {
                OPS::BASE, [](std::string& data){
                     return true;
                    }} //base is at the end as an else (fail statement)
        }
    };
    void split_eq(const std::string& expression);
    std::vector<std::string> tokenize(const std::string& expr);
    std::vector<std::string> split(const std::string& s, char delim) const;
    std::string resolve(decNode* node) const;
    bool eval_decnode(decNode* node) const;

    decNode* parse_or(const std::vector<std::string>& tokens, int& i);
    decNode* parse_and(const std::vector<std::string>& tokens, int& i);
    decNode* parse_eq(const std::vector<std::string>& tokens, int& i);
    decNode* parse_base(const std::vector<std::string>& tokens, int& i);

    void addToTree(decNode* node, decNode* starter = nullptr);
    decNode* head;
public:
    ifDecTree(/* args */);
    ~ifDecTree();
    bool evaluate() const;

};


#endif //!IF_DEC_TREE


/* 


*/