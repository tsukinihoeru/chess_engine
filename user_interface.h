//
//  user_interface.h
//  Chess_Engine
//
//  Created by Harry Chiu on 9/30/24.
//

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H
#include <iostream>
#include<set>
#include "user_interface.h"
#include "piece_image.h"

extern std::set <char> valid_fen_chars;

bool read_fen(std::string fen, std::vector<char>* board);

void print_board(std::vector<char>* board);

void print_board_0x88(std::vector<int>* board);
#endif
