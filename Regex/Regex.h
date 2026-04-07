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
    void comp() ;
    void compileToMDFA(Node* treeRoot, MDFA& target, const std::string& prefix);
    bool match(const std::string& s);
    bool search(const std::string& s);
};

#endif
