//
// Created by vlad on 3/29/26.
//

#ifndef LAB_NUM2_PARSERRESULT_H
#define LAB_NUM2_PARSERRESULT_H
class Node;
struct ParserResult {
    std::unique_ptr<Node> mainTree;
    std::string lookahead;
    bool hasLookahead = false;
    bool hasCapGroup = false;
    int groupCount = 0;
};
#endif //LAB_NUM2_PARSERRESULT_H