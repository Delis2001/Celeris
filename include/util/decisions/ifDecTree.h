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
private:
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
    decNode* head;
public:
    ifDecTree(/* args */);
    ~ifDecTree();

    void addToTree(decNode* node, decNode* starter = nullptr);

    bool evaluate(ASTreeNode* root) const;

};


#endif //!IF_DEC_TREE


/* 


*/