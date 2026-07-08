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
    assert(i >= 0 && i < size());
    return list[i];
}
const Pos& PosList::operator[](const int i) const {
    assert(i >= 0 && i < size());
    return list[i];
}
void PosList::clear(){
    list.clear();
}
History::History(const std::vector<std::array<PosList, 2>> l): list(l) {}
std::vector<std::array<PosList, 2>>::const_iterator History::begin() const {
    return list.begin();
}
std::vector<std::array<PosList, 2>>::const_iterator History::end() const {
    return list.end();
}
std::vector<std::array<PosList, 2>>::iterator History::begin() {
    assert(!list.empty());
    return list.begin();
}
std::vector<std::array<PosList, 2>>::iterator History::end() {
    return list.end();
}
std::array<PosList, 2>& History::front(){
    assert(!list.empty());
    return list.front();
}
std::array<PosList, 2>& History::back(){
    assert(!list.empty());
    return list.back();
}
bool History::empty() const {
    return list.empty();
}
size_t History::size() const {
    return list.size();
}
void History::add(const std::array<PosList, 2>& m){
    list.push_back(m);
}
void History::pop(){
    assert(!list.empty());
    list.pop_back();
}
void History::clear(){
    list.clear();
}
std::array<PosList, 2>& History::operator[](const int i){
    assert(i >= 0 && i < size());
    return list[i];
}
const std::array<PosList, 2>& History::operator[](const int i) const {
    assert(i >= 0 && i < size());
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