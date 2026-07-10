#include "Types.hpp"
#include "Board.hpp"
#include "Search.hpp"
#include "Random.hpp"


#include <chrono>
int main(){
    using namespace std::chrono_literals;
    Board b;
    Search s(0, b);
    auto start = std::chrono::steady_clock::now();
    int it = 0;
    while(true){
        auto now = std::chrono::steady_clock::now();
        if(now - start >= 1s){
            break;
        }
        it++;
        s.Cycle();
    }
    std::cout << it << '\n';
}

// Bot b;
//     std::string line;
//     while(b.is_running && std::getline(std::cin, line)){
//         std::stringstream ss(line);
//         std::string token;
//         ss >> token;
        
//         if (token.empty()) continue; 

//         int id = -1;
//         std::string cmd;

//         if (b.isnumber(token)) {
//             id = std::stoi(token);
//             ss >> cmd; 
//         }
//         else {
//             cmd = token; 
//         }

//         b.handle_command(id, cmd, ss); 
//         print_bitboard(b.brd.get_board());
//     }