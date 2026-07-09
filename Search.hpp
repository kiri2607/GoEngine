#pragma once
#include "Types.hpp"
#include "Board.hpp"
#include "Random.hpp"

struct MCTSNode{
    float n = 0, wwin = 0;
    i32 p = -1;
    std::vector<i32> chil;
    PosList legal;
    float winrate(bool col) const {
        if(col) return wwin / n;
        return 1 - wwin / n;
    }
};
struct SelectResult{
    i32 ind;
    float res;
};
struct Search{
    std::vector<MCTSNode> tree;
    Rand random;
    bool start_col;
    Board brd;
    Board start_brd;
    Search(bool c, const Board& b);
    float Rollout(Board& b, bool c);
    float uct1(const MCTSNode& p, const MCTSNode& v, bool pcol) const;
    i32 bestuct(i32 g, bool col);
    SelectResult Selection();
    void Backprop(const SelectResult& sres);
    void Cycle();
    void find_in_sub_tree(i32 g, std::vector<MCTSNode>& new_tree);  
    void make_move(const Pos& pos);
    Pos& get_best_move();
};