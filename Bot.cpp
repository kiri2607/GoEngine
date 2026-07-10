#include "Bot.hpp"

#include "Board.hpp"
#include "Search.hpp"
#include "Types.hpp"
#include "Random.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
bool Bot::isnumber(std::string s) {
    if (s.empty()) return false;
    size_t start = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    return std::all_of(s.begin() + start, s.end(), [](unsigned char c) { return std::isdigit(c) || c == '.'; });
}

Pos Bot::decode_play(std::string play) {
    std::transform(play.begin(), play.end(), play.begin(), ::toupper);
    if (play == "PASS") return PASS;

    char col_char = play[0];
    int col = col_char - 'A';
    if (col_char >= 'I') col--; 
    
    int row = std::stoi(play.substr(1)); 
    return {static_cast<i8>(DIM - row), static_cast<i8>(col)};
}

std::string Bot::encode_play(Pos pos) {
    if (pos == PASS) return "PASS";
    char col_char = 'A' + pos.j;
    if (col_char >= 'I') col_char++; 
    return std::string(1, col_char) + std::to_string(DIM - pos.i);
}

bool Bot::what_color(std::string c) {
    std::transform(c.begin(), c.end(), c.begin(), ::tolower);
    return (c == "w" || c == "white");
}

bool Bot::any_left(std::stringstream& args) {
    args >> std::ws; 
    return !args.eof();
}

void Bot::print_output(int id, std::string output, bool is_error) {
    std::cout << (is_error ? "? " : "= ");
    if (id != -1) {
        std::cout << id << ' ';
    }
    if (!output.empty()) {
        std::cout << output;
    }
    std::cout << "\n\n";
    std::cout.flush();
}

std::pair<bool, std::string> Bot::cmd_protocol_version(std::stringstream& args) {
    return {true, std::to_string(protocol_version)};
}

std::pair<bool, std::string> Bot::cmd_name(std::stringstream& args) {
    return {true, name};
}

std::pair<bool, std::string> Bot::cmd_version(std::stringstream& args) {
    return {true, version};
}

std::pair<bool, std::string> Bot::cmd_known_command(std::stringstream& args) {
    std::string cmd;
    args >> cmd;
    return {true, commands.contains(cmd) ? "true" : "false"};
}

std::pair<bool, std::string> Bot::cmd_list_commands(std::stringstream& args) {
    std::string res = "";
    for (const auto& cmd : commands) {
        res += cmd + "\n";
    }
    if (!res.empty()) res.pop_back(); 
    return {true, res};
}

std::pair<bool, std::string> Bot::cmd_quit(std::stringstream& args) {
    is_running = false;
    return {true, ""};
}

std::pair<bool, std::string> Bot::cmd_boardsize(std::stringstream& args) {
    std::string size_str;
    args >> size_str;
    
    if (!isnumber(size_str)) {
        return {false, "unacceptable size"};
    }
    
    int size = std::stoi(size_str);
    if (!board_sizes.contains(size)) {
        return {false, "unacceptable size"};
    }
    
    brd.clear();
    return {true, ""};
}

std::pair<bool, std::string> Bot::cmd_clear_board(std::stringstream& args) {
    brd.clear(); 
    return {true, ""};
}

std::pair<bool, std::string> Bot::cmd_komi(std::stringstream& args) {
    std::string komi_str;
    args >> komi_str;
    
    if (!isnumber(komi_str)) {
        return {false, "syntax error"};
    }
    
    float komi = std::stof(komi_str);
    brd.set_komi(komi);
    
    return {true, ""};
}

std::pair<bool, std::string> Bot::cmd_play(std::stringstream& args) {
    std::string color_str, vertex_str;
    args >> color_str >> vertex_str;
    
    if (color_str.empty() || vertex_str.empty()) {
        return {false, "syntax error"};
    }

    bool color = what_color(color_str);
    Pos move = decode_play(vertex_str);
    if(!brd.is_legal(move, color)){
        return {false, "illegal move"};
    }
    brd.make_move(move, color);
    return {true, ""};
}

std::pair<bool, std::string> Bot::cmd_genmove(std::stringstream& args) {
    using namespace std::chrono_literals;
    std::string color_str;
    args >> color_str;
    
    if (color_str.empty()) {
        return {false, "syntax error"};
    }

    bool color = what_color(color_str);
    Search s(color, brd);
    auto start_time = std::chrono::steady_clock::now().time_since_epoch();
    while(true){
        for(int i = 0; i < 1000; i++){
            s.Cycle();
        }
        auto now_time = std::chrono::steady_clock::now().time_since_epoch();
        if(now_time - start_time >= 5s) break;
    }
    return {true, encode_play(s.get_best_move())};
}


void Bot::handle_command(int id, std::string cmd, std::stringstream& args) {
    std::pair<bool, std::string> result;
    
    if (cmd == "protocol_version") result = cmd_protocol_version(args);
    else if (cmd == "name")        result = cmd_name(args);
    else if (cmd == "version")     result = cmd_version(args);
    else if (cmd == "known_command") result = cmd_known_command(args);
    else if (cmd == "list_commands") result = cmd_list_commands(args);
    else if (cmd == "quit")        result = cmd_quit(args);
    else if (cmd == "boardsize")   result = cmd_boardsize(args);
    else if (cmd == "clear_board") result = cmd_clear_board(args);
    else if (cmd == "komi")        result = cmd_komi(args);
    else if (cmd == "play")        result = cmd_play(args);
    else if (cmd == "genmove")     result = cmd_genmove(args);
    else {
        result = {false, "unknown command"};
    }

    print_output(id, result.second, !result.first);
}