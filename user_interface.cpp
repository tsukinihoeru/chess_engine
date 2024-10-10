//
//  user_interface.cpp
//  Chess_Engine
//
//  Created by Harry Chiu on 9/30/24.
//
#include "user_interface.h"

std::set <char> valid_fen_chars = {'p', 'b', 'n', 'r', 'q', 'k', 'P', 'B', 'N', 'R', 'Q', 'K'};
std::unordered_map<int, int> pp_to_piece_index = {{0, 12}, {9, 0}, {10, 1}, {11, 2},
    {12, 3}, {13, 4}, {14, 5}, {17, 6}, {18, 7}, {19, 8}, {20, 9}, {21, 10}, {22, 11}, {23, 12}};
bool read_fen(std::string fen, std::vector<char>* board){ //returns successful or not
    int line_length = 0; //counts length of lines, length has to equal 8 when c = '/'
    int total_length = 0; //as the name implies, the total number of squares accounted for
    for(char& c : fen){
        if(c == '/'){
            if(line_length != 8){
                std::cout << "Not a valid fen. Improper line length.\n";
                return false;
            }
            line_length = 0; //new line, reset the length
        }else if(int (c) > int ('0') && int (c) < int('9')) { // c is between 1 and 8
            line_length += int(c) - int('0');
            for(int i = 0; i < int(c) - int('0'); i++){ //pushback c blank spaces
                board->push_back('-');
            }
        }else if(valid_fen_chars.find(c) != valid_fen_chars.end()){
            line_length++;
            total_length++;
            board->push_back(c);
        }else{
            std::cout << "Not a valid fen. Invalid Character: ";
            std::cout << c;
            return false;
        }
        //check for validity after adding
        if(line_length > 8){
            std::cout << "Not a valid fen. Line length too long.\n";
            return false;
        }if(total_length > 64){
            std::cout << "Not a valid fen. More than 64 squares.\n";
            return false;
        }
    }
    return true;
}

void print_board(std::vector<char>* board){
    std::string s = "";
    for(int i = 0; i < 8; i++){ //row
        for(int l = 0; l < 6; l++){ //indivual lines for piece_image
            for(int j = 0; j < 8; j++){ // file
                    int piece_index = fen_char_to_piece_index[(*board)[j + i * 8]];
                    if((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)){ //light square
                        s += light_square_images[piece_index][l];
                    }else{
                        s += dark_square_images[piece_index][l];
                    }
            }s += '\n';
        }
    }
    std::cout << s;
}
void print_board_0x88(std::vector<int>* board){
    std::string s = "";
    for(int i = 0; i < 8; i++){ //row
        for(int l = 0; l < 6; l++){ //indivual lines for piece_image
            for(int j = 0; j < 8; j++){ // file
                    int piece_index = pp_to_piece_index[(*board)[j + i * 16]];
                    if((i % 2 == 0 && j % 2 == 0) || (i % 2 == 1 && j % 2 == 1)){ //light square
                        s += light_square_images[piece_index][l];
                    }else{
                        s += dark_square_images[piece_index][l];
                    }
            }s += '\n';
        }
    }
    std::cout << s;
}
