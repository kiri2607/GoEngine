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
inline i8 rozs(i8 i, i8 j);
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
    u8 g(const Pos& pos) const;
    void s(const i8 i, const i8 j, const bool v);
    void s(const Pos& pos, const bool v);
    void find_ones(PosList& l); 
};

struct Board{

    BitBoard brd[2];
    Zobrist hash;
    HashTable<bool> hist_hash;
    History hist;
    float komi;
    i8 passes = 0;
    Board(float k = 6.5);
    PosList find_group(const Pos& pos, BitBoard& vis);
    PosList find_group(const Pos& pos);
    bool is_eye(const Pos& pos, bool col);
    i8 get_air(const Pos& pos, BitBoard& vis);
    i8 get_air(const Pos& pos);
    i8 get_group_air(const PosList& group, BitBoard& vis);
    i8 get_group_air(const PosList& group);
    void revert_stone(const Pos& pos, bool col);
    std::vector<PosList> find_killed_groups(const Pos& pos, bool col);
    bool is_legal(const Pos& pos, bool col);
    void make_move(const Pos& pos, bool col);
    void unmake_move();
    void filter_legal(PosList& poss, bool col);
    PosList get_pseudo_legal_moves(bool col);
    PosList get_legal_moves(bool col);
    PosList get_with_air_count(bool col, i8 cnt);
    PosList get_with_air_count(bool col, i8 cnt, const PosList& poss);
    float end_game_result();
    bool games_end() const;
    BitBoard* get_board();
    
};

void print_bitboard(const BitBoard* b);
