//
// Created by vlad on 3/14/26.
//

#ifndef LAB_NUM2_STATE_H
#define LAB_NUM2_STATE_H
#include <map>
#include <string>
#include <utility>


class State {
    public:
    static int name;
    int id;
    bool isAcceptable = false;
    State(){id = name++;}
};


#endif //LAB_NUM2_STATE_H