//
// Created by vlad on 3/20/26.
//

#ifndef LAB_NUM2_MDFA_H
#define LAB_NUM2_MDFA_H
#include "../DFA/DFA.h"


class MDFA {
public:
    std::map<int, std::map<char,int>> tableTransitionMDFA;
    std::vector<std::set<int>> Pi;
    std::set<int> finalPi;

    int states{};
    int startMDFA{};

    void minimize( DFA& dfa);
    bool checkGroup(DFA& dfa,const std::set<int>& group);
    std::vector<std::set<int>> split(const std::set<int>& group,DFA& dfa);
    void buildMFDA(DFA& dfa);
    void dumpDOT(std::string filename);
};


#endif //LAB_NUM2_MDFA_H