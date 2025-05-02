#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <uthash.h>

// -----(Setup)-----

// Hashmap (Store move info per player)
typedef struct Move {
    int moveNumber;       // key: move order (starting at 1)
    int x, y;             // 1-based coordinates
    UT_hash_handle hh;    // makes this structure hashable
} Move;

// Global pointers for user and bot move hashmaps
Move *user_moves_map = NULL;
Move *bot_moves_map = NULL;

// To add a move to a hashmap
void add_move(Move **movesMap, int moveNumber, int x, int y) {
    Move *move = (Move *)malloc(sizeof(Move));
    move->moveNumber = moveNumber;
    move->x = x;
    move->y = y;
    HASH_ADD_INT(*movesMap, moveNumber, move);
}

// Sorting moves by the move number
int move_cmp(Move *a, Move *b) {
    return a->moveNumber - b->moveNumber;
}

// Printing moves from the hashmap
void print_moves_hash(Move *movesMap) {
    Move *m, *tmp;
    HASH_SORT(movesMap, move_cmp);
    HASH_ITER(hh, movesMap, m, tmp) {
        printf("%d:(%d,%d) ", m->moveNumber, m->x, m->y);
    }
    printf("\n");
}

// Tree node (Store board state per move)
typedef struct BoardStateNode {
    int moveNumber;                 // move count at which this state was recorded
    char board[3][3];
    struct BoardStateNode *left;    // Points the previous move
    struct BoardStateNode *right;   // Points the next move
} BoardStateNode;

BoardStateNode *boardTree = NULL;

// Insert a new board state into the tree (Inserted at the right-most leaf)
BoardStateNode* insert_board_state(BoardStateNode* root, int moveNumber, char board[3][3]) {
    BoardStateNode* node = (BoardStateNode*)malloc(sizeof(BoardStateNode));
    node->moveNumber = moveNumber;
    memcpy(node->board, board, sizeof(char)*9);
    node->left = node->right = NULL;
    if (root == NULL) return node;
    BoardStateNode* current = root;
    while(current->right != NULL) {
        current = current->right;
    }
    current->right = node;
    return root;
}

// Print board state from the tree
void print_board_state_tree(BoardStateNode* root) {
    while (root) {
        BoardStateNode* row_nodes[3];
        int count = 0;

        // Collect up to 3 nodes
        for (int i = 0; i < 3 && root; i++) {
            row_nodes[i] = root;
            root = root->right;
            count++;
        }

        // Move numbers
        for (int i = 0; i < count; i++)
            printf("Move %d     ", row_nodes[i]->moveNumber);
        printf("\n");

        // Each row of the 3x3 boards
        for (int r = 0; r < 3; r++) {
            for (int i = 0; i < count; i++) {
                for (int c = 0; c < 3; c++)
                    printf("%c ", row_nodes[i]->board[r][c]);
                printf("     ");
            }
            printf("\n");
        }
        printf("\n");
    }
}

// -----(Main Code)-----

// Print the board
void print_board(char board[3][3]) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Check if board is full
int is_board_full(char board[3][3]) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == '_') {
                return 0;
            }
        }
    }
    return 1;
}

// Check for a win
int check_win(char board[3][3], char symbol) {
    for(int i = 0; i < 3; i++) {
        // Rows
        if(board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return 1;
        // Columns
        if(board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return 1;
    }
    // Diagonals
    if(board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return 1;
    if(board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return 1;
    return 0;
}


int flag_first = 0;
int flag_sec = 0;
int flag_sec_mid = 0;
int flag_sec_edge = 0;
int flag_sec_side = 0;

void make_bot_move_easy(char board[3][3], int *botMoveCount, char bot_symbol) {
    int x = rand() % 3;
    int y = rand() % 3;
    while(board[x][y] != '_') {
        x = rand() % 3;
        y = rand() % 3;
    }
    board[x][y] = bot_symbol;
    (*botMoveCount)++;
    add_move(&bot_moves_map, *botMoveCount, x + 1, y + 1);
}

void make_bot_move_med(char board[3][3], int *botMoveCount, char bot_symbol, char user_symbol){
    // Check for a winning move :
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '_') {
                board[i][j] = bot_symbol;
                if (check_win(board, bot_symbol)) {
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                    return;
                }
                board[i][j] = '_'; // Revert if not a win
            }
        }
    }
    
    // Check for a blocking move :
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '_') {
                board[i][j] = user_symbol;
                if (check_win(board, user_symbol)) {
                    board[i][j] = bot_symbol; // Block by placing here
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                    return;
                }
                board[i][j] = '_'; // Revert
            }
        }
    }

    int x = rand() % 3;
    int y = rand() % 3;
    while(board[x][y] != '_') {
        x = rand() % 3;
        y = rand() % 3;
    }
    board[x][y] = bot_symbol;
    (*botMoveCount)++;
    add_move(&bot_moves_map, *botMoveCount, x + 1, y + 1);
}

void make_bot_move_hard(char board[3][3], int *botMoveCount, int userMoveCount, char bot_symbol, char user_symbol) {
    
    if(user_symbol == 'X') flag_sec = 1;

    // Check for a winning move :
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '_') {
                board[i][j] = bot_symbol;
                if (check_win(board, bot_symbol)) {
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                    return;
                }
                board[i][j] = '_'; // Revert if not a win
            }
        }
    }

    // Check for a blocking move :
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '_') {
                board[i][j] = user_symbol;
                if (check_win(board, user_symbol)) {
                    board[i][j] = bot_symbol; // Block by placing here
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                    return;
                }
                board[i][j] = '_'; // Revert
            }
        }
    }

    // Winning Strategy :
    if(flag_sec != 1){  // Bot going first , where flag_sec != 1

        if (*botMoveCount == 0) {
            board[2][2] = bot_symbol;
            (*botMoveCount)++;
            add_move(&bot_moves_map, *botMoveCount, 3, 3);
            return;
        }
    
        if (board[1][1] == user_symbol && *botMoveCount == 1 && bot_symbol == 'X') flag_first = 1;
    
        if (flag_first != 1) {
            if (*botMoveCount == 1) {
                if (board[2][0] == '_' && board[2][1] != user_symbol) {
                    board[2][0] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 3, 1);
                } 
                else if (board[0][2] == '_') {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 1, 3);
                }
            } 
            else if (*botMoveCount == 2) {
                if (board[0][0] == '_' && board[1][0] != user_symbol) {
                    board[0][0] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 1, 1);
                } 
                else if (board[0][2] == '_') {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 1, 3);
                }
            }    
        } 

        else {
            if (*botMoveCount == 1) {
                board[0][0] = bot_symbol;
                (*botMoveCount)++;
                add_move(&bot_moves_map, *botMoveCount, 1, 1);
            } 
        }
    }

    else {   // User going first , where flag_sec == 1

        if(userMoveCount == 1 && board[1][1] == user_symbol) flag_sec_mid = 1;
        else if( (userMoveCount == 1) && ( (board[0][0] == user_symbol) || (board[0][2] == user_symbol) || (board[2][0] == user_symbol) || (board[2][2] == user_symbol) ) ) flag_sec_edge = 1;
        else if( (userMoveCount == 1) && ( (board[0][1] == user_symbol) || (board[1][0] == user_symbol) || (board[1][2] == user_symbol) || (board[2][1] == user_symbol) ) ) flag_sec_side = 1;

        if(flag_sec_mid == 1){
            if(*botMoveCount == 0) {
                board[2][2] = bot_symbol;
                (*botMoveCount)++;
                add_move(&bot_moves_map, *botMoveCount, 3, 3);
            }
            else if(*botMoveCount == 1 && board[0][0] == user_symbol){
                board[2][0] = bot_symbol;
                (*botMoveCount)++;
                add_move(&bot_moves_map, *botMoveCount, 3, 1);
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
                            add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                            return;
                        }
                    }
                }
            }
        }

        else if (flag_sec_edge == 1){    
            if(*botMoveCount == 0) {
                board[1][1] = bot_symbol;
                (*botMoveCount)++;
                add_move(&bot_moves_map, *botMoveCount, 2, 2);
            }
            else if(*botMoveCount == 1){
                if(board[2][2] == user_symbol){
                    if(board[1][0] == user_symbol || board[0][1] == user_symbol) {
                        board[0][0] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 1, 1);
                    }
                    else if(board[0][0] == user_symbol) {
                        board[0][1] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 1, 2);
                    }
                }
                else if(board[0][2] == user_symbol){
                    if(board[1][0] == user_symbol || board[2][1] == user_symbol) {
                        board[2][0] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 3, 1);
                    }
                    else if(board[2][0] == user_symbol) {
                        board[2][1] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 3, 2);
                    }
                }
                else if(board[2][0] == user_symbol){
                    if(board[0][1] == user_symbol || board[1][2] == user_symbol) {
                        board[0][2] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 1, 3);
                    }
                }
                else if(board[0][0] == user_symbol){
                    if(board[2][1] == user_symbol || board[1][2] == user_symbol) {
                        board[2][2] = bot_symbol;
                        (*botMoveCount)++;
                        add_move(&bot_moves_map, *botMoveCount, 3, 3);
                    }
                }
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
                            add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                            return;
                        }
                    }
                }
            }
        }

        else if (flag_sec_side == 1){
            if(*botMoveCount == 0) {
                board[1][1] = bot_symbol;
                (*botMoveCount)++;
                add_move(&bot_moves_map, *botMoveCount, 2, 2);
            }
            else if(*botMoveCount == 1){
                if(board[2][1] == user_symbol) {
                    board[2][0] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 3, 1);
                }
                else if(board[0][1] == user_symbol || board[1][0] == user_symbol) {
                    board[0][0] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 1, 1);
                }
                else if(board[1][2] == user_symbol) {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                    add_move(&bot_moves_map, *botMoveCount, 1, 3);
                }
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
                            add_move(&bot_moves_map, *botMoveCount, i + 1, j + 1);
                            return;
                        }
                    }
                }
            }
        }
    }
}

// Player move (returns 1 if valid, 0 if invalid)
// Now records the player's move into the user moves hashmap.
int make_player_move(char board[3][3], int *userMoveCount, char user_symbol) {

    int x, y;
    printf("\nEnter coordinates (1-based)[1 1]: ");
    scanf("%d %d", &x, &y);

    // Validity
    if(x < 1 || x > 3 || y < 1 || y > 3) {
        printf("Invalid coordinates!\n");
        return 0;
    }
    if(board[x-1][y-1] != '_') {
        printf("Cell occupied!\n");
        return 0;
    }

    // Placement
    board[x-1][y-1] = user_symbol;
    (*userMoveCount)++;
    add_move(&user_moves_map, *userMoveCount, x, y);
    return 1;
}

int main() {
    char board[3][3] = {
        {'_', '_', '_'},
        {'_', '_', '_'},
        {'_', '_', '_'}
    };

    srand(time(0));

    // Choose Symbol
    char user_symbol, bot_symbol, difficulty;
    printf("Choose your symbol (X or O): ");
    scanf(" %c", &user_symbol);

    // Difficulty
    printf("Choose your Difficulty (Easy = E; Medium = M; Hard = H): ");
    scanf(" %c", &difficulty);

    if(difficulty == 'E' || difficulty == 'e') difficulty = 'E';
    else if(difficulty == 'M' || difficulty == 'm') difficulty = 'M';
    else if(difficulty == 'H' || difficulty == 'h') difficulty = 'H';

    if(user_symbol == 'X' || user_symbol == 'x') {
        user_symbol = 'X';
        bot_symbol = 'O';
    } 
    else {
        user_symbol = 'O';
        bot_symbol = 'X';
    }

    // Decide who goes first : 
    // If user picked X, user goes first -> current_player = user
    // If user picked O, bot goes first -> current_player = bot
    char current_player;
    if (user_symbol == 'X') {
        current_player = 'U';
    } 
    else {
        current_player = 'B';
    }

    // Move counters
    int user_moves_count = 0, bot_moves_count = 0;
    int totalMoveCount = 0;

    // Main Loop
    while(is_board_full(board) != 1) {

        if(current_player == 'U') {     // User turn

            if(make_player_move(board, &user_moves_count, user_symbol) != 1) continue; 
            totalMoveCount++;

            // Insert current board state into tree
            boardTree = insert_board_state(boardTree, totalMoveCount, board);

            // Check user win
            if(check_win(board, user_symbol) == 1) {
                print_board(board);
                printf("\nYou win!\n");
                break;
            }
            current_player = 'B'; // Next turn: bot
        }

        else {      // Bot's turn
            
            if(difficulty == 'E') make_bot_move_easy(board, &bot_moves_count, bot_symbol);
            else if(difficulty == 'M'){
                make_bot_move_med(board, &bot_moves_count, bot_symbol, user_symbol);
            }
            else if(difficulty == 'H') {
                make_bot_move_hard(board, &bot_moves_count, user_moves_count, bot_symbol, user_symbol);
            }
            totalMoveCount++;

            // Insert current board state into tree
            boardTree = insert_board_state(boardTree, totalMoveCount, board);

            // Check bot win
            if(check_win(board, bot_symbol) == 1) {
                print_board(board);
                printf("\nBot wins!\n");
                break;
            }
            current_player = 'U'; // Next turn: user
        }

        print_board(board);
        printf("\nYour moves (hashmap): ");
        print_moves_hash(user_moves_map);
        printf("Bot moves (hashmap): ");
        print_moves_hash(bot_moves_map);

    }

    // If board is full and no winner
    if( (is_board_full(board) == 1) && (check_win(board, user_symbol) != 1) && (check_win(board, bot_symbol) != 1) ) {
        print_board(board);
        printf("\nIt's a draw!\n");
    }

    // Print the board state tree
    printf("\nBoard states history:\n");
    print_board_state_tree(boardTree);

}