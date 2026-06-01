//
// Created by vlad on 3/20/26.
//

#ifndef LAB_NUM2_MDFA_H
#define LAB_NUM2_MDFA_H
#include "Product.h"
#include "../DFA/DFA.h"


class MDFA {
public:
    std::map<int, std::map<char,int>> tableTransitionMDFA;
    std::vector<std::set<int>> Pi;
    std::set<int> finalPi;
    std::set<char> alphabet;
    int startMDFA{};
    [[nodiscard]] const std::map<int, std::map<char, int>>& getTable() const {
        return tableTransitionMDFA;
    }
    [[nodiscard]] int getStart() const { return startMDFA; }
    [[nodiscard]] const std::set<int>& getFinals() const { return finalPi; }
    void setStart(int id) { startMDFA = id; }
    void addFinal(int id) { finalPi.insert(id); }
    void setAlphabet(const std::set<char>& alpha) { alphabet = alpha; }
    void addTransition(int from, char sym, int to) {tableTransitionMDFA[from][sym] = to;}
    void minimize( DFA& dfa);
    bool checkGroup(DFA& dfa,const std::set<int>& group) const;
    std::vector<std::set<int>> split(const std::set<int>& group,DFA& dfa) const;
    void buildMFDA(DFA& dfa);
    void dumpDOT(const std::string& filename);
    bool match(const std::string&);
    bool search(const std::string&);
    void selfMin();
    static bool in(const MDFA& A,const MDFA& B);
    NFA Reverse();
    int runSt(int start, const std::string& s) const ;
    MDFA invHom(const std::map<char, const std::string>& rules) ;

     [[nodiscard]] static MDFA diff(const MDFA&, const MDFA&, bool);
    std::vector<int> getAllFinInd(const std::string& s) ;
    [[nodiscard]] static Product getProduct(const MDFA& A,const MDFA& B) ;
     [[nodiscard]]static bool equal(const MDFA& A,const MDFA& B) ;
    [[nodiscard]] bool isEmpty() const;
};


#endif //LAB_NUM2_MDFA_H