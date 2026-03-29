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
        regex.comp();
        std::cout<<"Enter string: "<<std::endl;
        std::cin >> regular;
        std::cout<<regex.match(regular);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
}
