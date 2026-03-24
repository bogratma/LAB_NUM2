#include <iostream>
#include "tree/Node.h"
#include "NFA/State.h"
#include "NFA/NFA.h"
#include "DFA/DFA.h"
#include "MDFA/MDFA.h"
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
        MDFA mdfa;
        mdfa.minimize(dfa);
        mdfa.dumpDOT("mdfa.dot");

        std::cout<<"Input: ";
        std::string str;
        std::cin>>str;
        std::cout<<"String is matched flag: "<<mdfa.match(str)<<std::endl;
        std::cout<<"Input: ";
        std::cin>>str;
        std::cout<<"String is search flag: "<<mdfa.search(str)<<std::endl;

    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }



}