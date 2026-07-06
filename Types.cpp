#include "Types.hpp"

PosList::PosList(const std::vector<Pos> l): list(l) {}


std::vector<Pos>::const_iterator PosList::begin() const {
    assert(!list.empty());
    return list.begin();
}
std::vector<Pos>::const_iterator PosList::end() const {
    return list.end();
}
std::vector<Pos>::iterator PosList::begin(){
    assert(!list.empty());
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
    assert(!list.empty());
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