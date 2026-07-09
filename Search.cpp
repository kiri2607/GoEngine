#include "Search.hpp"
#include <algorithm>


float Search::Rollout(Board& b, bool c){
    static PosList moves;
    static PosList filtered;
    moves.clear();
    i32 howmany = 0;
    // std::cerr << "before rollout:\n";
    // print_bitboard(brd.get_board());
    // std::cerr << std::endl;
    while(!b.games_end()){
        moves = b.get_legal_moves(c);
        std::shuffle(moves.begin(), moves.end(), random.gen);
        // Pos move = PASS;
        // for(int i = 1; i <= 3; i++){
        //     filtered = b.get_with_air_count(!c, i, moves);
        //     if(filtered.empty()){
        //         continue;
        //     }
        //     move = filtered[random.rand(0, u32(filtered.size()) - 1)];
        //     break;
        // }
        // if(move != PASS) move = moves[random.rand(0, u32(moves.size()) - 1)];
        b.make_move(moves[0], c);
        howmany++;
        c = !c;
    } 
    while(howmany--){
        b.unmake_move();
    }
    // std::cerr << "after rollout:\n";
    // print_bitboard(brd.get_board());
    // std::cerr << std::endl;
    return b.end_game_result();
}

Search::Search(bool c, const Board& b){
    start_col = c;
    tree.push_back(MCTSNode());
    brd = b;
    float roll = Rollout(brd, start_col);
    tree.front().n = 1;
    tree.front().wwin = roll;
    tree.front().legal = brd.get_legal_moves(start_col);
    std::shuffle(tree.front().legal.begin(), tree.front().legal.end(), random.gen);
}

float Search::uct1(const MCTSNode& p, const MCTSNode& v, bool pcol) const {
    return v.winrate(pcol) + UCT1C * std::sqrt(p.n / v.n);
}

i32 Search::bestuct(i32 g, bool col){
    int gmax = -1;
    float max = -INFINITY;
    for(int i = 0; i < int(tree[g].chil.size()); i++){
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
        int gmax = bestuct(g, col);
        brd.make_move(tree[g].legal[gmax], col);
        g = tree[g].chil[gmax];
        col = !col;
    }
    if(brd.games_end()){
        return {g, tree[g].winrate(1)};
    }
    tree.push_back(MCTSNode());
    tree[g].chil.push_back(tree.size() - 1);
    tree.back().p = g;
    brd.make_move(tree[g].legal[tree[g].chil.size() - 1], col);
    g = tree[g].chil.back();
    col = !col;
    tree[g].legal = brd.get_legal_moves(col);
    std::shuffle(tree[g].legal.begin(), tree[g].legal.end(), random.gen);
    return {g, Rollout(brd, col)};
}

void Search::Backprop(const SelectResult& sres){
    int g = sres.ind;
    float res = sres.res;
    while(g != -1){
        if(g != 0) brd.unmake_move();
        tree[g].n++;
        tree[g].wwin += res;
        res = 1 - res;
        g = tree[g].p; 
    }
}

void Search::Cycle(){
    Backprop(Selection());
}

void Search::find_in_sub_tree(i32 g, std::vector<MCTSNode>& new_tree){
    new_tree.push_back(tree[g]);
    for(const auto& i : tree[g].chil){
        find_in_sub_tree(i, new_tree);
    }
}

void Search::make_move(const Pos& pos){
    std::vector<MCTSNode> new_tree;
    i32 new_root = tree[0].chil[std::find(tree[0].legal.begin(), tree[0].legal.end(), pos) - tree[0].legal.begin()];
    find_in_sub_tree(new_root, new_tree);
    tree = new_tree;
    brd.make_move(pos, start_col);
    start_col = !start_col;
}

Pos& Search::get_best_move(){
    return tree[0].legal[bestuct(0, start_col)];

}