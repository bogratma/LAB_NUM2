//
// Created by vlad on 3/13/26.
//
#include "Node.h"
#include <memory>
#include <vector>

#include "stack"

void useBinary(std::stack<std::unique_ptr<Node>>& syms, std::stack<char>& op) {
    if (op.empty()) throw std::runtime_error("Operation stack is empty");
    char c = op.top();
    op.pop();
    if (syms.size()<2) throw std::runtime_error("Invalid operation (need 2 operands)") ;
    auto aNodeRight = std::move(syms.top());
    syms.pop();
    auto aNodeLeft = std::move(syms.top());
    syms.pop();
    auto binaryNode = std::make_unique<Node>(c, std::move(aNodeLeft), std::move(aNodeRight));
    syms.push(std::move(binaryNode));

}
void useUnary(std::stack<std::unique_ptr<Node>>& syms,char c) {
    if (syms.empty()) throw std::runtime_error("Unary op error");
    if (c=='+') {
        if (syms.empty()) throw std::runtime_error("Invalid operation (need 1 operand)");
        auto aNode = std::move(syms.top());
        auto clon = aNode->clone();
        syms.pop();
        auto starNode = std::make_unique<Node>('*',std::move(aNode),nullptr);
        auto concat = std::make_unique<Node>('.',std::move(clon),std::move(starNode));
        syms.push(std::move(concat));
    }

    else if (c=='*') {
        if (syms.empty()) throw std::runtime_error("Invalid operation (need 1 operand)");
        auto aNode = std::move(syms.top());
        syms.pop();
        auto starNode = std::make_unique<Node>(c, std::move(aNode),nullptr);
        syms.push(std::move(starNode));
    }
}
std::unique_ptr<Node> Parser(const std::string& s) {
    std::stack<char> op;
    std::stack<std::unique_ptr<Node>> sym;

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];

        if (i>0) {
            char p = s[i-1];
            bool left = getPrior(p)==0 || p==')'|| p=='*';
            bool right = getPrior(c)==0 || c=='(';
            if (left && right) {
                while (!op.empty() && getPrior(op.top())>=getPrior('.')) {
                    useBinary(sym,op);
                }
                op.push('.');
            }
        }

        if (c=='(') op.push(c);

        else if (c==')') {
            bool correct = false;
            while (!op.empty()) {
                if (op.top()=='(') {
                    correct=true;
                    break;
                }
                useBinary(sym, op);
            }
            if (!correct) throw std::runtime_error("Extra )");
                op.pop();
        }

        else if(c=='*' || c=='+') {
            useUnary(sym, c);
        }

        else if (c=='|') {
            while (!op.empty() && getPrior(op.top())>=getPrior(c)) {
                useBinary(sym, op);
            }
            op.push(c);
        }

        else {
            auto aNode = std::make_unique<Node>(c);
            sym.push(std::move(aNode));
        }
    }

    while (!op.empty()) {
        if (op.top()=='(') {throw std::runtime_error("Extra (");}
        useBinary(sym, op);
    }

    if (sym.size() > 1) throw std::runtime_error("Unprocessed symbols!");
    return sym.empty() ? nullptr : std::move(sym.top());
}

int getPrior(char c) {
    switch (c) {
        case '*':
        case '+':return 3;
        case '.': return 2;
        case '|': return 1;
        case '(':
        case ')':return -1;
        default: return 0;
    }
}
void postOrder(Node* root,std::vector<char>& names) {
    if (root==nullptr) return;
    postOrder(root->left.get(),names);
    postOrder(root->right.get(),names);
    names.push_back(root->name);
    //std::cout << root->name << std::endl;
}
void writeNodes(Node* root, std::ostream& out) {
    if (!root) return;
    writeNodes(root->left.get(), out);
    writeNodes(root->right.get(), out);
    out<< "\""<<root<<"\" [label=\""<<root->name<<"\", shape=circle];\n";
    if (root->left) {
        out<<"\""<<root <<"\" -> \"" <<root->left.get()<<"\";\n";
    }
    if (root->right) {
        out<<"\""<<root<<"\" -> \"" <<root->right.get()<<"\";\n";
    }
}

void drawTree(Node* root, std::string filename = "tree") {
    std::string dotFile = filename + ".dot";
    std::string pngFile = filename + ".png";
    std::ofstream out(dotFile);
    out<<"digraph G {\n";
    out<<"rankdir=TB;\n";
    writeNodes(root, out);
    out<<"}\n";
    out.close();
    std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
    std::system(command.c_str());
}