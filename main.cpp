#include <iostream>
#include "tree/Node.h"
#include "NFA/State.h"
#include "NFA/NFA.h"
#include "DFA/DFA.h"
#include "MDFA/MDFA.h"
#include "Regex/Regex.h"

int main() {
    try {
        std::string regular;
        std::cout<<"Enter regular expression1: "<<std::endl;
        std::cin >> regular;
        Regex regex(regular);

        std::string regular1;
        std::cout<<"Enter regular expression2: "<<std::endl;
        std::cin >> regular1;
        Regex regex1(regular1);
        regex.comp();
        regex1.comp();
        regex.automata.dumpDOT("mdfa1.dot");
        regex1.automata.dumpDOT("mdfa2.dot");
        MDFA mdfa;
        mdfa = mdfa.diff(regex.automata,regex1.automata,false);
        mdfa.dumpDOT("mdfa3.dot");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
}
