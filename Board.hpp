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
    void set_komi(float k); // set komi to 'k'
    Board(float k = 6.5); // construct
    PosList find_group(const Pos& pos, BitBoard& vis); // find a group of 'pos' and store its position in 'vis'
    PosList find_group(const Pos& pos); // find a group of 'pos'
    bool is_eye(const Pos& pos, bool col); // is 'pos' an eye for 'col'
    i8 get_air(const Pos& pos, BitBoard& vis); // get air count for positions 'pos' but use 'vis' as a visited table
    i8 get_air(const Pos& pos); // get air count for position 'pos'
    i8 get_group_air(const PosList& group, BitBoard& vis); // get air count for a group 'group', and store all air positions in 'vis'
    i8 get_group_air(const PosList& group); // get air count for a group 'group'
    void revert_stone(const Pos& pos, bool col); // if on 'pos' wasnt a stone, then place a stone with color 'col', else if there was a stone with color 'col' remove a stone
    std::vector<PosList> find_killed_groups(const Pos& pos, bool col); // find all groups with color !col that were killed after placing a stone with color 'col' at 'pos'
    bool is_legal(const Pos& pos, bool col); // check if a move is legal
    void make_move(const Pos& pos, bool col); // make a move
    void unmake_move(); // unmake a last move
    void filter_legal(PosList& poss, bool col); // removes all moves in that are not legal for 'col'
    void filter_pos(PosList& poss, PosList& fil); // intersection of poss and fil
    PosList get_pseudo_legal_moves(bool col); // get all moves that are legal without the Ko rule
    PosList get_legal_moves(bool col); // get all legal moves for col in current board state
    PosList get_with_air_count(bool col, i8 cnt); // sorted list of spots of air of groups that have 'cnt' spots of air
    PosList get_with_air_count(bool col, i8 cnt, const PosList& poss); // 'get_with_air_count(bool, i8) but posistins are filtered using poss
    float end_game_result(); // if the game has ended, what is the result (0, 0.5, 1)
    bool games_end() const; // has the game ended?
    BitBoard* get_board(); // get boards state
    
};

void print_bitboard(const BitBoard* b);
