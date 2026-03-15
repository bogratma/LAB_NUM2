//
// Created by vlad on 3/15/26.
//

#ifndef LAB_NUM2_CHUNK_H
#define LAB_NUM2_CHUNK_H
#include "State.h"

struct Chunk {
    State* start;
    State* end;
    Chunk(State* start, State* end): start(start), end(end) {};
};
#endif //LAB_NUM2_CHUNK_H