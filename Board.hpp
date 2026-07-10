#pragma once

#include "Types.hpp"
#include "DataStruct.hpp"
#include "Random.hpp"
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



struct Board{

    std::array<BitBoard, 2> brd;
    BitBoard vis;
    BitBoard air;
    Zobrist hash;
    HashTable<bool> hist_hash;
    float komi;
    i16 passes = 0;
    void set_komi(float k); // set komi to 'k'
    Board(float k = 7.5); // construct
    void clear();
    void find_group(PosList& buff, const Pos& pos); // find a group of 'pos' and store its position in 'vis'
    bool is_eye(const Pos& pos, bool col); // is 'pos' an eye for 'col'
    i16 get_air(const Pos& pos); // get air count for positions 'pos' but use 'vis' as a visited table
    i16 get_group_air(const PosList& group); // get air count for a group 'group', and store all air positions in 'vis'
    void revert_stone(const Pos& pos, bool col); // if on 'pos' wasnt a stone, then place a stone with color 'col', else if there was a stone with color 'col' remove a stone
    void find_killed_stones(PosList& buff, const Pos& pos, bool col); // find all groups with color !col that were killed after placing a stone with color 'col' at 'pos'
    bool is_legal(const Pos& pos, bool col); // check if a move is legal
    void make_move(const Pos& pos, bool col); // make a move
    void filter_legal(PosList& poss, bool col); // removes all moves in that are not legal for 'col'
    void get_with_air_count(PosList& buff, bool col, i16 cnt); // sorted list of spots of air of groups that have 'cnt' spots of air
    void get_legal_moves(PosList& buff, bool col); // get all legal moves for col in current board state
    Pos get_random_legal(bool col, Rand&);
    float end_game_result(); // if the game has ended, what is the result (0, 0.5, 1)
    bool games_end() const; // has the game ended?
    std::array<BitBoard, 2> get_board(); // get boards state
    
};

void print_bitboard(const std::array<BitBoard, 2> b);
