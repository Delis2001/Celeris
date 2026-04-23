#include "util/decisions/ifDecTree.h"
#include "ast/ast.h"
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> tokenize(const std::string& expr) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quote = false;
    for (size_t j = 0; j < expr.size(); ++j) {
        char c = expr[j];
        if (c == '"' && !in_quote) {
            in_quote = true;
            token += c;
        } else if (c == '"' && in_quote) {
            in_quote = false;
            token += c;
            tokens.push_back(token);
            token.clear();
        } else if (!in_quote && c == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else if (!in_quote && (c == '=' || c == '&' || c == '|' || c == '!' || c == '+' || c == '-' || c == '*' || c == '/')) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            token += c;
            if (j + 1 < expr.size() && ((c == '=' && expr[j + 1] == '=') || (c == '&' && expr[j + 1] == '&') || (c == '|' && expr[j + 1] == '|') || (c == '!' && expr[j + 1] == '='))) {
                token += expr[j + 1];
                j++;
            }
            tokens.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        res.push_back(item);
    }
    return res;
}

ASTreeNode* findNode(ASTreeNode* root, const std::string& tag, const std::string& name = "") {
    if (root->getTag() == tag && (name.empty() || root->getAttribute("name") == name)) return root;
    for (auto& child : root->GetChildren()) {
        auto f = findNode(child.get(), tag, name);
        if (f) return f;
    }
    return nullptr;
}

std::string resolve(decNode* node, ASTreeNode* root) {
    std::string path = node->value;
    auto parts = split(path, '.');
    ASTreeNode* found = nullptr;
    if (parts.size() == 2) {
        found = findNode(root, parts[0]);
    } else if (parts.size() == 3) {
        found = findNode(root, parts[0], parts[1]);
    }
    if (found) {
        return found->getAttribute(parts.back());
    }
    return "";
}

bool eval_decnode(decNode* node, ASTreeNode* root) {
    if (!node) return true;
    switch (node->type) {
        case OPS::EQ:
            return resolve(node->left, root) == resolve(node->right, root);
        case OPS::NEQ:
            return resolve(node->left, root) != resolve(node->right, root);
        case OPS::AND:
            return eval_decnode(node->left, root) && eval_decnode(node->right, root);
        case OPS::OR:
            return eval_decnode(node->left, root) || eval_decnode(node->right, root);
        default:
            return false;
    }
}

decNode* parse_or(const std::vector<std::string>& tokens, int& i);
decNode* parse_and(const std::vector<std::string>& tokens, int& i);
decNode* parse_eq(const std::vector<std::string>& tokens, int& i);
decNode* parse_base(const std::vector<std::string>& tokens, int& i);

decNode* parse_or(const std::vector<std::string>& tokens, int& i) {
    decNode* left = parse_and(tokens, i);
    while (i < (int)tokens.size() && tokens[i] == "||") {
        i++;
        decNode* right = parse_and(tokens, i);
        decNode* node = new decNode(OPS::OR, "||");
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

decNode* parse_and(const std::vector<std::string>& tokens, int& i) {
    decNode* left = parse_eq(tokens, i);
    while (i < (int)tokens.size() && tokens[i] == "&&") {
        i++;
        decNode* right = parse_eq(tokens, i);
        decNode* node = new decNode(OPS::AND, "&&");
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

decNode* parse_eq(const std::vector<std::string>& tokens, int& i) {
    decNode* left = parse_base(tokens, i);
    if (i < (int)tokens.size() && (tokens[i] == "==" || tokens[i] == "!=")) {
        std::string op = tokens[i];
        i++;
        decNode* right = parse_base(tokens, i);
        decNode* node = new decNode(op == "==" ? OPS::EQ : OPS::NEQ, op);
        node->left = left;
        node->right = right;
        return node;
    }
    return left;
}

decNode* parse_base(const std::vector<std::string>& tokens, int& i) {
    if (i >= (int)tokens.size()) return nullptr;
    decNode* node = new decNode(OPS::BASE, tokens[i]);
    i++;
    return node;
}

/** 
 * Looks for the middle operator if it exists
 * 
 * then returns a data structure, containing the start and end positions of the operator 
 * 
 * In our case every value should be truthy...
 * In future releases, there is the possibility of a falsy value
 * Therefore the !operator would not be a part of the tree
 * 
 * The order of operations also needs to be considered
*/
void ifDecTree::split_eq(const std::string& expression){
    auto tokens = tokenize(expression);
    int i = 0;
    head = parse_or(tokens, i);
}

void ifDecTree::addToTree(decNode* node, decNode* starter){
    if (!head) {
        head = node;
    } else {
        // combine with AND
        decNode* andNode = new decNode(OPS::AND, "&&");
        andNode->left = head;
        andNode->right = node;
        head = andNode;
    }
}

ifDecTree::ifDecTree()
{
    head = nullptr;
}

ifDecTree::~ifDecTree(){
    // TODO: delete tree
}

bool ifDecTree::evaluate(ASTreeNode* root) const {
    return eval_decnode(head, root);
}