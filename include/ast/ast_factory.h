#ifndef AST_FACTORY_H
#define AST_FACTORY_H

#include <unordered_map>
#include <memory>
#include <functional>
#include "ast.h"


// Factory class
class ASTNodeFactory {
public:
    using CreatorFunc = std::function<std::shared_ptr<ASTreeNode>()>;


    static ASTNodeFactory& getInstance();

    void registerClass(const std::string& name, const CreatorFunc creator);


    std::shared_ptr<ASTreeNode> create(const std::string& name);


private:
    std::unordered_map<std::string, CreatorFunc> registry;
    ASTNodeFactory() = default; // Private constructor for singleton pattern
};

#endif //!AST_FACTORY_H