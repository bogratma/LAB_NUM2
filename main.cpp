#include <iostream>
#include "tree/Node.h"
#include "NFA/State.h"
#include "NFA/NFA.h"
#include "DFA/DFA.h"

int main() {
std::string regular;
std::cin >> regular;
    try {
        std::unique_ptr<Node> root = std::move(Parser(regular));
        drawTree(root.get(),"tree");
        NFA nfa;
        nfa.compile(root.get());
        nfa.dumpDot("nfa.dot");
        DFA dfa ;
        dfa.process(nfa);
        dfa.dumpDot("dfa.dot");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }



}