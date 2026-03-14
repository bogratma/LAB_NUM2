//
// Created by vlad on 3/13/26.
//
#include "Node.h"
#include <memory>
#include "stack"

void useOp(std::stack<std::unique_ptr<Node>>& syms, std::stack<char>& op) {
    if (op.empty()) return;
    char c = op.top();
    op.pop();
    if (c=='*') {
        if (syms.empty()) return;
        auto aNode = std::move(syms.top());
        syms.pop();
        auto starNode = std::make_unique<Node>(c, std::move(aNode),nullptr);
        syms.push(std::move(starNode));
    }
    else {
        if (syms.size()<2) return ;
        auto aNodeRight = std::move(syms.top());
        syms.pop();
        auto aNodeLeft = std::move(syms.top());
        syms.pop();
        auto binaryNode = std::make_unique<Node>(c, std::move(aNodeLeft), std::move(aNodeRight));
        syms.push(std::move(binaryNode));
    }
}

std::unique_ptr<Node> Parser(const std::string& s) {
    std::stack<char> op;
    std::stack<std::unique_ptr<Node>> sym;

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c=='(') op.push(c);
        else if (c==')') {
            while (!op.empty() && op.top()!='(') {
                useOp(sym, op);
            }
            if (!op.empty()) op.pop();
        }

        else if (c=='*' || c=='.' || c=='|') {
            while (!op.empty() && op.top()!='(' && getPrior(op.top())>=getPrior(c)) {
                useOp(sym, op);
            }
            op.push(c);
        }

        else {
            auto aNode = std::make_unique<Node>(c);
            sym.push(std::move(aNode));
        }

    }

    while (!op.empty()) {
        if (op.top()=='(') {op.pop();continue;}
        useOp(sym, op);
    }

    return std::move(sym.top());
}

int getPrior(char c) {
    switch (c) {
        case '*': return 3;
        case '.': return 2;
        case '|': return 1;
        default: return 0;
    }
}
void postOrder(Node* root) {
    if (root==nullptr) return;
    postOrder(root->left.get());
    postOrder(root->right.get());
    std::cout << root->name << std::endl;
}
void writeNodes(Node* root, std::ostream& out) {
    if (!root) return;
    writeNodes(root->left.get(), out);
    writeNodes(root->right.get(), out);
    out << "    \"" << root << "\" [label=\"" << root->name << "\", shape=circle];\n";
    if (root->left) {
        out << "    \"" << root << "\" -> \"" << root->left.get() << "\";\n";
    }
    if (root->right) {
        out << "    \"" << root << "\" -> \"" << root->right.get() << "\";\n";
    }
}

void drawTree(Node* root, std::string filename = "tree") {
    std::string dotFile = filename + ".dot";
    std::string pngFile = filename + ".png";
    std::ofstream out(dotFile);
    if (!out) {
        std::cerr << "Ошибка создания файла!" << std::endl;
        return;
    }
    out << "digraph G {\n";
    out << "    rankdir=TB;\n";
    writeNodes(root, out);
    out << "}\n";
    out.close();
    std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
    std::system(command.c_str());
    std::cout << "Граф сохранен как " << pngFile << std::endl;
}