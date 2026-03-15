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
    std::string name;
    bool isAcceptable = false;
    explicit State(std::string name):name(std::move(name)){};
};


#endif //LAB_NUM2_STATE_H