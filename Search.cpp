#include "Search.hpp"
#include <algorithm>


float Search::Rollout(Board& b, bool c){
    static PosList moves;
    static PosList filtered;
    moves.clear();
    while(!b.games_end()){
        moves = b.get_legal_moves(c);
        std::shuffle(moves.begin(), moves.end(), random.gen);
        Pos move = PASS;
        for(int i = 1; i <= 3; i++){
            filtered = b.get_with_air_count(!c, i, moves);
            if(filtered.empty()){
                continue;
            }
            move = filtered[random.rand(0, u32(filtered.size()) - 1)];
            break;
        }
        if(move != PASS) move = moves[random.rand(0, u32(moves.size()) - 1)];
        b.make_move(move, c);
        c = !c;
    } 
}
