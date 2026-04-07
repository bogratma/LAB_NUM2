//
// Created by vlad on 3/31/26.
//

#ifndef LAB_NUM2_STATEELIM_H
#define LAB_NUM2_STATEELIM_H
#include <iostream>
#include <map>
#include <ranges>
#include <string>

#include "../MDFA/MDFA.h"


class StateElim {
    public:
    std::map<int, std::map<int, std::string>> table;
        StateElim() = default;
        void init(const MDFA& mdfa) ;
        void eliminateState(int q) ;
    std::string getRegex(const MDFA& mdfa) ;
    void dumpDOT(const std::string& filename) const {
            std::ofstream out(filename);
            out << "digraph StateElim {\n";
            out << "  rankdir=LR;\n";
            for (auto const& [from, moves] : table) {
                for (auto const& [to, regex] : moves) {
                    std::string label = regex.empty() ? "ε" : regex;
                    out << "  " << from << " -> " << to
                        << " [label=\"" << label << "\"];\n";
                }
            }
            out << "}\n";
            out.close();
        }
};
#endif //LAB_NUM2_STATEELIM_H