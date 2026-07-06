#pragma once

#include "Types.hpp"
#include <bitset>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <array>
#include <vector>
#include <queue>
#include <utility>
#include <iostream>
#include <cstring>
#include <unordered_map>

inline i8 spla(i8 i, i8 j);
inline bool ij_in_bounds(i8 i, i8 j);
struct BitSet{
    u8 tab[BITSETSZ];
    void clear();
    BitSet();
    u8 g(const i8 i) const;
    void s(const i8 i, const bool v);
    bool operator==(const BitSet& other) const;
};

struct BitBoard: public BitSet{
    BitBoard();
    u8 g(const i8 i, const i8 j) const;
    void s(const i8 i, const i8 j, const bool v);
};

class Board{

    BitBoard brd[2];
    BitBoard vis;
    u64 hash[2][2]; // col, num
    std::unordered_map<u64, bool> hist_hash;
    History hist;
    public:
    float score[2];
    
    Board(float komi = 6.5);

    PosList find_group(const i8 i, const i8 j);

    i8 get_group_air(const PosList& group);

    i8 get_air(const i8 i, const i8 j);
    
    void revert_stone(const i8 i, const i8 j, bool col);

    std::vector<PosList> find_killed_groups(const i8 i, const i8 j, bool col);

    bool is_legal(const i8 i, const i8 j, bool col);

    i8 make_move(const i8 i, const i8 j, bool col);

    i8 unmake_move();

    PosList get_legal_moves(bool col);

    BitBoard* get_board();

    float end_game_result();
    
};

void print_bitboard(const BitBoard* b);
