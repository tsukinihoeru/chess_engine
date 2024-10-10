//
//  piece_image.h
//  Chess_Engine
//
//  Created by Harry Chiu on 9/30/24.
//
#ifndef PIECE_IMAGE_H
#define PIECE_IMAGE_H
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
extern std::vector<std::vector<std::string>> light_square_images;
extern std::vector<std::vector<std::string>> dark_square_images;
extern std::unordered_map<char, int> fen_char_to_piece_index;
#endif
