#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "raylib.h" // Include the Raylib library (so we can use its functions)

// -------

// To Check :
// Use : cd C:\raylib\raylib\raylib-wasm
// emmake make
// python -m http.server 8000
// visit : http://localhost:8000

// -------

// Window: 900 × 900
// Each cell: 200 × 200
// Board size = 3 × 200 = 600
// Margin = (Window - Board) ÷ 2 = (900 - 600) ÷ 2 = 150 pixels margin on each side
// So the board should start drawing at (150, 150)

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 900
#define CELL_SIZE 200
#define BOARD_OFFSET_X 150
#define BOARD_OFFSET_Y 150
                    
#define CREAM (Color){ 251, 237, 195, 255 }

// ------ (Initial Menu) ------

bool chooseXOActive = true;
bool chooseDifficultyActive = false;

char user_symbol, difficulty;

void Overlay_Choose_XO() {

    // Button size
    int buttonWidth = 180;
    int buttonHeight = 180;
    int spacing = 40; // space between buttons

    // Center X coordinate for both buttons combined
    int totalWidth = 2 * buttonWidth + spacing;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int y = SCREEN_HEIGHT / 2 - buttonHeight / 2;

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));   // 60% opacity and 40% transparency
    DrawText("Choose X or O", 227.5, 250, 60, WHITE);

    Rectangle xButton = { startX, y, buttonWidth, buttonHeight };
    Rectangle oButton = { startX + buttonWidth + spacing, y, buttonWidth, buttonHeight };

    DrawRectangleRec(xButton, RED);
    DrawText("X", xButton.x + 60, xButton.y + 45, 100, WHITE);

    DrawRectangleRec(oButton, BLUE);
    DrawText("O", oButton.x + 60, oButton.y + 45, 100, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        if (CheckCollisionPointRec(mouse, xButton)) {
            user_symbol = 'X';
            chooseXOActive = false;
            chooseDifficultyActive = true;
        } else if (CheckCollisionPointRec(mouse, oButton)) {
            user_symbol = 'O';
            chooseXOActive = false;
            chooseDifficultyActive = true;
        }
    }
}

void Overlay_Choose_Difficulty() {

    // Difficulty button size
    int diffButtonWidth = 180;
    int diffButtonHeight = 100;
    int diffSpacing = 40;

    // Center X coordinate for 3 buttons
    int totalDiffWidth = 3 * diffButtonWidth + 2 * diffSpacing;
    int diffStartX = (SCREEN_WIDTH - totalDiffWidth) / 2;
    int diffY = SCREEN_HEIGHT / 2 - diffButtonHeight / 2 + 10; // Vertically center + offset


    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
    DrawText("Choose Difficulty", 240, 300, 50, WHITE);

    Rectangle easyBtn =   { diffStartX, diffY, diffButtonWidth, diffButtonHeight };
    Rectangle mediumBtn = { diffStartX + diffButtonWidth + diffSpacing, diffY, diffButtonWidth, diffButtonHeight };
    Rectangle hardBtn =   { diffStartX + 2 * (diffButtonWidth + diffSpacing), diffY, diffButtonWidth, diffButtonHeight };

    DrawRectangleRec(easyBtn, GREEN); DrawText("Easy", easyBtn.x + 40, easyBtn.y + 30, 40, BLACK);
    DrawRectangleRec(mediumBtn, YELLOW); DrawText("Medium", mediumBtn.x + 20, mediumBtn.y + 30, 40, BLACK);
    DrawRectangleRec(hardBtn, RED); DrawText("Hard", hardBtn.x + 40, hardBtn.y + 30, 40, BLACK);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, easyBtn)) {
            difficulty = 'E';
            chooseDifficultyActive = false;
        } 
        else if (CheckCollisionPointRec(mouse, mediumBtn)) {
            difficulty = 'M';
            chooseDifficultyActive = false;
        } 
        else if (CheckCollisionPointRec(mouse, hardBtn)) {
            difficulty = 'H';
            chooseDifficultyActive = false;
        }
    }
}

// ------ (Logic) ------

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

// ------ (Bot Moves) ------ 

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
}

void make_bot_move_med(char board[3][3], int *botMoveCount, char bot_symbol, char user_symbol){
    
    // Check for a winning move :
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '_') {
                board[i][j] = bot_symbol;
                if (check_win(board, bot_symbol)) {
                    (*botMoveCount)++;
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
            return;
        }
    
        if (board[1][1] == user_symbol && *botMoveCount == 1 && bot_symbol == 'X') flag_first = 1;
    
        if (flag_first != 1) {
            if (*botMoveCount == 1) {
                if (board[2][0] == '_' && board[2][1] != user_symbol) {
                    board[2][0] = bot_symbol;
                    (*botMoveCount)++;
                } 
                else if (board[0][2] == '_') {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                }
            } 
            else if (*botMoveCount == 2) {
                if (board[0][0] == '_' && board[1][0] != user_symbol) {
                    board[0][0] = bot_symbol;
                    (*botMoveCount)++;
                } 
                else if (board[0][2] == '_') {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                }
            }    
        } 

        else {
            if (*botMoveCount == 1) {
                board[0][0] = bot_symbol;
                (*botMoveCount)++;
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
            }
            else if(*botMoveCount == 1 && board[0][0] == user_symbol){
                board[2][0] = bot_symbol;
                (*botMoveCount)++;
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
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
            }
            else if(*botMoveCount == 1){
                if(board[2][2] == user_symbol){
                    if(board[1][0] == user_symbol || board[0][1] == user_symbol) {
                        board[0][0] = bot_symbol;
                        (*botMoveCount)++;
                    }
                    else if(board[0][0] == user_symbol) {
                        board[0][1] = bot_symbol;
                        (*botMoveCount)++;
                    }
                }
                else if(board[0][2] == user_symbol){
                    if(board[1][0] == user_symbol || board[2][1] == user_symbol) {
                        board[2][0] = bot_symbol;
                        (*botMoveCount)++;
                    }
                    else if(board[2][0] == user_symbol) {
                        board[2][1] = bot_symbol;
                        (*botMoveCount)++;
                    }
                }
                else if(board[2][0] == user_symbol){
                    if(board[0][1] == user_symbol || board[1][2] == user_symbol) {
                        board[0][2] = bot_symbol;
                        (*botMoveCount)++;
                    }
                }
                else if(board[0][0] == user_symbol){
                    if(board[2][1] == user_symbol || board[1][2] == user_symbol) {
                        board[2][2] = bot_symbol;
                        (*botMoveCount)++;
                    }
                }
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
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
            }
            else if(*botMoveCount == 1){
                if(board[2][1] == user_symbol) {
                    board[2][0] = bot_symbol;
                    (*botMoveCount)++;
                }
                else if(board[0][1] == user_symbol || board[1][0] == user_symbol) {
                    board[0][0] = bot_symbol;
                    (*botMoveCount)++;
                }
                else if(board[1][2] == user_symbol) {
                    board[0][2] = bot_symbol;
                    (*botMoveCount)++;
                }
            }
            else if(*botMoveCount >= 2){
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 3; j++){
                        if(board[i][j] == '_'){
                            board[i][j] = bot_symbol;
                            (*botMoveCount)++;
                            return;
                        }
                    }
                }
            }
        }
    }
}

// ------ (Reset Code)------

void Reset_Game_State(char board[3][3]) {
    
    // Clear the board
    memset(board, '_', sizeof(char) * 9);
    
    // Reset the flags
    chooseXOActive = true;
    chooseDifficultyActive = false;
    flag_first   = 0;
    flag_sec     = 0;
    flag_sec_mid = 0;
    flag_sec_edge= 0;
    flag_sec_side= 0;
    
}

int Restart_Button(){

    int buttonWidth = 200;
    int buttonHeight = 90;

    int x = ((SCREEN_WIDTH - buttonWidth) / 2);
    int y = ((SCREEN_HEIGHT / 2) - (buttonHeight / 2) + 375);
    
    Rectangle restartBtn = { x, y, buttonWidth, buttonHeight };

    DrawRectangleRec(restartBtn, ORANGE);
    DrawText("Restart", restartBtn.x + 20, restartBtn.y + 25, 40, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, restartBtn)) {
            return 1;               
        }
    }
    return 0;

}

// ------ (Main Code) ------

int main(){

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic Tac Toe");

    SetTargetFPS(60);
    
    char board[3][3] = {
        {'_', '_', '_'},
        {'_', '_', '_'},
        {'_', '_', '_'}
    };

    srand(time(0));

    char bot_symbol, current_player;

    int user_moves_count = 0;
    int bot_moves_count = 0;

    char game_state = 'P';      // If the game state is playing
    bool game_initialize = false;

    // The main game loop — keeps running until we close the window
    while (!WindowShouldClose()){ // Checks if the user closed the window

        // Start drawing the screen
        BeginDrawing();

        // Set background color to black
        ClearBackground(BLACK);

        // ------ (Start of the Display) ------ 

        // ( Part-1 ) Use the Overlays :
        if (chooseXOActive == true) Overlay_Choose_XO();
        else if (chooseDifficultyActive == true) Overlay_Choose_Difficulty();    
        
        else {      
            if (game_initialize == false){
                
                if(user_symbol == 'X') bot_symbol = 'O';
                else if (user_symbol == 'O') bot_symbol = 'X';

                if (user_symbol == 'X') current_player = 'U';
                else if (user_symbol == 'O') current_player = 'B';

                game_initialize = true;
            }

            // ( Part-2 ) Take the inputs :
            if(game_state == 'P'){

                if(current_player == 'U'){
    
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){   // Check if the left mouse button was clicked    
                        Vector2 mousePos = GetMousePosition();      // Get the current mouse position on the screen
                
                        // Convert the pixel position to a grid cell index (0 to 2)
                        int col = (mousePos.x - BOARD_OFFSET_X) / CELL_SIZE;
                        int row = (mousePos.y - BOARD_OFFSET_Y) / CELL_SIZE;
                
                        // Make sure we clicked inside the grid
                        if (row >= 0 && row < 3 && col >= 0 && col < 3){
                            // If the cell is empty, mark it with the current player's symbol
                            if (board[row][col] == '_'){    
                                    
                                board[row][col] = user_symbol; 
                                user_moves_count++; 
    
                                if(check_win(board, user_symbol) == 1) game_state = 'U';
                                else if (is_board_full(board) == 1) game_state = 'D';
                                else current_player = 'B';
    
                            }
                        }
                
                    }
    
                }   
                else { 
    
                    if(difficulty == 'E') make_bot_move_easy(board, &bot_moves_count, bot_symbol);
                    else if (difficulty == 'M') make_bot_move_med(board, &bot_moves_count, bot_symbol, user_symbol);
                    else if (difficulty == 'H') make_bot_move_hard(board, &bot_moves_count, user_moves_count, bot_symbol, user_symbol);
    
                    if(check_win(board, bot_symbol) == 1) game_state = 'B';
                    else if (is_board_full(board) == 1) game_state = 'D';
                    else current_player = 'U';
    
                }
            }

            // ( Part-3 ) Draw the 3x3 grid and show X or O if the cell is marked :
            for (int row = 0; row < 3; row++){

                for (int col = 0; col < 3; col++){

                    // (Normally) :
                    // int x = col * CELL_SIZE;     
                    // int y = row * CELL_SIZE;

                    int x = BOARD_OFFSET_X + col * CELL_SIZE;
                    int y = BOARD_OFFSET_Y + row * CELL_SIZE;

                    // Draw a square outline for the cell (black border) with our custom border thickness
                    // DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK); (Normally)
                    Rectangle cell = { x, y, CELL_SIZE, CELL_SIZE };
                    DrawRectangleLinesEx(cell, 4, CREAM); 

                    if (board[row][col] == user_symbol){

                        // To get perfect allignment :
                        if(user_symbol == 'X'){
                            const char* symbol = "X";
                            // As the cell_size is 200x200 I want the symbol to take half the space 100x100 
                            int fontSize = 100;     
                            int textWidth = MeasureText(symbol, fontSize);
                            DrawText(symbol, x + (CELL_SIZE - textWidth)/2, y + (CELL_SIZE - fontSize)/2, fontSize, RED);
                        } 
                        else if(user_symbol == 'O'){
                            const char* symbol = "O";
                            int fontSize = 100;
                            int textWidth = MeasureText(symbol, fontSize);
                            DrawText(symbol, x + (CELL_SIZE - textWidth)/2, y + (CELL_SIZE - fontSize)/2, fontSize, BLUE);
                        }

                    }
                    else if (board[row][col] == bot_symbol){

                        // (Normally) :
                        // DrawText("O", x + 70, y + 50, 150, BLUE);

                        // Perfect allignment :
                        if(bot_symbol == 'X'){
                            const char* symbol = "X";
                            int fontSize = 100;     
                            int textWidth = MeasureText(symbol, fontSize);
                            DrawText(symbol, x + (CELL_SIZE - textWidth)/2, y + (CELL_SIZE - fontSize)/2, fontSize, RED);
                        } 
                        else if(bot_symbol == 'O'){
                            const char* symbol = "O";
                            int fontSize = 100;
                            int textWidth = MeasureText(symbol, fontSize);
                            DrawText(symbol, x + (CELL_SIZE - textWidth)/2, y + (CELL_SIZE - fontSize)/2, fontSize, BLUE);
                        }
                    }

                }
            }
    

            // ( Part-4 ) Check user/bot win/draw :
            if(game_state == 'U') {

                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,180});
                const char* msg = "You WIN!";
                int fontSize = 100;
                int textWidth = MeasureText(msg, fontSize);
                DrawText(msg, (SCREEN_WIDTH - textWidth)/2, (SCREEN_HEIGHT)/2 - 30, fontSize, BLUE);
                
                if(Restart_Button() == 1) {
                    Reset_Game_State(board);
                    bot_moves_count = 0;
                    user_moves_count = 0;
                    game_state = 'P';
                    game_initialize = false;
                }  

            }
            else if(game_state == 'B') {

                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,180});
                const char* msg = "Bot WIN!";
                int fontSize = 100;
                int textWidth = MeasureText(msg, fontSize);
                DrawText(msg, (SCREEN_WIDTH - textWidth)/2, (SCREEN_HEIGHT)/2 - 30, fontSize, RED);

                if(Restart_Button() == 1) {
                    Reset_Game_State(board);
                    bot_moves_count = 0;
                    user_moves_count = 0;
                    game_state = 'P';
                    game_initialize = false;
                }

            }
            else if(game_state == 'D') {

                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0,0,0,180});
                const char* msg = "Draw!";
                int fontSize = 100;
                int textWidth = MeasureText(msg, fontSize);
                DrawText(msg, (SCREEN_WIDTH - textWidth)/2, (SCREEN_HEIGHT)/2 - 30, fontSize, CREAM);

                if(Restart_Button() == 1) {
                    Reset_Game_State(board);
                    bot_moves_count  = 0;
                    user_moves_count = 0;
                    game_state = 'P';
                    game_initialize = false;
                }

            }
            
        }

        // Finish
        EndDrawing();  

    }
    // Close the window and free resources
    CloseWindow();
}