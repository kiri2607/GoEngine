#include <bitset>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <array>
#include <vector>
#include <queue>
#include <utility>
#include <iostream>
#include <random>
#include "Types.hpp"
#include "Board.hpp"
inline i8 spla(i8 i, i8 j){
    return i * DIM + j;
}

inline bool ij_in_bounds(i8 i, i8 j){
    return i >= 0 && i < DIM && j >= 0 && j < DIM;
}


void BitSet::clear(){
    std::fill(tab, tab + BITSETSZ, 0);
}
BitSet::BitSet(){
    clear();
}
u8 BitSet::g(const i8 i) const {
    assert(i >= 0 && i < BITSETSZ);
    return bool(tab[i / INTSZ] & (1 << (i % INTSZ)));
}
void BitSet::s(const i8 i, const bool v){
    assert(i >= 0 && i < BITSETSZ);
    tab[i / INTSZ] &= ~(1 << (i % INTSZ));
    tab[i / INTSZ] |= (1 << (i % INTSZ)) * v;
}
bool BitSet::operator==(const BitSet& other) const {
    for(i8 i = 0; i < BITSETSZ; i++){
        if(tab[i] != other.tab[i]) return false;
    }
    return true;
}

BitBoard::BitBoard(): BitSet(){}

u8 BitBoard::g(const i8 i, const i8 j) const {
    const i8 sp = spla(i, j);
    return BitSet::g(sp);
}
void BitBoard::s(const i8 i, const i8 j, const bool v){
    const i8 sp = spla(i, j);
    BitSet::s(sp, v);
}


Board::Board(float komi){
    score[1] = komi;
    score[0] = 0;
    hash[0][0] = 0;
    hash[0][1] = 0;
    hash[1][0] = 0;
    hash[1][1] = 0;
    hist_hash[0] = true;
}
PosList Board::find_group(const i8 i, const i8 j){
    static std::queue<Pos> q;
    static PosList odp;

    assert(ij_in_bounds(i, j));
    assert(brd[0].g(i, j) || brd[1].g(i, j));
    while(!q.empty()) q.pop();
    odp.clear();
    vis.clear();

    q.push({i, j});
    bool c = brd[1].g(i, j);
    vis.s(i, j, 1);

    while(!q.empty()){
        auto s = q.front();
        q.pop();
        odp.add({s.i, s.j});
        for(auto [x, y] : adjc){
            i8 ia = s.i + x, ja = s.j + y;
            if(!ij_in_bounds(ia, ja)) continue;
            if(vis.g(ia, ja) || !brd[c].g(ia, ja)) continue;
            vis.s(ia, ja, 1);
            q.push({ia, ja});
        }
    }
    return odp;
}
i8 Board::get_air(const i8 i, const i8 j){
    assert(ij_in_bounds(i, j));

    i8 odp = 4;
    for(auto [x, y] : adjc){
        i8 ia = i + x, ja = j + y;
        if(!ij_in_bounds(ia, ja)){
            odp--;
            continue;
        }
        odp -= brd[0].g(ia, ja) | brd[1].g(ia, ja);
    }
    return odp;
}

i8 Board::get_group_air(const PosList& group) {
    i8 odp = 0;
    for(auto [i, j] : group){
        assert(ij_in_bounds(i, j));
        odp += get_air(i, j);
    }
    return odp;
}


void Board::revert_stone(const i8 i, const i8 j, bool col){
    assert(ij_in_bounds(i, j));
    brd[col].s(i, j, !brd[col].g(i, j));
    hash[col][0] ^= zobrist[0][i][j];
    hash[col][1] ^= zobrist[1][i][j];
}


std::vector<PosList> Board::find_killed_groups(const i8 i, const i8 j, bool col){
    static PosList group;
    static std::vector<PosList> groups;
    group.clear();
    groups.clear();
    for(int k = 0; k < 4; k++){
        const i8 ia = i + adjc[k].i, ja = j + adjc[k].j;
        if(!ij_in_bounds(ia, ja)) continue;
        if(!brd[!col].g(ia, ja)) continue;
        group = find_group(ia, ja);
        if(get_group_air(group) == 0){
            for(auto [x, y] : group){
                brd[!col].s(x, y, 0);
            }
            groups.push_back(group);
        }
    }
    for(auto g : groups){
        for(auto [x, y] : g){
            brd[!col].s(x, y, 1);
        }
    }
    return groups;
}

bool Board::is_legal(const i8 i, const i8 j, bool col){
    static std::vector<PosList> killed_groups;

    assert(ij_in_bounds(i, j));
    
    if(brd[0].g(i, j) || brd[1].g(i, j)) return false;
    killed_groups.clear();
    revert_stone(i, j, col);

    killed_groups = find_killed_groups(i, j, col);

    if(killed_groups.size() == 0 && get_group_air(find_group(i, j)) == 0){
        revert_stone(i, j, col);
        return false;
    }

    for(auto g : killed_groups){
        for(auto [x, y] : g){
            brd[!col].s(x, y, 0);
            hash[!col][0] ^= zobrist[0][x][y];
            hash[!col][1] ^= zobrist[1][x][y];
        }
    }

    bool odp = false;
    if(memcmp(hash, pre_hash, sizeof(hash)) != 0) odp = true;
    for(auto g : killed_groups){
        for(auto [x, y] : g){
            brd[!col].s(x, y, 1);
            hash[!col][0] ^= zobrist[0][x][y];
            hash[!col][1] ^= zobrist[1][x][y];
        }
    }

    revert_stone(i, j, col);
    return odp;
}


i8 Board::make_move(const i8 i, const i8 j, bool col){
    static std::vector<PosList> killed_groups;
    static std::array<PosList, 2> changed_stones;

    assert(ij_in_bounds(i, j));
    assert(brd[0].g(i, j) || brd[1].g(i, j));

    killed_groups.clear();
    changed_stones[0].clear();
    changed_stones[1].clear();
    std::memcpy(pre_hash, hash, sizeof(hash));
    revert_stone(i, j, col);
    changed_stones[col].add({i, j});
    killed_groups = find_killed_groups(i, j, col);
    i8 odp = 0;
    for(auto g : killed_groups){
        odp += g.size();
        for(auto [x, y] : g){
            revert_stone(x, y, !col);
            changed_stones[!col].add({x, y});
        }
    }
    hist.add(changed_stones);
    score[col] += odp;
    return odp;
}

i8 Board::unmake_move(){
    assert(!hist.empty());
    for(int c = 0; c < 2; c++){
        for(auto move : hist.back()[c]){
            revert_stone(move.i, move.j, c);
        }
    }
    hist.pop();
    if(!hist.empty()){
        for(int c = 0; c < 2; c++){
            for(auto move : hist.back()[c]){
                pre_hash[c][0] ^= zobrist[0][move.i][move.j];
                pre_hash[c][1] ^= zobrist[1][move.i][move.j];
            }
        }
    }
}

PosList Board::get_legal_moves(bool col){
    static PosList odp;
    odp.clear();
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            if(is_legal(i, j, col)) odp.add({i, j});
        }
    }
    return odp;
}

float Board::end_game_result(){
    static BitBoard scr[2];
    scr[0].clear();
    scr[1].clear();
    std::queue<Pos> q;
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            if(brd[0].g(i, j)){
                q.push({i, j});
                scr[0].s(i, j, 1);
            }
        }
    }
    while(!q.empty()){
        auto s = q.front();
        q.pop();
        for(auto [x, y] : adjc){
            i8 ia = s.i + x, ja = s.j + y;
            if(!ij_in_bounds(ia, ja)) continue;
            if(scr[0].g(ia, ja) || brd[1].g(ia, ja)) continue;
            scr[0].s(ia, ja, 1);
            q.push({ia, ja});
        }
    }
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            if(brd[1].g(i, j)){
                q.push({i, j});
                scr[1].s(i, j, 1);
            }
        }
    }
    while(!q.empty()){
        auto s = q.front();
        q.pop();
        for(auto [x, y] : adjc){
            i8 ia = s.i + x, ja = s.j + y;
            if(!ij_in_bounds(ia, ja)) continue;
            if(scr[1].g(ia, ja) || brd[0].g(ia, ja)) continue;
            scr[1].s(ia, ja, 1);
            q.push({ia, ja});
        }
    }
    float end_game_score[2];
    memcpy(end_game_score, score, sizeof(score));
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            if(scr[0].g(i, j) && scr[1].g(i, j)){
                continue;
            }
            if(scr[0].g(i, j)){
                end_game_score[0] += !brd[0].g(i, j);
            }
            if(scr[1].g(i, j)){
                end_game_score[1] += !brd[1].g(i, j);
            }
        }
    }
    if(end_game_score[0] == end_game_score[1]) return 0.5;
    else if(end_game_score[1] > end_game_score[0]) return 1;
    else return 0;
}

BitBoard* Board::get_board(){
    return brd;
}


void print_bitboard(const BitBoard* b){
    for(int i = 0; i < DIM; i++){
        for(int j = 0; j < DIM; j++){
            if(b[0].g(i, j)) std::cerr << 'X';
            else if(b[1].g(i, j)) std::cerr << '#';
            else std::cerr << '.';
        }
        std::cerr << '\n';
    }
}