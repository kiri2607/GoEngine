#pragma once
#include "Types.hpp"
#include "Board.hpp"
#include "Random.hpp"

class Search{
    struct MCTSNode{
        float wins[2] = {0.0, 0.0};
        std::vector<int> chil;
    };
    std::vector<MCTSNode> tree;
    Rand random;
    float Rollout(Board& b, bool c);

};