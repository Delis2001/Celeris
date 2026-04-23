#include <string>

/** 
 * Defines the structure of a single node in a decision tree.
 * 
 * We need to note that an operator cannot be a leafnode
 * Leaf nodes are nodes that we find do not have children
 * In this case we use the isOp which would be initialized in the constructor]
 * 
 * if isOp is true and left and right are nullptrs then throw a std::system_error and stop the program
 * 
 * decNode* left takes priority over decNode* right
*/

enum class OPS {
    EQ,
    AND,
    OR,
    ADD,
    NOT,
    NEQ,
    SUB,
    DIV,
    MUL,
    BASE // means numbers, any number in the expression
};
struct decNode
{
    OPS type;
    bool isOp;
    std::string value;
    decNode* left;
    decNode* right;

    decNode(): left(nullptr), right(nullptr) {}

    decNode(OPS type, const std::string value)
    : type(type), value(value), left(nullptr), right(nullptr)
    {}
};
