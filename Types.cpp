#include "Types.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <cassert>

PosList::PosList(const std::vector<Pos> l): list(l) {}
std::vector<Pos>::const_iterator PosList::begin() const {
    assert(!list.empty());
    return list.begin();
}
std::vector<Pos>::const_iterator PosList::end() const {
    return list.end();
}
std::vector<Pos>::iterator PosList::begin(){
    return list.begin();
}
std::vector<Pos>::iterator PosList::end(){
    return list.end();
}
Pos& PosList::front(){
    assert(!list.empty());
    return list.front();
}
Pos& PosList::back(){
    assert(!list.empty());
    return list.back();
}
bool PosList::empty() const {
    return list.empty();
}
size_t PosList::size() const {
    return list.size();
}
void PosList::add(const Pos& m){
    list.push_back(m);
}
void PosList::pop(){
    assert(!list.empty());
    list.pop_back();
}
Pos& PosList::operator[](const int i){
    assert(i >= 0 && size_t(i) < size());
    return list[i];
}
const Pos& PosList::operator[](const int i) const {
    assert(i >= 0 && size_t(i) < size());
    return list[i];
}
void PosList::clear(){
    list.clear();
}



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





History::History(const std::vector<HistoryEntry> l): list(l) {}
std::vector<HistoryEntry>::const_iterator History::begin() const {
    return list.begin();
}
std::vector<HistoryEntry>::const_iterator History::end() const {
    return list.end();
}
std::vector<HistoryEntry>::iterator History::begin() {
    assert(!list.empty());
    return list.begin();
}
std::vector<HistoryEntry>::iterator History::end() {
    return list.end();
}
HistoryEntry& History::front(){
    assert(!list.empty());
    return list.front();
}
HistoryEntry& History::back(){
    assert(!list.empty());
    return list.back();
}
bool History::empty() const {
    return list.empty();
}
size_t History::size() const {
    return list.size();
}
void History::add(const HistoryEntry& m){
    list.push_back(m);
}
void History::pop(){
    assert(!list.empty());
    list.pop_back();
}
void History::clear(){
    list.clear();
}
HistoryEntry& History::operator[](const int i){
    assert(i >= 0 && size_t(i) < size());
    return list[i];
}
const HistoryEntry& History::operator[](const int i) const {
    assert(i >= 0 && size_t(i) < size());
    return list[i];
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


template<typename T>
void HashTable<T>::clear(){
    std::fill(stpos.begin(), stpos.end(), -1);
    nodes.clear();
}
template<typename T>
HashTable<T>::HashTable(){
    stpos.resize(HASHTABLESZ);
    clear();
}
template<typename T>
T& HashTable<T>::g(const Zobrist& z){
    const u64 h = mask & z[0];
    if(stpos[h] == -1){
        nodes.push_back({z, T(), -1});
        stpos[h] = i32(nodes.size()) - 1;
        return nodes.back().val;
    }
    int g = stpos[h];
    while(true){
        if(nodes[g].z == z) return nodes[g].val;
        if(nodes[g].next == -1) break;
        g = nodes[g].next;
    }
    nodes.push_back({z, T(), -1});
    nodes[g].next = i32(nodes.size()) - 1;
    return nodes.back().val;
}
template<typename T>
void HashTable<T>::s(const Zobrist& z, const T& val){
    bool to_remove = val == T();
    const u64 h = mask & z[0];
    if(stpos[h] == -1){
        if(!to_remove){
            nodes.push_back({z, val, -1});
            stpos[h] = i32(nodes.size()) - 1;
        }
        return;
    }
    int g = stpos[h];
    if(nodes[g].z == z){
        if(!to_remove) nodes[g].val = val;
        else stpos[h] = nodes[g].next;
        return;
    }
    while(nodes[g].next != -1){
        if(nodes[nodes[g].next].z == z){
            if(!to_remove) nodes[nodes[g].next].val = val;
            else nodes[g].next = nodes[nodes[g].next].next;
            return;
        }
        g = nodes[g].next;
    }
    if(!to_remove){
        nodes.push_back({z, val, -1});
        nodes[g].next = i32(nodes.size()) - 1;
    }
}


template struct HashTable<bool>;