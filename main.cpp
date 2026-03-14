#include <iostream>
#include "tree/Node.h"
int main() {
std::string regular;
std::cin >> regular;
std::unique_ptr<Node> root = std::move(Parser(regular));
drawTree(root.get(),"tree");
}