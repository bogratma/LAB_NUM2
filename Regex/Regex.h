//
// Created by vlad on 3/25/26.
//

#ifndef LAB_NUM2_REGEX_H
#define LAB_NUM2_REGEX_H
#include "../MDFA/MDFA.h"
#include <memory>
class Regex {
    std::unique_ptr<Node> mainRoot;
    std::unique_ptr<Node> lookRoot;
    bool is_compiled = false;
    bool has_lookahead = false;
public:
    MDFA mainAutomata;
    MDFA lookaheadAutomata;
    explicit Regex(const std::string& pattern) {
        ParserResult res = Parser(pattern);
        mainRoot = std::move(res.mainTree);
        has_lookahead = res.hasLookahead;
        if (has_lookahead) {
            ParserResult lookRes = Parser(res.lookahead);
            lookRoot = std::move(lookRes.mainTree);
        }
        if (mainRoot) drawTree(mainRoot.get(), "main_tree");
        if (lookRoot) drawTree(lookRoot.get(), "look_tree");
    }
    void comp() {
        if (is_compiled) return;
        compileToMDFA(mainRoot.get(), mainAutomata, "main");
        if (has_lookahead && lookRoot) {
            compileToMDFA(lookRoot.get(), lookaheadAutomata, "lookahead");
        }
        is_compiled = true;
    }
    void compileToMDFA(Node* treeRoot, MDFA& target, const std::string& prefix) {
        if (!treeRoot) return;
        NFA nfa;
        nfa.compile(treeRoot);
        nfa.dumpDot(prefix + "nfa.dot");
        DFA dfa;
        dfa.process(nfa);
        dfa.dumpDot(prefix + "dfa.dot");
        target.minimize(dfa);
        target.dumpDOT(prefix + "mdfa.dot");
    }
    bool match(const std::string& s) {
        if (!is_compiled) comp();
        const std::vector<int> points = mainAutomata.getAllFinInd(s);
        if (has_lookahead) {
            for (int i = points.size() - 1; i >= 0; --i) {
                const int pos = points[i];
                std::string tail = s.substr(pos);
                if (lookaheadAutomata.match(tail)) {
                    return true;
                }
            }
            return false;
        }
        if (points.empty()) return false;
        return points.back() == s.length();
    }
    bool search(const std::string& s) {
        if (!is_compiled) comp();
        const std::vector<int> points = mainAutomata.getAllFinInd(s);
        if (has_lookahead) {
            for (int i = points.size() - 1; i >= 0; --i) {
                const int pos = points[i];
                std::string tail = s.substr(pos);
                if (lookaheadAutomata.search(tail)) {
                    return true;
                }
            }
            return false;
        }

        if (points.empty()) return false;
        return points.back() == s.length();
    }
};

#endif
