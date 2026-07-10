#include "Board.hpp"
#include "Random.hpp"
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
#include <stack>



void Board::set_komi(float k){
    komi = k;
}

Board::Board(float k){
    set_komi(k);
    hist_hash.s(hash, true);
}

void Board::clear(){
    vis.clear();
    air.clear();
    hash = Zobrist();
    hist_hash.clear();
    hist_hash.s(hash, true);
    brd[0].clear();
    brd[1].clear();
}

void Board::find_group(PosList& buff, const Pos& pos){
    static PosList stack = []() {
        PosList p;
        p.reserve(DIM * DIM + 1);
        return p;
    }();
    buff.clear();
    assert(pos.in_bounds());
    assert(brd[0].g(pos) || brd[1].g(pos));
    stack.clear();

    stack.add(pos);
    bool c = brd[1].g(pos);
    vis.s(pos, 1);

    while(!stack.empty()){
        auto s = stack.back();
        stack.pop();
        buff.add(s);
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(vis.g(sa) || !brd[c].g(sa)) continue;
            vis.s(sa, 1);
            stack.add(sa);
        }
    }
}

bool Board::is_eye(const Pos& pos, bool col){
    assert(pos.in_bounds());
    if(brd[0].g(pos) || brd[1].g(pos)) return false;
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds()) continue;
        if(!brd[col].g(posa)) return false;
    }
    return true;
}

i16 Board::get_air(const Pos& pos){
    assert(pos.in_bounds());

    i16 odp = 4;
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds() || air.g(posa) || brd[0].g(posa) || brd[1].g(posa)){
            odp--;
            continue;
        }
        air.s(posa, 1);
    }
    return odp;
}

i16 Board::get_group_air(const PosList& group) {
    i16 odp = 0;
    air.clear();
    for(auto pos : group){
        assert(pos.in_bounds());
        odp += get_air(pos);
    }
    return odp;
}


void Board::revert_stone(const Pos& pos, bool col){
    assert(pos.in_bounds());
    brd[col].s(pos, !brd[col].g(pos));
    hash[0] ^= zobrist[0][col][pos.i][pos.j];
    hash[1] ^= zobrist[1][col][pos.i][pos.j];
}


void Board::find_killed_stones(PosList& buff, const Pos& pos, bool col){
    buff.clear();
    static PosList group;
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds()) continue;
        if(!brd[!col].g(posa)) continue;
        if(vis.g(posa)) continue;
        find_group(group, posa);
        if(get_group_air(group) == 0){
            buff.insert(buff.end(), group.begin(), group.end());
        }
    }
}

bool Board::is_legal(const Pos& pos, bool col){
    static PosList killed_stones;
    static PosList group;
    group.clear();
    if(pos == PASS) return true;
    assert(pos.in_bounds());
    vis.clear();
    if(brd[0].g(pos) || brd[1].g(pos)) return false;
    killed_stones.clear();
    revert_stone(pos, col);

    find_killed_stones(killed_stones, pos, col);
    find_group(group, pos);
    if(killed_stones.size() == 0 && get_group_air(group) == 0){
        revert_stone(pos, col);
        return false;
    }

    for(const auto& g : killed_stones){
        revert_stone(g, !col);
    }

    bool odp = false;
    if(!hist_hash.g(hash)) odp = true;
    for(const auto& g : killed_stones){
        revert_stone(g, !col);
    }

    revert_stone(pos, col);
    return odp;
}


void Board::make_move(const Pos& pos, bool col){
    static PosList killed_stones;
    if(pos == PASS){
        passes++;
        return;
    }
    assert(pos.in_bounds());
    assert(!brd[0].g(pos) && !brd[1].g(pos));
    hist_hash.s(hash, 1);

    passes = 0;
    
    killed_stones.clear();
    revert_stone(pos, col);
    vis.clear();
    find_killed_stones(killed_stones, pos, col);
    for(const auto& g : killed_stones){
        revert_stone(g, !col);
    }
}

void Board::filter_legal(PosList& poss, bool col){
    i32 size = 0;
    for(i32 i = 0; i < i32(poss.size()); i++){
        if(is_legal(poss[i], col)){
            poss[size] = poss[i];
            size++;
        }
    }
    poss.resize(size);
}



void Board::get_with_air_count(PosList& buff, bool col, i16 cnt){
    static PosList group;
    buff.clear();
    vis.clear();
    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(vis.g(i, j) || !brd[col].g(i, j)) continue;
            group.clear();
            find_group(group, {i, j});
            i16 gair = get_group_air(group);
            if(gair == cnt){
                air.find_ones(buff);
            }
        }
    }
    std::sort(buff.begin(), buff.end());
    buff.erase(std::unique(buff.begin(), buff.end()), buff.end());
}


void Board::get_legal_moves(PosList& buff, bool col){
    buff.clear();
    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(brd[0].g(i, j) || brd[1].g(i, j)) continue;
            if(is_legal({i, j}, col)) buff.add({i, j});
        }
    }
    buff.add(PASS);
}

Pos Board::get_random_legal(bool col, Rand& r){
    static PosList freespots = []() {
        PosList p;
        p.reserve(DIM * DIM + 1);
        return p;
    }();
    freespots.clear();
    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(!brd[0].g(i, j) && !brd[1].g(i, j)) freespots.add({i, j});
        }
    }
    freespots.add(PASS);
    while(true){
        i16 i = r.rand<i16>(0, i16(freespots.size()) - 1);
        if(is_legal(freespots[i], col)) return freespots[i];
    }
}

float Board::end_game_result(){
    static BitBoard scr[2];
    static PosList stack = []() {
        PosList p;
        p.reserve(DIM * DIM + 1);
        return p;
    }();
    stack.clear();
    scr[0].clear();
    scr[1].clear();
    
    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(brd[0].g(i, j)){
                stack.add({i, j});
                scr[0].s(i, j, 1);
            }
        }
    }
    while(!stack.empty()){
        auto s = stack.back();
        stack.pop();
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(scr[0].g(sa) || brd[1].g(sa)) continue;
            scr[0].s(sa, 1);
            stack.add({sa});
        }
    }

    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(brd[1].g(i, j)){
                stack.add({i, j});
                scr[1].s(i, j, 1);
            }
        }
    }
    while(!stack.empty()){
        auto s = stack.back();
        stack.pop();
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(scr[1].g(sa) || brd[0].g(sa)) continue;
            scr[1].s(sa, 1);
            stack.add({sa});
        }
    }

    float score[2] = {0, komi};
    for(i16 i = 0; i < DIM; i++){
        for(i16 j = 0; j < DIM; j++){
            if(scr[0].g(i, j) && scr[1].g(i, j)) continue;
            else if(scr[0].g(i, j)) score[0]++;
            else if(scr[1].g(i, j)) score[1]++;
        }
    }
    if(score[0] == score[1]) return 0.5;
    else if(score[1] > score[0]) return 1;
    else return 0;
}

bool Board::games_end() const {
    return passes >= 2;
}

std::array<BitBoard, 2> Board::get_board(){
    return brd;
}

void print_bitboard(const std::array<BitBoard, 2> b){
    for(int i = 0; i < DIM; i++){
        for(int j = 0; j < DIM; j++){
            if(b[0].g(i, j)) std::cerr << 'X';
            else if(b[1].g(i, j)) std::cerr << '#';
            else std::cerr << '.';
        }
        std::cerr << '\n';
    }
}