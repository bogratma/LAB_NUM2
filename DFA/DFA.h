//
// Created by vlad on 3/17/26.
//

#ifndef LAB_NUM2_DFA_H
#define LAB_NUM2_DFA_H
#include <unordered_map>

#include "../NFA/NFA.h"
#include "../NFA/State.h"


class DFA {
    public:
    int startDFA{};
    std::map<std::set<State*>,int> tableDFA{};
    std::map<int,std::map<char,int>> transitionTable{};

    std::set<int> finalDFA{};
    std::set<char> alphabet{};
    DFA(){};
    std::set<State*> epsClosure(State*,NFA&);
    void process(NFA&);
    std::set<State*> Tmove(const std::set<State*>&,NFA&,char);
    void DFS(State* cur,NFA& nfa, std::set<State*>& states) ;
    void dumpDot(const std::string& filename) ;
};


#endif //LAB_NUM2_DFA_H