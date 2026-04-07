//
// Created by vlad on 3/13/26.
//
#include "Node.h"

#include <iostream>
#include <memory>
#include <vector>

#include "ParserResult.h"
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
    if (c=='|') binaryNode->type=OR;
    else binaryNode->type=CONCAT;
    syms.push(std::move(binaryNode));

}
void useUnary(std::stack<std::unique_ptr<Node>>& syms,char c) {
    if (syms.empty()) throw std::runtime_error("Unary op error");
    if (c=='+') {
        if (syms.empty()) throw std::runtime_error("Invalid operation (need 1 operand)");
        auto aNode = std::move(syms.top());
        syms.pop();
        auto clon = aNode->clone();
        auto starNode = std::make_unique<Node>('*',std::move(aNode),nullptr);
        starNode->type=STAR;
        auto concat = std::make_unique<Node>('.',std::move(clon),std::move(starNode));
        concat->type=CONCAT;
        syms.push(std::move(concat));
    }

    else if (c=='*') {
        if (syms.empty()) throw std::runtime_error("Invalid operation (need 1 operand)");
        auto aNode = std::move(syms.top());
        syms.pop();
        auto starNode = std::make_unique<Node>(c, std::move(aNode),nullptr);
        starNode->type=STAR;
        syms.push(std::move(starNode));
    }
}
std::unique_ptr<Node> useRange(const std::string& s, size_t& i) {
    const size_t close = s.find(']', i);
    if (close == std::string::npos) throw std::runtime_error("No ]");
    std::unique_ptr<Node> rangeRoot = nullptr;
    for (size_t j = i + 1; j < close; ++j) {
        char start = s[j];
        char end = start;
        if (j + 2 < close && s[j + 1] == '-') {
            end = s[j + 2];
            j += 2;
        }
        if (start > end) std::swap(start, end);
        for (char m = start; m <= end; ++m) {
            auto nextLeaf = std::make_unique<Node>(m);
            nextLeaf->type = SYM;
            if (!rangeRoot) {
                rangeRoot = std::move(nextLeaf);
            } else {
                auto orNode = std::make_unique<Node>('|');
                orNode->type = OR;
                orNode->left = std::move(rangeRoot);
                orNode->right = std::move(nextLeaf);
                rangeRoot = std::move(orNode);
            }
        }
    }
    if (!rangeRoot) throw std::runtime_error("Empty []");
    i = close;
    return rangeRoot;
}
std::pair<int,int> parseBounds(const std::string& s, size_t& i) {
    size_t j= i + 1;
    int m=0;
    int n=-1;
    if (j<s.size() && s[j] == ',') {
        m=0;
        j++;
        if (j>=s.size() || !isdigit(s[j]))
            throw std::runtime_error("Invalid {,}");
        n=0;
        while (j<s.size() && isdigit(s[j])) {
            n=n*10 +(s[j] - '0');
            j++;
        }
    }
    else if (j<s.size() && isdigit(s[j])) {
        m=0;
        while (j<s.size() && isdigit(s[j])) {
            m=m*10+(s[j]-'0');
            j++;
        }
        if (j<s.size() && s[j] == '}') {
            n=m;
        }
        else if (j<s.size() && s[j] == ',') {
            j++;
            if (j<s.size() && isdigit(s[j])) {
                n=0;
                while (j < s.size() && isdigit(s[j])) {
                    n=n*10+(s[j]-'0');
                    j++;
                }
            } else {
                n=-1;
            }
        } else {
            throw std::runtime_error("Invalid {m...}");
        }
    }
    else {
        throw std::runtime_error("Invalid {");
    }
    if (j>=s.size() || s[j] != '}')
        throw std::runtime_error("No }");
    i=j;
    if (n !=-1 && m>n)
        throw std::runtime_error("Invalid range");

    return {m, n};
}
void useRepeat(std::stack<std::unique_ptr<Node>>& syms, const std::string& s, size_t& i) {
    if (syms.empty()) throw std::runtime_error("Repeat needs operand");
    auto base = std::move(syms.top());
    syms.pop();
    auto [m, n] = parseBounds(s, i);
    auto makeChain = [&](int count) -> std::unique_ptr<Node> {
        if (count == 0) {
            auto eps = std::make_unique<Node>('$');
            eps->type = SYM;
            return eps;
        }
        std::unique_ptr<Node> chain = base->clone();
        for (int k = 1; k < count; ++k) {
            auto concat = std::make_unique<Node>('.', std::move(chain), base->clone());
            concat->type = CONCAT;
            chain = std::move(concat);
        }
        return chain;
    };
    std::unique_ptr<Node> result = nullptr;
    if (n != -1) { //верхняя граница
        for (int k = m; k <= n; ++k) {
            auto branch = makeChain(k);
            if (!result) {
                result = std::move(branch);
            } else {
                auto orNode = std::make_unique<Node>('|', std::move(result), std::move(branch));
                orNode->type = OR;
                result = std::move(orNode);
            }
        }
    }
    else {
        result = makeChain(m);
        auto star = std::make_unique<Node>('*', base->clone(), nullptr);
        star->type = STAR;

        if (m == 0) {
            result = std::move(star);
        } else {
            auto concat = std::make_unique<Node>('.', std::move(result), std::move(star));
            concat->type = CONCAT;
            result = std::move(concat);
        }
    }

    syms.push(std::move(result));
}
ParserResult Parser(const std::string& s) {
    ParserResult result;
    std::stack<char> op;
    std::stack<std::unique_ptr<Node>> sym;
    bool prev = false;
    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        bool flag = false;

        if (c=='%') {
            if (i+1>=s.length()) throw std::runtime_error("End with %");
            flag = true;
            c = s[++i];
        }
        if (!flag && c == '/') {
            while (!op.empty()) {
                if (op.top() == '(') throw std::runtime_error("Unclosed ( before /");
                useBinary(sym, op);
            }
            result.mainTree = std::move(sym.top());
            sym.pop();
            if (i + 1 < s.length()) {
                result.lookahead = s.substr(i + 1);
                result.hasLookahead = true;
            }
            return result;
        }

        bool curPrev = flag || getPrior(s[i]) == 0;
        if (prev && (curPrev || c=='(' ||c=='[')) {
                while (!op.empty() && getPrior(op.top())>=getPrior('.')) {
                    useBinary(sym,op);
                }
                op.push('.');
        }

        if (!flag && c=='('){ op.push(c);prev=false;}

        else if (!flag && c==')') {
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
            prev=true;
        }
        else if(!flag && (c=='*' || c=='+')) {
            useUnary(sym, c);
            prev = true;
        }

        else if (!flag && c=='|') {
            while (!op.empty() && getPrior(op.top())>=getPrior(c)) {
                useBinary(sym, op);
            }
            op.push(c);
            prev = false;
        }
        else if (!flag && c == '[') {
            sym.push(useRange(s, i));
            prev = true;
        }
        else if (!flag && c == '{') {
            useRepeat(sym, s, i);
            prev = true;
        }
        else {
            auto aNode = std::make_unique<Node>(c);
            aNode->type=SYM;
            sym.push(std::move(aNode));
            prev = true;
        }

    }

    while (!op.empty()) {
        if (op.top()=='(') {throw std::runtime_error("Extra (");}
        useBinary(sym, op);
    }
    if (!sym.empty()) {
        result.mainTree = std::move(sym.top());
        sym.pop();
    }
    if (sym.size() > 1) throw std::runtime_error("Unprocessed symbols!");
    return result;
}


int getPrior(char c) {
    switch (c) {
        case '*':
        case '+':return 3;
        case '.': return 2;
        case '|': return 1;
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
            return -1;
        default: return 0;
    }
}

void postOrder(Node* root,std::vector<Node*>& names) {
    if (root==nullptr) return;
    postOrder(root->left.get(),names);
    postOrder(root->right.get(),names);
    names.push_back(root);
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
