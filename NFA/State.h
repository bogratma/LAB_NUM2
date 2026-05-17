//
// Created by vlad on 3/14/26.
//

#ifndef LAB_NUM2_STATE_H
#define LAB_NUM2_STATE_H
#include <map>
#include <string>
#include <utility>
#include <vector>

class State;

struct Tag {
    int reg;
};
struct Eps {
    State* to;
    int prior = 0;
    std::vector<Tag> tags;
};
class State {
    public:
    static int name;
    int id;
    bool isAcceptable = false;
    std::vector<Eps> epsTrans;
    State(){id = name++;}
};


#endif //LAB_NUM2_STATE_H