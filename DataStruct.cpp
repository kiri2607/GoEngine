#include "Types.hpp"
#include "DataStruct.hpp"

void DSU::set(i16 x){
    assert(inbs(0, x, BRDSZ));
    size[x] = 1;
    first[x] = x;
    last[x] = x;
    next[x] = x;
    val[x].set(rozs(x));
}

DSU::DSU(){
    for(i16 i = 0; i < BRDSZ; i++) set(i);
}
i16 DSU::end(){
    return -1;
}
i16 DSU::nxt(i16 x){
    assert(inbs(0, x, BRDSZ));
    if(next[x] == x) return end();
    else return next[x];
}
i16 DSU::fnd(i16 x){
    assert(inbs(0, x, BRDSZ));
    if(last[x] != x) last[x] = fnd(last[x]);
    return last[x];
}
void DSU::uni(i16 x, i16 y){
    assert(inbs(0, x, BRDSZ));
    assert(inbs(0, y, BRDSZ));
    x = fnd(x); y = fnd(y);
    if(x == y) return;
    if(size[x] > size[y]) std::swap(x, y);
    val[y].add(val[x]);
    size[y] += size[x];
    next[x] = first[y];
    first[y] = first[x];
    last[x] = y;
}
void DSU::del(i16 x){
    assert(inbs(0, x, BRDSZ));
    x = first[fnd(x)];
    while(x != end()){
        i16 n = nxt(x);
        set(x);
        x = n;
    }
}
i16 DSU::it(i16 x){
    assert(inbs(0, x, BRDSZ));
    return first[fnd(x)];
}
i16 DSU::fnd(Pos x){
    return fnd(spla(x));
}
void DSU::uni(Pos x, Pos y){
    uni(spla(x), spla(y));
}
void DSU::del(Pos x){
    del(spla(x));
}
i16 DSU::it(Pos x){
    return it(spla(x));
}