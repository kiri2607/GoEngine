#include "Search.hpp"
#include <algorithm>


float Search::Rollout(Board& b, bool c){
    static PosList moves;
    static PosList filtered;
    moves.clear();
    while(!b.games_end()){
        b.make_move(b.get_random_legal(c, random), c);
        c = !c;
    } 
    return b.end_game_result();
}

Search::Search(bool c, const Board& b){
    start_col = c;
    tree.push_back(MCTSNode());
    start_brd = b;
    brd = b;
    float roll = Rollout(brd, start_col);
    tree.front().n = 1;
    tree.front().wwin = roll;
    brd.get_legal_moves(tree.front().legal, start_col);
    std::shuffle(tree.front().legal.begin(), tree.front().legal.end(), random.gen);
}

float Search::uct1(const MCTSNode& p, const MCTSNode& v, bool pcol) const {
    return v.winrate(pcol) + UCT1C * std::sqrt(std::log(p.n) / v.n);
}

i32 Search::bestuct(i32 g, bool col){
    i32 gmax = -1;
    float max = -INFINITY;
    for(i32 i = 0; i < i32(tree[g].chil.size()); i++){
        const auto save = uct1(tree[g], tree[tree[g].chil[i]], col);
        if(save > max){
            max = save;
            gmax = i;
        }
    }
    return gmax;
}

SelectResult Search::Selection(){
    i32 g = 0;
    bool col = start_col;
    
    while(tree[g].chil.size() == tree[g].legal.size() && !brd.games_end()){
        i32 gmax = bestuct(g, col);
        brd.make_move(tree[g].legal[gmax], col);
        g = tree[g].chil[gmax];
        col = !col;
    }
    if(brd.games_end()){
        if(tree[g].n > 0) return {g, tree[g].winrate(1)};
        return {g, brd.end_game_result()};
        
    }
    tree.push_back(MCTSNode());
    tree[g].chil.push_back(tree.size() - 1);
    tree.back().p = g;
    brd.make_move(tree[g].legal[tree[g].chil.size() - 1], col);
    g = tree[g].chil.back();
    col = !col;
    brd.get_legal_moves(tree[g].legal, col);
    std::shuffle(tree[g].legal.begin(), tree[g].legal.end(), random.gen);
    return {g, Rollout(brd, col)}; 
}

void Search::Backprop(const SelectResult& sres){
    i32 g = sres.ind;
    float res = sres.res;
    while(g != -1){
        tree[g].n++;
        tree[g].wwin += res;
        res = 1 - res;
        g = tree[g].p; 
    }
}

void Search::Cycle(){
    brd = start_brd;
    Backprop(Selection());
}

Pos Search::get_best_move(){
    Pos gmaks;
    i32 maks = -1e9;
    for(i32 i = 0; i < tree[0].legal.size(); i++){
        if(maks < tree[tree[0].chil[i]].n){
            maks = tree[tree[0].chil[i]].n;
            gmaks = tree[0].legal[i];
        }
    }
    return gmaks;
}