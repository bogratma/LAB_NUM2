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
    std::set<char> alphabet;
    int startMDFA{};

    void minimize( DFA& dfa);
    bool checkGroup(DFA& dfa,const std::set<int>& group) const;
    std::vector<std::set<int>> split(const std::set<int>& group,DFA& dfa) const;
    void buildMFDA(DFA& dfa);
    void dumpDOT(const std::string& filename);
    bool match(const std::string&);
    bool search(const std::string&);
    void selfMin();
    std::string toRegex();
    void initStackEl(std::map<std::pair<int, int>, std::string>& R, int S, int F);
    std::pair<std::vector<int>, std::vector<int>> getNeighbors(const std::map<std::pair<int, int>, std::string>& R, int k) ;
    MDFA diff(MDFA&, MDFA&,bool);
};


#endif //LAB_NUM2_MDFA_H