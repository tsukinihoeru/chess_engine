//
//  board_manager.h
//  Chess_Engine
//
//  Created by Harry Chiu on 10/1/24.
//

#ifndef BOARD_MANAGER_H
#define BOARD_MANAGER_H
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

#define EMPTY 0
#define PAWN 1
#define BISHOP 2
#define KNIGHT 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE 8
#define BLACK 16

#define WHITE_QS_CASTLE 8
#define WHITE_KS_CASTLE 9
#define BLACK_QS_CASTLE 10
#define BLACK_KS_CASTLE 11
#define WHITE_CAN_PASSANT 12
#define BLACK_CAN_PASSANT 13

#define NO_FLAG 0
//#define CASTLE_FLAG 1
#define EN_PASSANT_FLAG 2
#define TWO_UP 3 //can passant now
#define CAPTURE_FLAG 4
#define PROMOTION 5
#define PROMOTION_CAPTURE 6

struct move{
    int source;
    int dest;
    int flag;
    int captured_piece;
    int promotion_piece;
};

struct previous_state{
    int white_can_passant;
    int black_can_passant;
    bool white_qs_castle;
    bool white_ks_castle;
    bool black_qs_castle;
    bool black_ks_castle;
    //bool is_pawn_before_promotion; will implement later, my brain hurts right now
};

bool initialize_board(std::string fen);
int rank_file_to_index(int rank, int file);
std::vector<int> * get_board_pointer();

int contains_friendly(int pos, int side);
int contains_enemy(int pos, int piece);

void print_board();

void generate_moves(std::vector<move>* move_list);
void get_pawn_moves(int pos, std::vector<move>* move_list);
void get_knight_moves(int pos, std::vector<move>* move_list);
void get_king_moves(int pos, std::vector<move>* move_list);
void get_sliding_moves(int pos, std::vector<move>* move_list);

bool is_in_check(int side);
bool in_check_after();
bool square_is_attacked(int pos, int side);


void make_move(move m);
void unmake_move(move m);
void set_previous_state(previous_state ps);
previous_state create_previous_state();
move create_move(int source, int dest);

#endif
