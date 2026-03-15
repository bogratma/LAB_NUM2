//
// Created by vlad on 3/13/26.
//

#ifndef LAB_NUM2_NODE_H
#define LAB_NUM2_NODE_H
#include <fstream>
#include <memory>
#include <vector>

class Node {
    public:
    char name;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node(const char name, std::unique_ptr<Node> left, std::unique_ptr<Node> right):name(name),left(std::move(left)),right(std::move(right)){};
    explicit Node(char name):name(name),left(nullptr),right(nullptr){};
    [[nodiscard]] std::unique_ptr<Node> clone() const {
        auto newNode = std::make_unique<Node>(name);
        if (left) newNode->left = left->clone();
        if (right) newNode->right = right->clone();
        return newNode;
    }
};

std::unique_ptr<Node> Parser(const std::string& s);
int getPrior(char c);
void postOrder(Node*,std::vector<char>&);
void drawTree(Node*, std::string);


#endif //LAB_NUM2_NODE_H