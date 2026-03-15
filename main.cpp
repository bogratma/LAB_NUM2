#include <iostream>
#include "tree/Node.h"
#include "NFA/State.h"
#include "NFA/NFA.h"

int main() {
std::string regular;
std::cin >> regular;
    try {
        std::unique_ptr<Node> root = std::move(Parser(regular));
        drawTree(root.get(),"tree");
        NFA nfa;
        nfa.compile(root.get());
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }



}