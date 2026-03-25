//
// Created by vlad on 3/25/26.
//

#ifndef LAB_NUM2_REGEX_H
#define LAB_NUM2_REGEX_H
#include "../MDFA/MDFA.h"


class Regex {
    std::unique_ptr<Node> root;
    bool is_compiled = false;
    public:
    MDFA automata;
    explicit Regex(const std::string& pattern) {
        root = Parser(pattern);
        drawTree(root.get(),"tree");
    }
    void comp() {
        if (is_compiled) return;
        NFA nfa;
        nfa.compile(root.get());
        nfa.dumpDot("nfa.dot");
        DFA dfa ;
        dfa.process(nfa);
        dfa.dumpDot("dfa.dot");
        automata.minimize(dfa);
        automata.dumpDOT("mdfa.dot");
        is_compiled = true;
    }
    bool match(const std::string& s) {
        if (!is_compiled) comp();
        return automata.match(s);
    }
    bool search(const std::string& s) {
        if (!is_compiled) comp();
        return automata.search(s);
    }
};


#endif //LAB_NUM2_REGEX_H