//
// Created by vlad on 3/13/26.
//

#ifndef LAB_NUM2_NODE_H
#define LAB_NUM2_NODE_H
#include <fstream>
#include <iostream>
#include <memory>

class Node {
    public:
    char name;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node(char name, std::unique_ptr<Node> left, std::unique_ptr<Node> right):left(std::move(left)),right(std::move(right)),name(name){};
    explicit Node(char name):name(name),left(nullptr),right(nullptr){};
};

std::unique_ptr<Node> Parser(const std::string& s);
int getPrior(char c);
void postOrder(Node* root);
void drawTree(Node*, std::string);


#endif //LAB_NUM2_NODE_H