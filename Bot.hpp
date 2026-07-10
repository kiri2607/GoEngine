#pragma once

#include "Search.hpp"
#include "Board.hpp"
#include "Types.hpp"
#include "Random.hpp"
#include <string>
#include <utility>
#include <set>
struct Bot{
    Board brd;
    int protocol_version = 2;
    std::string name = "kyrlek";
    std::string version = "1.0";
    std::set<int> board_sizes = {9};
    std::set<std::string> commands = {
        "protocol_version",
        "name",
        "version",
        "known_command",
        "list_commands",
        "quit",
        "boardsize",
        "clear_board",
        "komi",
        "play",
        "genmove"
    };
    bool is_running = true;
    bool isnumber(std::string s);
    Pos decode_play(std::string play);
    std::string encode_play(Pos pos);
    bool what_color(std::string c);
    bool any_left(std::stringstream& args);
    void print_output(int id, std::string output, bool is_error);

    std::pair<bool, std::string> cmd_protocol_version(std::stringstream& args);
    std::pair<bool, std::string> cmd_name(std::stringstream& args);
    std::pair<bool, std::string> cmd_version(std::stringstream& args);
    std::pair<bool, std::string> cmd_known_command(std::stringstream& args);
    std::pair<bool, std::string> cmd_list_commands(std::stringstream& args);
    std::pair<bool, std::string> cmd_quit(std::stringstream& args);
    std::pair<bool, std::string> cmd_boardsize(std::stringstream& args);
    std::pair<bool, std::string> cmd_clear_board(std::stringstream& args);
    std::pair<bool, std::string> cmd_komi(std::stringstream& args);
    std::pair<bool, std::string> cmd_play(std::stringstream& args);
    std::pair<bool, std::string> cmd_genmove(std::stringstream& args);

    void handle_command(int id, std::string cmd, std::stringstream& args);
};