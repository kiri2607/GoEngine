#pragma once
#include "Types.hpp"


template<typename T>
struct HashTable{
    struct Node{
        Zobrist z;
        T val;
        i32 next = -1;
    };
    std::vector<Node> nodes;
    std::vector<i32> stpos;
    u64 mask = HASHTABLESZ - 1;
    void clear(){
        std::fill(stpos.begin(), stpos.end(), -1);
        nodes.clear();
    }
    HashTable(){
        stpos.resize(HASHTABLESZ);
        clear();
    }
    T& g(const Zobrist& z){
        const u64 h = mask & z[0];
        if(stpos[h] == -1){
            nodes.push_back({z, T(), -1});
            stpos[h] = i32(nodes.size()) - 1;
            return nodes.back().val;
        }
        i32 g = stpos[h];
        while(true){
            if(nodes[g].z == z) return nodes[g].val;
            if(nodes[g].next == -1) break;
            g = nodes[g].next;
        }
        nodes.push_back({z, T(), -1});
        nodes[g].next = i32(nodes.size()) - 1;
        return nodes.back().val;
    }
    void s(const Zobrist& z, const T& val){
        bool to_remove = val == T();
        const u64 h = mask & z[0];
        if(stpos[h] == -1){
            if(!to_remove){
                nodes.push_back({z, val, -1});
                stpos[h] = i32(nodes.size()) - 1;
            }
            return;
        }
        i32 g = stpos[h];
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
};

struct DSUEntry{
    BitBoard air;
    void add(const DSUEntry& other){
        air |= other.air;
    }
    void set(const Pos& x){
        assert(inbs(0, x, DIM));
        air.reset();
        for(const auto& i : adjc) air.s(x + i, 1);
    }
    void update(const Pos& x, std::array<BitBoard, 2>& brd){
        assert(inbs(0, x, DIM));
        air.s(x, !brd[0].g(x) && !brd[1].g(x));
    }
};

struct DSU{
    i16 size[BRDSZ];
    i16 first[BRDSZ];
    i16 last[BRDSZ];
    i16 next[BRDSZ];
    DSUEntry val[BRDSZ];
    void set(i16 x); // set index x to default values
    DSU();
    i16 end();
    i16 nxt(i16 x);
    i16 fnd(i16 x); // find the capitan of x
    void uni(i16 x, i16 y); // union sets that contain x and y
    void del(i16 x); // delete and reset elements of set that contains x
    i16 it(i16 x); // give the index of the first element of the set that contains x
    i16 fnd(Pos x); // find the capitan of x
    void uni(Pos x, Pos y); // union sets that contain x and y
    void del(Pos x); // delete and reset elements of set that contains x
    i16 it(Pos x); // give the index of the first element of the set that contains x
};