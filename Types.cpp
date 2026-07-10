#include "Types.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <cassert>

void PosList::add(const Pos& m){
    this -> push_back(m);
}
void PosList::pop(){
    assert(!this->empty());
    this -> pop_back();
}

bool BitBoard::g(const i16 i, const i16 j) const {
    if(i < 0 || i >= DIM || j < 0 || j >= DIM) return false;
    return this -> test(spla(i, j));
}
void BitBoard::s(const i16 i, const i16 j, const bool v){
    if(i < 0 || i >= DIM || j < 0 || j >= DIM) return;
    this -> set(spla(i, j), v);
}

bool BitBoard::g(const Pos& pos) const {
    return g(pos.i, pos.j);
}
void BitBoard::s(const Pos& pos, const bool v){
    s(pos.i, pos.j, v);
}

void BitBoard::clear(){
    this -> reset();
}

void BitBoard::find_ones(PosList& l){
    l.add({i16(this -> _Find_first()), 0});
    auto size = this -> count();
    while(l.size() < size){
        l.add({i16(this -> _Find_next(l.back().i)), 0});
    }
    for(auto& p : l){
        p = rozs(p.i);
    }
}
void History::add(const HistoryEntry& m){
    this -> push_back(m);
}
void History::pop(){
    assert(!this->empty());
    this -> pop_back();
}

Zobrist::Zobrist(){
    std::fill(z, z + 2, 0);
}

bool Zobrist::operator==(const Zobrist& other) const {
    return z[0] == other.z[0] && z[1] == other.z[1];
}
u64& Zobrist::operator[](const int i){
    return z[i];
}
const u64& Zobrist::operator[](const int i) const {
    return z[i];
}
