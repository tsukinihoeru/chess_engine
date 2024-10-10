//
//  board_manager.cpp
//  Chess_Engine
//
//  Created by Harry Chiu on 10/1/24.
//

#include "board_manager.h"

std::vector<int> board;
int current_side = WHITE;

std::vector<int> * get_board_pointer(){
    return &board;
}

std::unordered_map<char, int> fen_char_to_piece_type = {{'p', PAWN}, {'b', BISHOP}, {'n', KNIGHT},
    {'r', ROOK}, {'q', QUEEN}, {'k', KING}};




//ALL PIECE OFFSETS
const std::vector<int> knight_offsets = {0x1F, 0x21, 0xE, 0x12, -0x12, -0xE, -0x21, -0x1F};

const std::vector<int> king_offsets = {0x10, -0x10, 1, -1, 0x11, -0x11, 0xF, -0xF};

//index 1: Bishop, index 3: rook, index 4: queen, corresponds to piece IDs
const std::vector<std::vector<int>> sliding_piece_offsets = {{},{}, {0x11, -0x11, 0xF, -0xF}, {},
    {0x10, -0x10, 1, -1}, {0x10, -0x10, 1, -1, 0x11, -0x11, 0xF, -0xF}};


bool initialize_board(std::string fen){
    int rank = 7;
    int file = 0;
    
    //wipe the board and fill it with 0s
    board.clear();
    for(int i = 0; i < 128; i++)
        board.push_back(0);
    
    
    for(char& c : fen){
        if(c == '/'){
            rank--;
            file = 0;
        }else if(int (c) > int ('0') && int (c) < int('9')) { // c is between 1 and 8
            file += int (c) - int('0');
        }else if(fen_char_to_piece_type.contains((char) std::tolower(c)) && (!(rank_file_to_index(rank, file) & 0x88))){
            int piece_type = fen_char_to_piece_type[(char) std::tolower(c)];
            int piece_color = isupper(c) ? WHITE : BLACK;
            board[rank_file_to_index(rank, file)] = piece_type | piece_color;
            file++;
        }else if(rank == 0 && file == 8){
            if(c == 'K'){
                board[WHITE_KS_CASTLE] = 1;
                //std::cout << "white can kingside castle\n";
            }else if(c == 'Q'){
                board[WHITE_QS_CASTLE] = 1;
                //std::cout << "white can queenside castle\n";
            }else if(c == 'q'){
                board[BLACK_QS_CASTLE] = 1;
                //std::cout << "black can queenside castle\n";
            }else if(c == 'k'){
                board[BLACK_KS_CASTLE] = 1;
                //std::cout << "black can kingside castle\n";
            }else if(c == 'w'){
                current_side = WHITE;
            }else if(c == 'b'){
                current_side = BLACK;
            }
        }
    }
    return true;
}

int rank_file_to_index(int rank, int file){
    return rank * 16 + file;
}

void flip_sides(){
    current_side ^= 24;
}

void generate_moves(std::vector<move>* move_list){
   /*
    //IF DOUBLE CHECK -> CAN ONLY MOVE KING
    if(double_check){
        for(int i = 0; i < 128; i++){
            if(i & 0x88){ //not a valid square
                continue;
            }
            if(contains_friendly(i, side)){
                if((board[i] & 7) == KING){
                    get_king_moves(i, move_list);
                    return;
                }
            }
        }
    }*/
    for(int i = 0; i < 128; i++){
        if(i & 0x88){ //not a valid square
            continue;
        }
        if(contains_friendly(i, current_side)){
            int piece_id = board[i] & 7;
            if(piece_id == PAWN){
                get_pawn_moves(i, move_list);
            }
            else if(piece_id == KNIGHT){
                get_knight_moves(i, move_list);
            }else if(piece_id == KING){
                get_king_moves(i, move_list);
            }else{
                get_sliding_moves(i, move_list);
            }
        }
    }
}

move create_move(int source, int dest){
    struct move new_move;
    new_move.source = source;
    new_move.dest = dest;
    new_move.flag = NO_FLAG;
    new_move.captured_piece = -1;
    return new_move;
}

move create_move(int source, int dest, int flag){
    struct move new_move;
    new_move.source = source;
    new_move.dest = dest;
    new_move.flag = flag;
    new_move.captured_piece = -1;
    return new_move;
}

move create_move(int source, int dest, int flag, int captured_piece){
    struct move new_move;
    new_move.source = source;
    new_move.dest = dest;
    new_move.flag = flag;
    new_move.captured_piece = captured_piece;
    return new_move;
}

move create_promotion_move(int source, int dest, int promotion_piece){
    struct move new_move;
    new_move.source = source;
    new_move.dest = dest;
    new_move.flag = PROMOTION;
    new_move.promotion_piece = promotion_piece;
    return new_move;
}

move create_promotion_capture(int source, int dest, int promotion_piece, int captured_piece){
    struct move new_move;
    new_move.source = source;
    new_move.dest = dest;
    new_move.flag = PROMOTION_CAPTURE;
    new_move.promotion_piece = promotion_piece;
    new_move.captured_piece = captured_piece;
    return new_move;
}

void get_pawn_moves(int pos, std::vector<move>* move_list){
    int direction = 1; //white is positive, goes up the board
    if((board[pos] & 24) == BLACK){
        direction = -1;
    }
    
    if((direction == 1 && pos/16 == 6) || (direction == -1 && pos/16 == 1)){
        //either promos or promo captures, I LOVE LIFE HAHAA HA HAAAAAAAAAAAA
        int next_square = pos + 16 * direction;
        if(!(next_square&0x88) && !board[next_square]){// nothing in front
            move_list->push_back(create_promotion_move(pos, next_square, (board[pos] & 24) + KNIGHT));
            move_list->push_back(create_promotion_move(pos, next_square, (board[pos] & 24) + BISHOP));
            move_list->push_back(create_promotion_move(pos, next_square, (board[pos] & 24) + ROOK));
            move_list->push_back(create_promotion_move(pos, next_square, (board[pos] & 24) + QUEEN));
        }
        //check for captures
        int capture_1 = 17 * direction + pos;
        int capture_2 = 15 * direction + pos;
        if(!(capture_1 & 0x88)){
            //If it contains an enemy
            if(contains_enemy(capture_1, board[pos])){
                move_list->push_back(create_promotion_capture(pos, capture_1, (board[pos] & 24) + KNIGHT, board[capture_1]));
                move_list->push_back(create_promotion_capture(pos, capture_1, (board[pos] & 24) + BISHOP, board[capture_1]));
                move_list->push_back(create_promotion_capture(pos, capture_1, (board[pos] & 24) + ROOK, board[capture_1]));
                move_list->push_back(create_promotion_capture(pos, capture_1, (board[pos] & 24) + QUEEN, board[capture_1]));
            }
        }
        if(!(capture_2 & 0x88)){
            if(contains_enemy(capture_2, board[pos])){
                move_list->push_back(create_promotion_capture(pos, capture_2, (board[pos] & 24) + KNIGHT, board[capture_2]));
                move_list->push_back(create_promotion_capture(pos, capture_2, (board[pos] & 24) + BISHOP, board[capture_2]));
                move_list->push_back(create_promotion_capture(pos, capture_2, (board[pos] & 24) + ROOK, board[capture_2]));
                move_list->push_back(create_promotion_capture(pos, capture_2, (board[pos] & 24) + QUEEN, board[capture_2]));
            }
        }
        return;
    }
    
    int next_square = pos + 16 * direction;
    
    //check for moves
    if(!(next_square&0x88) && !board[next_square]){// nothing in front
        move_list->push_back(create_move(pos, next_square));
        if(((pos / 16) == 1 && direction == 1) || ((pos / 16) == 6 && direction == -1)){ // if the pawn is still on starting square
            int next_next_square = next_square + 16*direction;
            if(!(next_next_square&0x88) && !board[next_next_square]){ //pushing two squares up
                move_list->push_back(create_move(pos, next_next_square, TWO_UP));
            }
        }
    }
    
    
    //check for captures
    int capture_1 = 17 * direction + pos;
    int capture_2 = 15 * direction + pos;
    if(!(capture_1 & 0x88)){
        //If it contains an enemy
        if(contains_enemy(capture_1, board[pos])){
            move_list->push_back(create_move(pos, capture_1, CAPTURE_FLAG, board[capture_1]));
        }
    }
    if(!(capture_2 & 0x88)){
        if(contains_enemy(capture_2, board[pos])){
            move_list->push_back(create_move(pos, capture_2, CAPTURE_FLAG, board[capture_2]));
        }
    }
    
    //en passant, the dreaded one
    if(((pos / 16) == 4 && direction == 1) || ((pos / 16) == 3 && direction == -1)){
        if(direction == 1){
            if(board[WHITE_CAN_PASSANT] != -1){
                if(abs(board[WHITE_CAN_PASSANT] - pos) == 1){
                    move_list->push_back(create_move(pos, board[WHITE_CAN_PASSANT] + 16, EN_PASSANT_FLAG));
                }
            }
        }else{
            if(board[BLACK_CAN_PASSANT] != -1){
                if(abs(board[BLACK_CAN_PASSANT] - pos) == 1){
                    move_list->push_back(create_move(pos, board[BLACK_CAN_PASSANT] - 16, EN_PASSANT_FLAG));
                }
            }
        }
    }
}

void get_knight_moves(int pos, std::vector<move>* move_list){ //trust that the piece at pos is a knight
    for(int offset : knight_offsets){
        int npos = pos + offset;
        if(npos&0x88) //not a valid move
            continue;
        
        //Empty square
        if(!board[npos]){
             move_list->push_back(create_move(pos, npos));
        }
        
        //capture
        else if(contains_enemy(npos, board[pos])){ //Contains an enemy piece
            move_list->push_back(create_move(pos, npos, CAPTURE_FLAG, board[npos])); //CAPTURE
        }
    }
}

void get_king_moves(int pos, std::vector<move>* move_list){ //trust that the piece at pos is a king
    for(int offset : king_offsets){
        int npos = pos + offset;
        if(npos&0x88) //not a valid move
            continue;
        if(!board[npos]){ //EMPTY SQUARE
             move_list->push_back(create_move(pos, npos));
        }else if(contains_enemy(npos, board[pos])){ //Contains an enemy piece
            move_list->push_back(create_move(pos, npos, CAPTURE_FLAG, board[npos])); //CAPTURE
        }
    }if((board[pos] & 24) == WHITE){
        if(board[WHITE_KS_CASTLE]){
            if(!board[5] && !board[6]){
                if(!square_is_attacked(4, WHITE) && !square_is_attacked(5, WHITE) && !square_is_attacked(6, WHITE)){
                    move_list->push_back(create_move(pos, 6, WHITE_KS_CASTLE));
                }
            }
        }if(board[WHITE_QS_CASTLE]){
            if(!board[1] && !board[2] && !board[3]){
                if(!square_is_attacked(2, WHITE) && !square_is_attacked(3, WHITE) && !square_is_attacked(4, WHITE)){
                    move_list->push_back(create_move(pos, 2, WHITE_QS_CASTLE));
                }
            }
        }
    }else{
        if(board[BLACK_KS_CASTLE]){
            if(!board[117] && !board[118]){
                if(!square_is_attacked(116, BLACK) && !square_is_attacked(117, BLACK) && !square_is_attacked(118, BLACK)){
                    move_list->push_back(create_move(pos, 118, BLACK_KS_CASTLE));
                }
            }
        }if(board[BLACK_QS_CASTLE]){
            if(!board[115] && !board[114] && !board[113]){
                if(!square_is_attacked(114, BLACK) && !square_is_attacked(115, BLACK) && !square_is_attacked(116, BLACK)){
                    move_list->push_back(create_move(pos, 114, BLACK_QS_CASTLE));
                }
            }
        }
    }
}

void get_sliding_moves(int pos, std::vector<move>* move_list){
    for(int offset: sliding_piece_offsets[board[pos] & 7]){
        for(int magnitude = 1; magnitude < 8; magnitude++){
            int npos = magnitude * offset + pos;
            if(npos & 0x88){ //not a valid move
                break;
            }
            
            //empty square
            if(!board[npos]){
                move_list->push_back(create_move(pos, npos));
            }else if(contains_friendly(npos, board[pos])){
                break;
            }
            else{ //contains enemy, is a CAPTURE
                move_list->push_back(create_move(pos, npos, CAPTURE_FLAG, board[npos]));
                break;
            }
        }
    }
}

int contains_friendly(int pos, int piece){ // returns 0 if no friendly, and some other number otherwise
    return board[pos] & (piece & 24);
}

int contains_enemy(int pos, int piece){
    return board[pos] & ((piece & 24) ^ 24);
}

void print_board(){
    for(int rank = 7; rank >= 0; rank--){
        for(int file = 0; file < 16; file++){
            int index = rank * 16 + file;
            if(board[index]){
                std::cout << (board[index] & 7);
                //std::cout << " ";
                
            }
            else
                std::cout<<".";
        }std::cout<<'\n';
    }
}


void make_move(move m){
    flip_sides();
    //reset en passant flags
    if((board[m.source] & 24) == WHITE){
        board[BLACK_CAN_PASSANT] = -1;
        if((board[m.source] & 7) == KING){
            board[WHITE_KS_CASTLE] = false;
            board[WHITE_QS_CASTLE] = false;
        }
    }else{
        board[WHITE_CAN_PASSANT] = -1;
        if((board[m.source] & 7) == KING){
            board[BLACK_KS_CASTLE] = false;
            board[BLACK_QS_CASTLE] = false;
        }
    }
    //check castling
    //if move originated from a1
    if(m.source == 0){
        board[WHITE_QS_CASTLE] = 0;
    }
    
    //if move originated from h1
    else if(m.source == 7){
        board[WHITE_KS_CASTLE] = 0;
    }
    
    //if move originated from a8
    else if(m.source == 112){
        board[BLACK_QS_CASTLE] = 0;
    }
    
    //if move originated from h8
    else if(m.source == 119){
        board[BLACK_KS_CASTLE] = 0;
    }
    
    //if dest at a1
    if(m.dest == 0){
        board[WHITE_QS_CASTLE] = 0;
    }
    
    //if dest at h1
    else if(m.dest == 7){
        board[WHITE_KS_CASTLE] = 0;
    }
    
    //if dest at a8
    else if(m.dest == 112){
        board[BLACK_QS_CASTLE] = 0;
    }
    
    //if dest h8
    else if(m.dest == 119){
        board[BLACK_KS_CASTLE] = 0;
    }
    
    if(m.flag == NO_FLAG){
        board[m.dest] = board[m.source];
        board[m.source] = EMPTY;
    }
    
    else if(m.flag == CAPTURE_FLAG){
        
        
        board[m.dest] = board[m.source];
        board[m.source] = EMPTY;
    }
    
    //set the en passant flags
    else if(m.flag == TWO_UP){
        if((board[m.source] & 24) == WHITE){
            board[BLACK_CAN_PASSANT] = m.dest;
        }else{
            board[WHITE_CAN_PASSANT] = m.dest;
        }
        board[m.dest] = board[m.source];
        board[m.source] = EMPTY;
        
    }
    
    else if(m.flag == WHITE_KS_CASTLE){
        //move the king
        board[6] = board[4];
        board[4] = EMPTY;
        //move the rook
        board[5] = board[7];
        board[7] = EMPTY;
    }else if(m.flag == WHITE_QS_CASTLE){
        //move the king
        board[2] = board[4];
        board[4] = EMPTY;
        //move the rook
        board[3] = board[0];
        board[0] = EMPTY;
    }
    else if(m.flag == BLACK_KS_CASTLE){
        //move the king
        board[118] = board[116];
        board[116] = EMPTY;
        //move the rook
        board[117] = board[119];
        board[119] = EMPTY;
    }
    else if(m.flag == BLACK_QS_CASTLE){
        //move the king
        board[114] = board[116];
        board[116] = EMPTY;
        //move the rook
        board[115] = board[112];
        board[112] = EMPTY;
    }
    
    else if(m.flag == EN_PASSANT_FLAG){
        //scream and fucking cry idk how to code this shit
        board[m.dest] = board[m.source];
        if((board[m.source] & 24) == WHITE){
            board[board[WHITE_CAN_PASSANT]] = EMPTY;
            
        }else{
            board[board[BLACK_CAN_PASSANT]] = EMPTY;
        }
        board[m.source] = EMPTY;
    }
    else if(m.flag == PROMOTION){
        board[m.dest] = m.promotion_piece;
        board[m.source] = EMPTY;
    }
    else if(m.flag == PROMOTION_CAPTURE){
        board[m.dest] = m.promotion_piece;
        board[m.source] = EMPTY;
    }
}

void set_previous_state(previous_state ps){
    board[WHITE_KS_CASTLE] = ps.white_ks_castle;
    board[WHITE_QS_CASTLE] = ps.white_qs_castle;
    board[BLACK_KS_CASTLE] = ps.black_ks_castle;
    board[BLACK_QS_CASTLE] = ps.black_qs_castle;
    board[WHITE_CAN_PASSANT] = ps.white_can_passant;
    board[BLACK_CAN_PASSANT] = ps.black_can_passant;
}

previous_state create_previous_state(){
    previous_state ps;
    ps.white_ks_castle =  board[WHITE_KS_CASTLE];
    ps.white_qs_castle = board[WHITE_QS_CASTLE];
    ps.black_ks_castle = board[BLACK_KS_CASTLE];
    ps.black_qs_castle = board[BLACK_QS_CASTLE];
    ps.white_can_passant = board[WHITE_CAN_PASSANT];
    ps.black_can_passant = board[BLACK_CAN_PASSANT];
    return ps;
}

void unmake_move(move m){
    flip_sides();
    if(m.flag == NO_FLAG || m.flag == TWO_UP){
        board[m.source] = board[m.dest];
        board[m.dest] = EMPTY;
    }else if(m.flag == CAPTURE_FLAG){
        board[m.source] = board[m.dest];
        board[m.dest] = m.captured_piece;
    }
    
    else if(m.flag == WHITE_KS_CASTLE){
        //move the king
        board[4] = board[6];
        board[6] = EMPTY;
        
        //move the rook
        board[7] = board[5];
        board[5] = EMPTY;
    }else if(m.flag == WHITE_QS_CASTLE){
        //move the king
        board[4] = board[2];
        board[2] = EMPTY;
        //move the rook
        board[0] = board[3];
        board[3] = EMPTY;
    }
    else if(m.flag == BLACK_KS_CASTLE){
        //move the king
        board[116] = board[118];
        board[118] = EMPTY;
        //move the rook
        board[119] = board[117];
        board[117] = EMPTY;
    }
    else if(m.flag == BLACK_QS_CASTLE){
        //move the king
        board[116] = board[114];
        board[114] = EMPTY;
        //move the rook
        board[112] = board[115];
        board[115] = EMPTY;
    }else if(m.flag == EN_PASSANT_FLAG){
        board[m.source] = board[m.dest];
        if((board[m.source] & 24) == WHITE){
            board[board[WHITE_CAN_PASSANT]] = BLACK | PAWN;
        }else{
            board[board[BLACK_CAN_PASSANT]] = WHITE | PAWN;
        }
        board[m.dest] = EMPTY;
    }else if(m.flag == PROMOTION){
        board[m.source] = (board[m.dest] & 24) + PAWN;
        board[m.dest] = EMPTY;
    }
    else if(m.flag == PROMOTION_CAPTURE){
        board[m.source] = (board[m.dest] & 24) + PAWN;
        board[m.dest] = m.captured_piece;
    }
}


//helper algo for make_move, extermely inefficient
bool is_in_check(int side){ //side as in WHITE or BLACK
    
    //find king position
    int king_pos = -1;
    for(int i = 0; i < 128; i++){
        if(board[i] == side + KING){
            king_pos = i;
            break;
        }
    }
    return square_is_attacked(king_pos, side);
}

bool in_check_after(){
    int king_pos = -1;
    for(int i = 0; i < 128; i++){
        if(board[i] == (current_side ^ 24) + KING){
            king_pos = i;
            break;
        }
    }
    return square_is_attacked(king_pos, (current_side ^ 24));
}

bool square_is_attacked(int pos, int side){
    //check for enemy pawns
    int op_side = BLACK;
    int direction = 1;
    if(side == BLACK){
        direction = -1;
        op_side = WHITE;
    }
    int capture_1 = 17 * direction + pos;
    int capture_2 = 15 * direction + pos;
    if(!(capture_1 & 0x88)){
        if(board[capture_1] & op_side && (board[capture_1] & 7) == PAWN){
            return true;
        }
    }if(!(capture_2 & 0x88)){
        if(board[capture_2] & op_side && (board[capture_2] & 7) == PAWN){
            return true;
        }
    }
    
    //check for enemy knights
    for(int offset : knight_offsets){
        int npos = pos + offset;
        if(npos&0x88) //not a valid move
            continue;
        if(board[npos] & op_side && (board[npos] & 7) == KNIGHT){
            return true;
        }
    }
    
    //check for enemy king
    for(int offset : king_offsets){
        int npos = pos + offset;
        if(npos&0x88) //not a valid move
            continue;
        if(board[npos] & op_side && (board[npos] & 7) == KING){
            return true;
        }
    }
    //check for rooks and queens
    for(int offset: sliding_piece_offsets[ROOK]){
        for(int magnitude = 1; magnitude < 8; magnitude++){
            int npos = magnitude * offset + pos;
            if(npos & 0x88){ //not a valid move
                break;
            }
            if(board[npos] & side){
                break;
            }
            else if (board[npos] & op_side){
                if((board[npos] & 7) == ROOK|| (board[npos] & 7) == QUEEN){
                    return true;
                }else
                    break;
            }
        }
    }
    
    //check for bishops and queens
    for(int offset: sliding_piece_offsets[BISHOP]){
        for(int magnitude = 1; magnitude < 8; magnitude++){
            int npos = magnitude * offset + pos;
            if(npos & 0x88){ //not a valid move
                break;
            }
            if(board[npos] & side){
                break;
            }
            else if (board[npos] & op_side){
                if((board[npos] & 7) == BISHOP|| (board[npos] & 7) == QUEEN){
                    return true;
                }else
                    break;
            }
        }
    }
    
    return false;
}


/*
void generate_xray_attacks(int side){ //SIDE IS THE ONE THAT GETS TO MOVE, THEREFORE WE WANT ATTACKS FROM THE ENEMY OF SIDE
    for(int i = 0; i < 128; i++){
        if(i & 0x88){ //not a valid square
            continue;
        }
        //see all enemy attacks for attacked squares, and sliding pieces for pins
        if(contains_enemy(i, side)){
            int piece_id = board[i] & 7;
            if(piece_id == PAWN){
                generate_pawn_xray(i);
            }
            else if(piece_id == KNIGHT){
                generate_knight_xray(i);
            }else if(piece_id == KING){
                generate_king_xray(i);
            }else{
                generate_sliding_xray(i);
            }
        }
    }
}

void generate_pawn_xray(int pos){
    int direction = 1; //white is positive, goes up the board
    if((board[pos] & 24) == BLACK){
        direction = -1;
    }
    
    //diagonal squares are attacked as long as they exist
    int capture_1 = 17 * direction + pos;
    int capture_2 = 15 * direction + pos;
    if(!(capture_1 & 0x88)){
        is_attacked_xray[capture_1] = true;
        
        //check if king is attacked, and if it is double checked
        if(contains_enemy(capture_1, board[pos]) && (board[capture_1] & 7) == KING){
            if(capture_square == -1){
                capture_square = pos;
            }else{
                double_check = true;
            }
        }
    }if(!(capture_2 & 0x88)){
        is_attacked_xray[capture_2] = true;
        
        //check if king is attacked, and if it is double checked
        if(contains_enemy(capture_1, board[pos]) && (board[capture_1] & 7) == KING){
            if(capture_square == -1){
                capture_square = pos;
            }else{
                double_check = true;
            }
        }
    }
}

void generate_knight_xray(int pos){
    for(int offset : knight_offsets){
        int npos = pos + offset;
        if(!(npos & 0x88)){
            is_attacked_xray[npos] = true;
            
            //check if king is attacked, and if it is double checked
            if(contains_enemy(npos, board[pos]) && (board[npos] & 7) == KING){
                if(capture_square == -1){
                    capture_square = pos;
                }else{
                    double_check = true;
                }
            }

        }
    }
}

void generate_king_xray(int pos){
    for(int offset : king_offsets){
        int npos = pos + offset;
        if(!(npos & 0x88)){
            is_attacked_xray[npos] = true;
        }
    }
}

void generate_sliding_xray(int pos){
    bool xrayed_through_king = false;
    for(int offset: sliding_piece_offsets[board[pos] & 7]){
        for(int magnitude = 1; magnitude < 8; magnitude++){
            int npos = magnitude * offset + pos;
            if(npos & 0x88){ //not a valid move
                break;
            }if(!board[npos]){ //empty square
                is_attacked_xray[npos] = true;
            }else if(contains_friendly(npos, board[pos])){ //friendly piece is "attacked" (protected)
                is_attacked_xray[npos] = true;
                break;
            }else{ //contains enemy, is a CAPTURE
                
                
                //sliding pieces xray through the king
                if((board[npos] & 7) == KING){
                    xrayed_through_king = true;
                    is_attacked_xray[npos] = true;
                    if(capture_square == -1){
                        capture_square = pos;

                        //compute blocking squares
                        for(int bagnitude = 1; bagnitude < magnitude; bagnitude++){
                            int block_pos = npos - offset * bagnitude;
                            squares_to_block[block_pos] = true;
                        }
                        
                    }else{
                        double_check = true;
                    }
                }
                
                //enemy piece blocks xray
                else{
                    is_attacked_xray[npos] = true;
                    break;
                }
            }
        }
    }
}*/
