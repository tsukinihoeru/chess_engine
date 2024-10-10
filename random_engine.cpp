//
//  random_engine.cpp
//  Chess_Engine
//
//  Created by Harry Chiu on 10/9/24.
//

#include "random_engine.h"

move pick_move_random(bool * mate){
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
        return create_move(0, 0);
    }
    // Initialize a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, legal_moves.size() - 1);
    int a = distrib(gen);
    return legal_moves[a];
}
