//
//  restrict_engine.cpp
//  Chess_Engine
//
//  Created by Harry Chiu on 10/9/24.
//
#include "restrict_engine.h"

int get_move_num(){
    std::vector<move> moves;
    std::vector<move> legal_moves;
    generate_moves(&moves);
    
    previous_state ps = create_previous_state();
    for(int i = 0; i < moves.size(); i++){
        make_move(moves[i]);
        if (!in_check_after()){
            legal_moves.push_back(moves[i]);
        }
        set_previous_state(ps);
        unmake_move(moves[i]);
    }
    return legal_moves.size();
}

move pick_move_restrict(bool * mate){
    std::vector<move> moves;
    std::vector<move> legal_moves;
    generate_moves(&moves);
    previous_state ps = create_previous_state();
    for(int i = 0; i < moves.size(); i++){
        make_move(moves[i]);
        if (!in_check_after()){
            legal_moves.push_back(moves[i]);
        }
        set_previous_state(ps);
        unmake_move(moves[i]);
    }
    if(legal_moves.size() == 0){
        *mate = true;
        return create_move(0,0);
    }
    move best_move = legal_moves[0];
    int move_rating = 1000000;
    for(int i = 0; i < legal_moves.size(); i++){
        make_move(legal_moves[i]);
        int k = get_move_num();
        if(k <= move_rating){
            move_rating = k;
            best_move = legal_moves[i];
        }
        set_previous_state(ps);
        unmake_move(legal_moves[i]);
    }
    return best_move;
}



