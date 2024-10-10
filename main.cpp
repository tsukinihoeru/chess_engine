//
//  main.cpp
//  Chess_Engine
//
//  Created by Harry Chiu on 9/30/24.
//

#include <iostream>
#include <set>
#include "user_interface.h"
#include "board_manager.h"
#include "main.h"
#include "random_engine.h"
#include "restrict_engine.h"

#include <chrono>
#include <thread>

int main(int argc, const char * argv[]) {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    /*
    std::vector<char> board;
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    if(read_fen(fen, &board)){
        print_board(&board);
        std::string move;
        std::cout << "Make a move: ";
        std::cin >> move;
        while(move != "quit"){
            if(int(move[0]) >= int('a') && int(move[0]) <= int('h') && int(move[2]) >= int('a') && int(move[2]) <= int('h')
               && int(move[1]) >= int('1') && int(move[1]) <= int('8') && int(move[3]) >= int('1') && int(move[3]) <= int('8')){
                int x1 = int(move[0]) - int('a');
                int y1 = int('8') - int(move[1]);
                int x2 = int(move[2]) - int('a');
                int y2 = int('8') - int(move[3]);
                int coor1 = x1 + 8 * y1;
                int coor2 = x2 + 8 * y2;
                board[coor2] = board[coor1];
                board[coor1] = '-';
                print_board(&board);
                std::cout << "Make a move: ";
                std::cin >> move;
            }else{
                std::cout << "Invalid move, input again: ";
                std::cin >> move;
            }
        }
    }*/
   
    
    bool mate = false;
    int restrictWins = 0;
    int randomWins = 0;
    int draws = 0;
    for(int k = 0; k < 400; k++){
        initialize_board(fen);
        for(int i = 0; i < 200; i++){
            
            //int j = i + 1;
            //std::cout<< j;
            //std::cout << ". ";
            
            move m = pick_move_random(&mate);
            if(mate){
                //std::cout << "restrict won";
                restrictWins++;
                break;
                //return 0;
            }
            //std::cout << move_notation(m);
            //std::cout << " ";
            
            make_move(m);
            
            m = pick_move_restrict(&mate);
            if(mate){
                //std::cout << "random won";
                randomWins++;
                break;
                //return 0;
            }
            //std::cout << move_notation(m);
            //std::cout << " ";
            make_move(m);
        }
        if(!mate)
            draws++;
        mate = false;
    }
    std::cout << "\n\n Restrict wins: ";
    std::cout << restrictWins;
    std::cout << "   Random wins: ";
    std::cout << randomWins;
    std::cout << "   Draws: ";
    std::cout << draws;
    //print_board_0x88(get_board_pointer());
    /*
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    initialize_board(fen);
    std::cout <<"Perft results: \n";
    auto t1 = high_resolution_clock::now();
    std::cout << perft(5, WHITE);
    auto t2 = high_resolution_clock::now();
    std::cout << "\nTime taken: ";
    duration<double, std::milli> ms_double = t2 - t1;
    std::cout << ms_double.count() << "ms\n";
    */
    return 0;
}

long perft(int depth, int side){
    if(depth == 0){
        return 1;
    }
    std::vector<move> move_list;
    long nodes = 0;
    
    int other_side = WHITE;
    if(side == WHITE)
        other_side = BLACK;
    
    generate_moves(&move_list);
    previous_state ps = create_previous_state();
    for(int i = 0; i < move_list.size(); i++){
        make_move(move_list[i]);
        long current_moves = 0;
        if (!in_check_after()){
            current_moves = perft(depth - 1, other_side);
            nodes += current_moves;
        }
        set_previous_state(ps);
        unmake_move(move_list[i]);
        if(depth == 5){
            std::cout << move_notation(move_list[i]);
            std::cout << ": ";
            std::cout << current_moves;
            std::cout << "\n";
        }
    }
    return nodes;
}

std::string move_notation(move m){
    std::string str = "";
    str += (char) ((m.source & 7) + (int) 'a');
    str += std::to_string((m.source / 16) + 1);
    str += (char) ((m.dest & 7) + (int) 'a');
    str += std::to_string((m.dest / 16) + 1);
    if(m.flag == PROMOTION || m.flag == PROMOTION_CAPTURE){
        switch((m.promotion_piece & 7)){
            case BISHOP:
                str += "b";
                break;
            case KNIGHT:
                str += "n";
                break;
            case QUEEN:
                str += "q";
                break;
            case ROOK:
                str += "r";
                break;
        }
    }
    return str;
}
