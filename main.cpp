#include <iostream>
#include "tree/Node.h"
#include "NFA/State.h"
#include "NFA/NFA.h"
#include "DFA/DFA.h"
#include "MDFA/MDFA.h"
#include "Regex/Regex.h"
#include "StateElimination/StateElim.h"

int main() {
    try {
        /*std::string regular;
        std::cout<<"Enter regular expression1: "<<std::endl;
        std::cin >> regular;
        Regex regex(regular);
        regex.comp();
        StateElim st;
        std::string second = st.getRegex(regex.mainAutomata);
        std::cout<<second<<std::endl;
        Regex regex2(second);
        regex2.comp();
        std::cout<<MDFA::equal(regex.mainAutomata,regex2.mainAutomata)<<std::endl;*/


        /*std::string regular; //difference
        std::cout<<"Enter regular expression1: "<<std::endl;
        std::cin >> regular;
        Regex regex(regular);

        std::string regular1;
        std::cout<<"Enter regular expression2: "<<std::endl;
        std::cin >> regular1;
        Regex regex1(regular1);

        regex1.comp();
        regex.comp();
        MDFA dif = MDFA::diff(regex.mainAutomata,regex1.mainAutomata,false);
        dif.dumpDOT("dif.dot");*/
        ParserResult root = Parser("(a|b)(!f)");
        NFA nfa;
        nfa.compile(root.mainTree.get());
        nfa.dumpDot("cap.dot");
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }

}
