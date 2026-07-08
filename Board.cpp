#include "Board.hpp"

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

inline i8 spla(i8 i, i8 j){
    return i * DIM + j;
}
inline Pos rozs(i8 i){
    return {static_cast<i8>(i / DIM), static_cast<i8>(i % DIM)};
}
void BitSet::clear(){
    std::fill(tab, tab + BITSETSZ, 0);
}
BitSet::BitSet(){
    clear();
}
u8 BitSet::g(const i8 i) const {
    assert(i >= 0 && i < BITSETSZ * 8);
    return bool(tab[i / INTSZ] & (1 << (i % INTSZ)));
}
void BitSet::s(const i8 i, const bool v){
    assert(i >= 0 && i < BITSETSZ * 8);
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

u8 BitBoard::g(const Pos& pos) const {
    const i8 sp = spla(pos.i, pos.j);
    return BitSet::g(sp);
}
void BitBoard::s(const Pos& pos, const bool v){
    const i8 sp = spla(pos.i, pos.j);
    BitSet::s(sp, v);
}

void BitBoard::find_ones(PosList& l){
    for(int i = 0; i < BITSETSZ; i++){
        if(tab[i] == 0) continue;
        u8 kopia = tab[i];
        while(kopia != 0){
            auto g = std::__lg(kopia);
            l.add(rozs(INTSZ * i + g));
            kopia ^= (1 << g);
        }
    }
}

void Board::set_komi(float k){
    komi = k;
}

Board::Board(float k){
    set_komi(k);
    hist_hash.s(hash, true);
}

PosList Board::find_group(const Pos& pos, BitBoard& vis){
    static std::queue<Pos> q;
    static PosList odp;

    assert(pos.in_bounds());
    assert(brd[0].g(pos) || brd[1].g(pos));
    if(vis.g(pos)) return {};
    while(!q.empty()) q.pop();
    odp.clear();

    q.push(pos);
    bool c = brd[1].g(pos);
    vis.s(pos, 1);

    while(!q.empty()){
        auto s = q.front();
        q.pop();
        odp.add(s);
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(vis.g(sa) || !brd[c].g(sa)) continue;
            vis.s(sa, 1);
            q.push(sa);
        }
    }
    return odp;
}

PosList Board::find_group(const Pos& pos){
    static BitBoard vis;
    vis.clear();
    return find_group(pos, vis);
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

i8 Board::get_air(const Pos& pos, BitBoard& vis){
    assert(pos.in_bounds());

    i8 odp = 4;
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds() || vis.g(posa) || brd[0].g(posa) | brd[1].g(posa)){
            odp--;
            continue;
        }
        vis.s(posa, 1);
    }
    return odp;
}


i8 Board::get_air(const Pos& pos){
    assert(pos.in_bounds());
    i8 odp = 4;
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds() || brd[0].g(posa) | brd[1].g(posa)){
            odp--;
        }
    }
    return odp;
} 

i8 Board::get_group_air(const PosList& group, BitBoard& vis) {
    i8 odp = 0;
    for(auto pos : group){
        assert(pos.in_bounds());
        odp += get_air(pos, vis);
    }
    return odp;
}

i8 Board::get_group_air(const PosList& group) {
    static BitBoard vis;
    vis.clear();
    return get_group_air(group, vis);
}


void Board::revert_stone(const Pos& pos, bool col){
    assert(pos.in_bounds());
    brd[col].s(pos, !brd[col].g(pos));
    hash[0] ^= zobrist[0][col][pos.i][pos.j];
    hash[1] ^= zobrist[1][col][pos.i][pos.j];
}


std::vector<PosList> Board::find_killed_groups(const Pos& pos, bool col){
    static PosList group;
    static std::vector<PosList> groups;
    static BitBoard vis;
    vis.clear();
    group.clear();
    groups.clear();
    for(const auto& dif : adjc){
        const Pos posa = pos + dif;
        if(!posa.in_bounds()) continue;
        if(!brd[!col].g(posa)) continue;
        if(vis.g(posa)) continue;
        group = find_group(posa, vis);
        if(get_group_air(group) == 0){
            groups.push_back(group);
        }
    }
    return groups;
}

bool Board::is_legal(const Pos& pos, bool col){
    static std::vector<PosList> killed_groups;
    static BitBoard vis;
    
    if(pos == PASS) return true;
    assert(pos.in_bounds());
    vis.clear();
    if(brd[0].g(pos) || brd[1].g(pos)) return false;
    killed_groups.clear();
    revert_stone(pos, col);

    killed_groups = find_killed_groups(pos, col);

    if(killed_groups.size() == 0 && get_group_air(find_group(pos, vis)) == 0){
        revert_stone(pos, col);
        return false;
    }

    for(auto g : killed_groups){
        for(auto gpos : g){
            revert_stone(gpos, !col);
        }
    }

    bool odp = false;
    if(!hist_hash.g(hash)) odp = true;
    for(auto g : killed_groups){
        for(auto gpos : g){
            revert_stone(gpos, !col);
        }
    }

    revert_stone(pos, col);
    return odp;
}


void Board::make_move(const Pos& pos, bool col){
    static std::vector<PosList> killed_groups;
    static std::array<PosList, 2> changed_stones;
    if(pos == PASS){
        passes++;
        hist.add({});
        return;
    }
    assert(pos.in_bounds());
    assert(!brd[0].g(pos) && !brd[1].g(pos));
    
    killed_groups.clear();
    changed_stones[0].clear();
    changed_stones[1].clear();
    revert_stone(pos, col);
    changed_stones[col].add(pos);
    killed_groups = find_killed_groups(pos, col);
    for(auto g : killed_groups){
        for(auto gpos : g){
            revert_stone(gpos, !col);
            changed_stones[!col].add(gpos);
        }
    }
    hist.add(changed_stones);
    hist_hash.s(hash, 1);
}

void Board::unmake_move(){
    assert(!hist.empty());
    if(hist.back()[0].empty() && hist.back()[1].empty()){
        passes--;
        hist.pop();
        return;
    }
    for(int c = 0; c < 2; c++){
        for(auto move : hist.back()[c]){
            revert_stone(move, c);
            
        }
    }
    hist.pop();
    passes = (!hist.empty() && hist.back()[0].empty() && hist.back()[1].empty());
    hist_hash.s(hash, 0);
}

void Board::filter_legal(PosList& poss, bool col){
    static PosList odp;
    odp.clear();
    for(const auto& move : poss){
        if(is_legal(move, col)) odp.add(move);
    }
    poss = odp;
}

void Board::filter_pos(PosList& poss, PosList& fil){
    static PosList odp;
    odp.clear();
    std::sort(poss.begin(), poss.end());
    std::sort(fil.begin(), fil.end());
    std::set_intersection(poss.begin(), poss.end(), fil.begin(), fil.end(), std::back_inserter(odp.list));
    poss = odp;
}

PosList Board::get_with_air_count(bool col, i8 cnt){
    static BitBoard vis;
    static BitBoard air;
    static PosList odp;
    vis.clear();
    odp.clear();
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            if(vis.g(i, j) || !brd[col].g(i, j)) continue;
            air.clear();
            i8 gair = get_group_air(find_group({i, j}, vis), air);
            if(gair == cnt){
                air.find_ones(odp);
            }
        }
    }
    std::sort(odp.begin(), odp.end());
    odp.list.erase(std::unique(odp.begin(), odp.end()), odp.end());
    return odp;
}

PosList Board::get_with_air_count(bool col, i8 cnt, const PosList& poss){
    static PosList odp;
    static PosList kopia;
    kopia = poss;
    odp = get_with_air_count(col, cnt);
    filter_pos(odp, kopia);
    return odp;
}

PosList Board::get_pseudo_legal_moves(bool col){
    static PosList odp;
    static PosList myata;
    static PosList opata;
    myata = get_with_air_count(col, 1);
    opata = get_with_air_count(!col, 1);
    std::sort(myata.begin(), myata.end(), std::greater<Pos>());
    std::sort(opata.begin(), opata.end(), std::greater<Pos>());
    odp.clear();
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
            while(!myata.empty() && myata.back() < Pos{i, j}) myata.pop();
            while(!opata.empty() && opata.back() < Pos{i, j}) opata.pop();
            if(brd[0].g(i, j) || brd[1].g(i, j)) continue;
            if((!myata.empty() && myata.back() == Pos{i, j} && (opata.empty() || opata.back() != Pos{i, j})) || ((opata.empty() || opata.back() != Pos{i, j}) && is_eye({i, j}, !col))){
                continue;
            }
            odp.add({i, j});
        }
    }
    odp.add(PASS);
    return odp;
}


PosList Board::get_legal_moves(bool col){
    static PosList pseudo;
    pseudo = get_pseudo_legal_moves(col);
    filter_legal(pseudo, col);
    return pseudo;
}


float Board::end_game_result(){
    static BitBoard scr[2];
    static std::queue<Pos> q;
    while(!q.empty()) q.pop();
    scr[0].clear();
    scr[1].clear();
    
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
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(scr[0].g(sa) || brd[1].g(sa)) continue;
            scr[0].s(sa, 1);
            q.push({sa});
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
        for(const auto& dif : adjc){
            const Pos sa = s + dif;
            if(!sa.in_bounds()) continue;
            if(scr[1].g(sa) || brd[0].g(sa)) continue;
            scr[1].s(sa, 1);
            q.push({sa});
        }
    }

    float score[2] = {0, komi};
    for(i8 i = 0; i < DIM; i++){
        for(i8 j = 0; j < DIM; j++){
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