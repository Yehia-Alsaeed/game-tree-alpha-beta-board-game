#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include "../include/Move.h" 
#include "../include/Stack.h"
#include "../include/GameTree.h"
using namespace std;

const int CELL_SIZE = 70;
const int BOARD_OFFSET_X = 60;
const int BOARD_OFFSET_Y = 60;
const int STATUS_OFFSET_X = BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE + 60;
const int WINDOW_WIDTH = STATUS_OFFSET_X + 350;
const int WINDOW_HEIGHT = BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE + 100;


const int TOKEN_LIMIT = 3;  
const bool AI_ENABLED = true; 

// Game rules and board state
class GameBoard {
private:
    vector<std::vector<char>> board;
    char currentPlayer;
    int playerATokensExited;
    int playerBTokensExited;
    int playerATokensEliminated; // Counts eliminated tokens
    int playerBTokensEliminated; 
    bool aiMode; 
    std::vector<std::pair<int, int>> walls; 
    
    
    bool redJumpJustCompleted;
    int redJumpRow;
    int redJumpCol;

    
    int countPlayerTokens(char player) const {

        int tokensOnBoard = 0;
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == player) {
                    tokensOnBoard++;
                }
            }
        }
        
        
        int tokensExited = (player == PLAYER_A) ? playerATokensExited : playerBTokensExited;
        
        return tokensOnBoard + tokensExited;
    }

public:
    GameBoard() : currentPlayer(PLAYER_A), playerATokensExited(0), playerBTokensExited(0), 
                  playerATokensEliminated(0), playerBTokensEliminated(0), aiMode(AI_ENABLED),
                  redJumpJustCompleted(false), redJumpRow(-1), redJumpCol(-1) {
       
        board.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, EMPTY));
        
        
        for (int i = 0; i < TOKEN_LIMIT; i++) {
            board[i+1][0] = PLAYER_A;  // Left border, starting from row 1
        }
        
       
        for (int i = 0; i < TOKEN_LIMIT; i++) {
            if (i == 0) {
                board[0][3] = PLAYER_B;  
            } else {
                board[0][i] = PLAYER_B;  
            }
        }
        
    
        initializeWalls();
    }

    void initializeWalls() {
        
        walls.push_back(std::make_pair(0, 0)); 
        walls.push_back(std::make_pair(0, 4)); // Top-right corner
        walls.push_back(std::make_pair(4, 0)); // Bottom-left corner
        walls.push_back(std::make_pair(4, 4)); // Bottom-right corner
    }

    bool isWall(int row, int col) const {
        for (const auto& wall : walls) {
            if (wall.first == row && wall.second == col) {
                return true;
            }
        }
        return false;
    }

    char getValueAt(int row, int col) const {
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return ' ';
        if (isWall(row, col)) return WALL;
        return board[row][col];
    }

    char getCurrentPlayer() const { return currentPlayer; }
    int getPlayerATokensExited() const { return playerATokensExited; }
    int getPlayerBTokensExited() const { return playerBTokensExited; }
    int getPlayerATokensEliminated() const { return playerATokensEliminated; }
    int getPlayerBTokensEliminated() const { return playerBTokensEliminated; }
    bool isAIMode() const { return aiMode; }
    
    const std::vector<std::vector<char>>& getBoard() const { return board; }
    const std::vector<std::pair<int, int>>& getWalls() const { return walls; }

    bool isValidMove(const Move& move) const {
       
        if (move.fromRow < 0 || move.fromRow >= BOARD_SIZE || 
            move.fromCol < 0 || move.fromCol >= BOARD_SIZE ||
            move.toRow < 0 || move.toRow >= BOARD_SIZE || 
            move.toCol < 0 || move.toCol >= BOARD_SIZE) {
            return false;
        }
        
        
        if (board[move.fromRow][move.fromCol] != move.player) return false;
        
        
        if (board[move.toRow][move.toCol] != EMPTY || isWall(move.toRow, move.toCol)) return false;
        
        // Player A can only move horizontally to the right.
        if (move.player == PLAYER_A) {
            if (move.isJump) {
                
                if (move.fromRow == move.toRow && move.toCol == move.fromCol + 2 && 
                    board[move.fromRow][move.fromCol + 1] == PLAYER_B) {
                    return true;
                }
            } else {
                
                if (move.toCol == move.fromCol + 1 && move.toRow == move.fromRow) {
                    return true;
                }
            }
        }
        // Player B can only move vertically down.
        else if (move.player == PLAYER_B) {
            if (move.isJump) {
                // Jump down
                if (move.fromCol == move.toCol && move.toRow == move.fromRow + 2 && 
                    board[move.fromRow + 1][move.fromCol] == PLAYER_A) {
                    return true;
                }
            } else {
                
                if (move.toRow == move.fromRow + 1 && move.toCol == move.fromCol) {
                    return true;
                }
            }
        }
        
        return false;
    }

    void makeMove(const Move& move) {
        if (!isValidMove(move)) {
            MessageBox(NULL, "Invalid move!", "Error", MB_OK);
            return;
        }
        
        
        bool isRedPlayerJump = (move.player == PLAYER_A && move.isJump);
        
        
        int jumpedRow = -1;
        int jumpedCol = -1;
        
        if (move.isJump) {
            jumpedRow = (move.fromRow + move.toRow) / 2;
            jumpedCol = (move.fromCol + move.toCol) / 2;
        }
        
        
        if (isRedPlayerJump) {
            
            char debugMsg1[100];
            sprintf(debugMsg1, "RED JUMP: From (%d,%d) to (%d,%d), jumping over (%d,%d)", 
                    move.fromRow, move.fromCol, move.toRow, move.toCol, jumpedRow, jumpedCol);
            OutputDebugStringA(debugMsg1);
            
           
            board[move.toRow][move.toCol] = PLAYER_A;
            
            
            board[jumpedRow][jumpedCol] = EMPTY;
            playerBTokensEliminated++;
            
            
            board[move.fromRow][move.fromCol] = EMPTY;
            
            
            if (board[move.toRow][move.toCol] != PLAYER_A) {
                
                board[move.toRow][move.toCol] = PLAYER_A;
                OutputDebugStringA("WARNING: Had to force red token at destination!");
            }
            
            
            char debugMsg2[100];
            sprintf(debugMsg2, "RED JUMP COMPLETED: Red token at (%d,%d) = %c", 
                    move.toRow, move.toCol, board[move.toRow][move.toCol]);
            OutputDebugStringA(debugMsg2);
            
            
            redJumpJustCompleted = true;
            redJumpRow = move.toRow;
            redJumpCol = move.toCol;
        }
        
        else {
            
            board[move.fromRow][move.fromCol] = EMPTY;
            
            
            board[move.toRow][move.toCol] = move.player;
            
           
            if (move.isJump) {
                board[jumpedRow][jumpedCol] = EMPTY;
                if (move.player == PLAYER_B) {
                    playerATokensEliminated++; 
                }
            }
        }
        
        
        bool tokenExited = false;
        
        if (isRedPlayerJump) {
            
            tokenExited = false;
        }
        else {
            
            if (move.player == PLAYER_A && move.toCol == BOARD_SIZE - 1) {
                
                board[move.toRow][move.toCol] = EMPTY;
                playerATokensExited++;
                tokenExited = true;
            } 
            
            else if (move.player == PLAYER_B && move.toRow == BOARD_SIZE - 1) {
                
                board[move.toRow][move.toCol] = EMPTY;
                playerBTokensExited++;
                tokenExited = true;
            }
        }
        
       
        if (!(tokenExited && (playerATokensExited == TOKEN_LIMIT || playerBTokensExited == TOKEN_LIMIT))) {
            currentPlayer = (currentPlayer == PLAYER_A) ? PLAYER_B : PLAYER_A;
        }
        
       
        if (isGameOver() || (tokenExited && (playerATokensExited == TOKEN_LIMIT || playerBTokensExited == TOKEN_LIMIT))) {
            // Different messages based on who wins
            char winner = getWinner();
            
            if (winner == PLAYER_A) {
                //Lw Player A kesb w kesb ezay
               
                int playerBTotal = countPlayerTokens(PLAYER_B);
                if (playerBTotal == 0) {
                    
                    MessageBox(NULL, "Victory! You eliminated all AI tokens!", "You Win!", MB_OK | MB_ICONINFORMATION);
                } else {
                   
                    MessageBox(NULL, "Victory! la3eeeeeeeeeeb!", "You Win!", MB_OK | MB_ICONINFORMATION);
                }
            } else if (winner == PLAYER_B) {
                int playerATotal = countPlayerTokens(PLAYER_A);
                if (playerATotal == 0) {
                    
                    MessageBox(NULL, "Defeat! The AI has eliminated all your tokens.", "You Lose!", MB_OK | MB_ICONINFORMATION);
                } else {
                   
                    MessageBox(NULL, "Defeat! The AI has beaten you by exiting all its tokens.", "You Lose!", MB_OK | MB_ICONINFORMATION);
                }
            } else if (winner == 'D') {
                
                if (playerATokensExited == playerBTokensExited && playerATokensExited > 0) {
                    MessageBox(NULL, "Draw! Both players have the same number of tokens exited.", "Game Draw", MB_OK | MB_ICONINFORMATION);
                } else {
                    
                    MessageBox(NULL, "Draw! Neither player can make a winning move.", "Game Draw", MB_OK | MB_ICONINFORMATION);
                }
            }
            
            
            int response = MessageBox(NULL, "Do you want to start a new game?", "Game Over", MB_YESNO | MB_ICONQUESTION);
            if (response == IDYES) {
                *this = GameBoard();
               
            }
        }
        else if (aiMode && currentPlayer == PLAYER_B) {
            // AI turn 
            makeAIMove();
        }
    }

    void makeAIMove() {
        
        Move bestMove = findBestMove();
        
        if (bestMove.player != ' ') {
         
            Sleep(500);
            makeMove(bestMove);
        }
        else {
          
            MessageBox(NULL, "AI has no valid moves!", "AI Turn", MB_OK);
            
            
            currentPlayer = PLAYER_A;
        }
    }

    bool isGameOver() const {
        
        if (playerATokensExited == TOKEN_LIMIT || playerBTokensExited == TOKEN_LIMIT) {
            return true;
        }
        
        int playerATokensOnBoard = 0;
        int playerBTokensOnBoard = 0;
        
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) {
                    playerATokensOnBoard++;
                } else if (board[row][col] == PLAYER_B) {
                    playerBTokensOnBoard++;
                }
            }
        }
        
       
        if ((playerATokensOnBoard == 0 && playerATokensExited < TOKEN_LIMIT) || 
            (playerBTokensOnBoard == 0 && playerBTokensExited < TOKEN_LIMIT)) {
            return true;
        }
        
       
        if (playerATokensOnBoard > 0 && playerBTokensOnBoard > 0) {
           
            bool playerACanMove = false;
            for (int row = 0; row < BOARD_SIZE && !playerACanMove; row++) {
                for (int col = 0; col < BOARD_SIZE && !playerACanMove; col++) {
                    if (board[row][col] == PLAYER_A) {
                  
                  if (col + 1 < BOARD_SIZE && board[row][col + 1] == EMPTY && !isWall(row, col + 1)) {
                            playerACanMove = true;
                        }
                       
                        else if (col + 2 < BOARD_SIZE && board[row][col + 1] == PLAYER_B && 
                                board[row][col + 2] == EMPTY && !isWall(row, col + 2)) {
                            playerACanMove = true;
                        }
                    }
                }
            }
            
            
            bool playerBCanMove = false;
            for (int row = 0; row < BOARD_SIZE && !playerBCanMove; row++) {
                for (int col = 0; col < BOARD_SIZE && !playerBCanMove; col++) {
                    if (board[row][col] == PLAYER_B) {
                        
                        if (row + 1 < BOARD_SIZE && board[row + 1][col] == EMPTY && !isWall(row + 1, col)) {
                            playerBCanMove = true;
                        }
                        
                        else if (row + 2 < BOARD_SIZE && board[row + 1][col] == PLAYER_A && 
                                board[row + 2][col] == EMPTY && !isWall(row + 2, col)) {
                            playerBCanMove = true;
                        }
                    }
                }
            }
            
            
            if (!playerACanMove && !playerBCanMove) {
                return true;
            }
        }
        
        return false;
    }

    char getWinner() const {
        if (!isGameOver()) return ' ';
        
        
        if (playerATokensExited == playerBTokensExited && playerATokensExited > 0) {
            return 'D'; 
        }
        
       
        if (playerATokensExited == TOKEN_LIMIT) return PLAYER_A;
        if (playerBTokensExited == TOKEN_LIMIT) return PLAYER_B;
        
       
        if (playerATokensExited > playerBTokensExited) return PLAYER_A;
        if (playerBTokensExited > playerATokensExited) return PLAYER_B;
        
       
        int playerATokensOnBoard = 0;
        int playerBTokensOnBoard = 0;
        
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) {
                    playerATokensOnBoard++;
                } else if (board[row][col] == PLAYER_B) {
                    playerBTokensOnBoard++;
                }
            }
        }
        
        
        if (playerATokensOnBoard == 0 && playerATokensExited < TOKEN_LIMIT) {
            return PLAYER_B;
        }
        
        
        if (playerBTokensOnBoard == 0 && playerBTokensExited < TOKEN_LIMIT) {
            return PLAYER_A;
        }
        
        
        if (playerATokensOnBoard > 0 && playerBTokensOnBoard > 0) {
           
            bool playerACanMove = false;
            for (int row = 0; row < BOARD_SIZE && !playerACanMove; row++) {
                for (int col = 0; col < BOARD_SIZE && !playerACanMove; col++) {
                    if (board[row][col] == PLAYER_A) {
                     
                        if (col + 1 < BOARD_SIZE && board[row][col + 1] == EMPTY && !isWall(row, col + 1)) {
                            playerACanMove = true;
                        }
                       
                        else if (col + 2 < BOARD_SIZE && board[row][col + 1] == PLAYER_B && 
                                board[row][col + 2] == EMPTY && !isWall(row, col + 2)) {
                            playerACanMove = true;
                        }
                    }
                }
            }
            
            
            bool playerBCanMove = false;
            for (int row = 0; row < BOARD_SIZE && !playerBCanMove; row++) {
                for (int col = 0; col < BOARD_SIZE && !playerBCanMove; col++) {
                    if (board[row][col] == PLAYER_B) {
                        
                        if (row + 1 < BOARD_SIZE && board[row + 1][col] == EMPTY && !isWall(row + 1, col)) {
                            playerBCanMove = true;
                        }
                        
                        else if (row + 2 < BOARD_SIZE && board[row + 1][col] == PLAYER_A && 
                                board[row + 2][col] == EMPTY && !isWall(row + 2, col)) {
                            playerBCanMove = true;
                        }
                    }
                }
            }
            
            
            if (!playerACanMove && !playerBCanMove) {
                return 'D'; 
            }
        }
        
        return ' ';
    }

    std::vector<Move> getValidMoves() const {
        std::vector<Move> validMoves;
        
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == currentPlayer) {
                    
                    if (currentPlayer == PLAYER_A) {
                       
                        Move moveRight(row, col, row, col + 1, false, PLAYER_A);
                        if (col + 1 < BOARD_SIZE && isValidMove(moveRight)) {
                            validMoves.push_back(moveRight);
                        }
                        
                        
                        Move jumpRight(row, col, row, col + 2, true, PLAYER_A);
                        if (col + 2 < BOARD_SIZE && isValidMove(jumpRight)) {
                            validMoves.push_back(jumpRight);
                        }
                    }
                    
                    else {
                        
                        Move moveDown(row, col, row + 1, col, false, PLAYER_B);
                        if (row + 1 < BOARD_SIZE && isValidMove(moveDown)) {
                            validMoves.push_back(moveDown);
                        }
                        
                        
                        Move jumpDown(row, col, row + 2, col, true, PLAYER_B);
                        if (row + 2 < BOARD_SIZE && isValidMove(jumpDown)) {
                            validMoves.push_back(jumpDown);
                        }
                    }
                }
            }
        }
        
        return validMoves;
    }

    
    Move findBestMove() {
        
        GameTree gameTree(board, currentPlayer, playerATokensExited, playerBTokensExited, walls);
        
       
        return gameTree.findBestMoveWithBacktracking();
    }
};


HWND hwnd;
HDC hdc;
HBRUSH hBrushA, hBrushB, hBrushEmpty, hBrushBackground, hBrushGrid, hBrushWall;
HFONT hFont;
GameBoard gameBoard;
int selectedRow = -1;
int selectedCol = -1;
bool showValidMoves = false;
std::vector<Move> validMovesForSelected;


void DrawBoard() {
    
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, hBrushBackground);
    
    
    RECT boardRect;
    boardRect.left = BOARD_OFFSET_X - 10;
    boardRect.top = BOARD_OFFSET_Y - 10;
    boardRect.right = BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE + 10;
    boardRect.bottom = BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE + 10;
    
    // Draw the board border.
    HPEN hPenBorder = CreatePen(PS_SOLID, 3, RGB(60, 60, 60));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenBorder);
    
    Rectangle(hdc, boardRect.left, boardRect.top, boardRect.right, boardRect.bottom);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPenBorder);
    
    // Draw grid
    for (int row = 0; row <= BOARD_SIZE; row++) {
        MoveToEx(hdc, BOARD_OFFSET_X, BOARD_OFFSET_Y + row * CELL_SIZE, NULL);
        LineTo(hdc, BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE, BOARD_OFFSET_Y + row * CELL_SIZE);
    }
    
    for (int col = 0; col <= BOARD_SIZE; col++) {
        MoveToEx(hdc, BOARD_OFFSET_X + col * CELL_SIZE, BOARD_OFFSET_Y, NULL);
        LineTo(hdc, BOARD_OFFSET_X + col * CELL_SIZE, BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE);
    }
    
    
    HPEN hPenExitA = CreatePen(PS_SOLID, 3, RGB(220, 50, 50)); //red
    HPEN hPenExitB = CreatePen(PS_SOLID, 3, RGB(50, 220, 50)); //green
    
    
    SelectObject(hdc, hPenExitA);
    MoveToEx(hdc, BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE, BOARD_OFFSET_Y, NULL);
    LineTo(hdc, BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE, BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE);
    
    
    SelectObject(hdc, hPenExitB);
    MoveToEx(hdc, BOARD_OFFSET_X, BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE, NULL);
    LineTo(hdc, BOARD_OFFSET_X + BOARD_SIZE * CELL_SIZE, BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPenExitA);
    DeleteObject(hPenExitB);
    
   
    for (const auto& wall : gameBoard.getWalls()) {
        int row = wall.first;
        int col = wall.second;
        
        RECT wallRect;
        wallRect.left = BOARD_OFFSET_X + col * CELL_SIZE + 1;
        wallRect.top = BOARD_OFFSET_Y + row * CELL_SIZE + 1;
        wallRect.right = wallRect.left + CELL_SIZE - 2;
        wallRect.bottom = wallRect.top + CELL_SIZE - 2;
        
        
        SelectObject(hdc, hBrushWall);
        Rectangle(hdc, wallRect.left, wallRect.top, wallRect.right, wallRect.bottom);
    }
    
   
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            
            if (gameBoard.isWall(row, col)) continue;
            
            char value = gameBoard.getValueAt(row, col);
            RECT cellRect;
            cellRect.left = BOARD_OFFSET_X + col * CELL_SIZE + 8;
            cellRect.top = BOARD_OFFSET_Y + row * CELL_SIZE + 8;
            cellRect.right = cellRect.left + CELL_SIZE - 16;
            cellRect.bottom = cellRect.top + CELL_SIZE - 16;
            
            if (value == PLAYER_A) {
                
                Ellipse(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);
                SelectObject(hdc, hBrushA);
                Ellipse(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);
                
                
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 255));
                DrawText(hdc, "A", 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            } 
            else if (value == PLAYER_B) {
               
                Ellipse(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);
                SelectObject(hdc, hBrushB);
                Ellipse(hdc, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);
                
               
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 255));
                DrawText(hdc, "B", 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            
            
            if (row == selectedRow && col == selectedCol) {
                RECT selectedRect;
                selectedRect.left = BOARD_OFFSET_X + col * CELL_SIZE + 3;
                selectedRect.top = BOARD_OFFSET_Y + row * CELL_SIZE + 3;
                selectedRect.right = selectedRect.left + CELL_SIZE - 6;
                selectedRect.bottom = selectedRect.top + CELL_SIZE - 6;
                
                
                HPEN hPenSelected = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPenSelected);
                
                SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, selectedRect.left, selectedRect.top, selectedRect.right, selectedRect.bottom);
                
                SelectObject(hdc, hOldPen);
                DeleteObject(hPenSelected);
            }
        }
    }
    
    
    if (showValidMoves && selectedRow != -1 && selectedCol != -1) {
        for (const auto& move : validMovesForSelected) {
            RECT moveRect;
            moveRect.left = BOARD_OFFSET_X + move.toCol * CELL_SIZE + 20;
            moveRect.top = BOARD_OFFSET_Y + move.toRow * CELL_SIZE + 20;
            moveRect.right = moveRect.left + CELL_SIZE - 40;
            moveRect.bottom = moveRect.top + CELL_SIZE - 40;
            
            
            HBRUSH hBrushHint = CreateSolidBrush(RGB(100, 200, 255));
            SelectObject(hdc, hBrushHint);
            Ellipse(hdc, moveRect.left, moveRect.top, moveRect.right, moveRect.bottom);
            DeleteObject(hBrushHint);
        }
    }
    
    
    RECT statusRect;
    statusRect.left = STATUS_OFFSET_X - 10;
    statusRect.top = BOARD_OFFSET_Y - 10;
    statusRect.right = WINDOW_WIDTH - 20;
    statusRect.bottom = BOARD_OFFSET_Y + BOARD_SIZE * CELL_SIZE + 220;
    
    
    HPEN hPenStatus = CreatePen(PS_SOLID, 2, RGB(80, 80, 80));
    SelectObject(hdc, hPenStatus);
    
    HBRUSH hBrushStatus = CreateSolidBrush(RGB(240, 240, 245));
    SelectObject(hdc, hBrushStatus);
    
    RoundRect(hdc, statusRect.left, statusRect.top, statusRect.right, statusRect.bottom, 20, 20);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hPenStatus);
    DeleteObject(hBrushStatus);
    
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    RECT textRect;
    textRect.left = STATUS_OFFSET_X + 10;
    textRect.top = BOARD_OFFSET_Y + 20;
    textRect.right = WINDOW_WIDTH - 40;
    textRect.bottom = textRect.top + 30;
    
    
    std::string modeText = "Game Mode: ";
    modeText += gameBoard.isAIMode() ? "Player vs AI" : "Player vs Player";
    DrawText(hdc, modeText.c_str(), modeText.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
   
    textRect.top += 40;
    textRect.bottom = textRect.top + 30;
    
    std::string playerText = "Current Player: ";
    if (gameBoard.getCurrentPlayer() == PLAYER_A) {
        playerText += "Red (A)";
        if (gameBoard.isAIMode()) {
            playerText += " - Green(B)";
        }
    } else {
        playerText += "Green (B)";
        if (gameBoard.isAIMode()) {
            playerText += " - AI Turn";
        }
    }
    DrawText(hdc, playerText.c_str(), playerText.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
    
    textRect.top += 40;
    textRect.bottom = textRect.top + 30;
    
    std::string exitedTextA = "Red tokens exited: ";
    exitedTextA += std::to_string(gameBoard.getPlayerATokensExited());
    exitedTextA += " of ";
    exitedTextA += std::to_string(TOKEN_LIMIT);
    DrawText(hdc, exitedTextA.c_str(), exitedTextA.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    
    std::string exitedTextB = "Green tokens exited: ";
    exitedTextB += std::to_string(gameBoard.getPlayerBTokensExited());
    exitedTextB += " of ";
    exitedTextB += std::to_string(TOKEN_LIMIT);
    DrawText(hdc, exitedTextB.c_str(), exitedTextB.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
    
    textRect.top += 40;
    textRect.bottom = textRect.top + 30;
    
    std::string eliminatedTextA = "Red tokens eliminated: ";
    eliminatedTextA += std::to_string(gameBoard.getPlayerATokensEliminated());
    DrawText(hdc, eliminatedTextA.c_str(), eliminatedTextA.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    
    std::string eliminatedTextB = "Green tokens eliminated: ";
    eliminatedTextB += std::to_string(gameBoard.getPlayerBTokensEliminated());
    DrawText(hdc, eliminatedTextB.c_str(), eliminatedTextB.length(), &textRect, DT_LEFT | DT_SINGLELINE);
    
    
    textRect.top += 50;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "Game Rules:", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- Red moves right only", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- Green moves down only", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- Jump over opponent's token", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- First to exit all tokens wins", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- Blue squares are walls", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    
    textRect.top += 50;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "Controls:", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- H: Show hint (best move)", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- V: Show/hide valid moves", -1, &textRect, DT_LEFT | DT_SINGLELINE);
    
    textRect.top += 30;
    textRect.bottom = textRect.top + 30;
    DrawText(hdc, "- N: New game", -1, &textRect, DT_LEFT | DT_SINGLELINE);
}


void HandleBoardClick(int x, int y) {
   
    int col = (x - BOARD_OFFSET_X) / CELL_SIZE;
    int row = (y - BOARD_OFFSET_Y) / CELL_SIZE;
    
    
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return;
    }
    
    
    if (gameBoard.isWall(row, col)) {
        return;
    }
    
    
    char currentPlayer = gameBoard.getCurrentPlayer();
    
   
    if (gameBoard.isAIMode() && currentPlayer == PLAYER_B) {
        MessageBox(hwnd, "It's AI's turn. Please wait.", "AI Turn", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    
    if (selectedRow != -1 && selectedCol != -1) {
        
        bool validDestination = false;
        Move selectedMove;
        
        for (const auto& move : validMovesForSelected) {
            if (move.toRow == row && move.toCol == col) {
                validDestination = true;
                selectedMove = move;
                break;
            }
        }
        
        if (validDestination) {
           
            gameBoard.makeMove(selectedMove);
            
            
            selectedRow = -1;
            selectedCol = -1;
            validMovesForSelected.clear();
            
            
            InvalidateRect(hwnd, NULL, TRUE);
        } else {
            
            if (gameBoard.getValueAt(row, col) == currentPlayer) {
               
                selectedRow = row;
                selectedCol = col;
                
               
                validMovesForSelected.clear();
                
                
                for (int r = 0; r < BOARD_SIZE; r++) {
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        Move move(selectedRow, selectedCol, r, c, false, currentPlayer);
                        
                        
                        if (abs(r - selectedRow) == 2 || abs(c - selectedCol) == 2) {
                            move.isJump = true;
                        }
                        
                        if (gameBoard.isValidMove(move)) {
                            validMovesForSelected.push_back(move);
                        }
                    }
                }
                
                
                InvalidateRect(hwnd, NULL, TRUE);
            } else {
                
                selectedRow = -1;
                selectedCol = -1;
                validMovesForSelected.clear();
                
                
                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    } else {
        
        if (gameBoard.getValueAt(row, col) == currentPlayer) {
            
            selectedRow = row;
            selectedCol = col;
            
            
            validMovesForSelected.clear();
            
            
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    Move move(selectedRow, selectedCol, r, c, false, currentPlayer);
                    
                    
                    if (abs(r - selectedRow) == 2 || abs(c - selectedCol) == 2) {
                        move.isJump = true;
                    }
                    
                    if (gameBoard.isValidMove(move)) {
                        validMovesForSelected.push_back(move);
                    }
                }
            }
            
            
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
}


void ShowHint() {
    
    char currentPlayer = gameBoard.getCurrentPlayer();
    
    
    if (gameBoard.isAIMode() && currentPlayer == PLAYER_B) {
        MessageBox(hwnd, "It's AI's turn. No hint needed.", "AI Turn", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    
    Move bestMove = gameBoard.findBestMove();
    
    if (bestMove.player != ' ') {
        
        selectedRow = bestMove.fromRow;
        selectedCol = bestMove.fromCol;
        
        
        validMovesForSelected.clear();
        validMovesForSelected.push_back(bestMove);
        
        
        showValidMoves = true;
        
        
        InvalidateRect(hwnd, NULL, TRUE);
        
        
        MessageBox(hwnd, "Hint: The best move is highlighted.", "Hint", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hwnd, "No valid moves available.", "Hint", MB_OK | MB_ICONINFORMATION);
    }
}


void ToggleShowValidMoves() {
    showValidMoves = !showValidMoves;
    

    InvalidateRect(hwnd, NULL, TRUE);
}


void StartNewGame() {
    gameBoard = GameBoard();
    
    selectedRow = -1;
    selectedCol = -1;
    validMovesForSelected.clear();
    
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            hdc = GetDC(hwnd);
            
            hBrushA = CreateSolidBrush(RGB(220, 50, 50));     
            hBrushB = CreateSolidBrush(RGB(50, 220, 50));     
            hBrushEmpty = CreateSolidBrush(RGB(240, 240, 240)); 
            hBrushBackground = CreateSolidBrush(RGB(220, 220, 230)); 
            hBrushGrid = CreateSolidBrush(RGB(200, 200, 200)); 
            hBrushWall = CreateSolidBrush(RGB(100, 100, 180)); 
            
            
            hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, "Arial");
            SelectObject(hdc, hFont);
            
            return 0;
            
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                
            
                DrawBoard();
                
                EndPaint(hwnd, &ps);
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                
            
                HandleBoardClick(xPos, yPos);
            }
            return 0;
            
        case WM_KEYDOWN:
            switch (wParam) {
                case 'H': 
                    ShowHint();
                    break;
                    
                case 'V': 
                    ToggleShowValidMoves();
                    break;
                    
                case 'N': 
                    StartNewGame();
                    break;
            }
            return 0;
            
        case WM_DESTROY:
            DeleteObject(hBrushA);
            DeleteObject(hBrushB);
            DeleteObject(hBrushEmpty);
            DeleteObject(hBrushBackground);
            DeleteObject(hBrushGrid);
            DeleteObject(hBrushWall);
            DeleteObject(hFont);
            ReleaseDC(hwnd, hdc);
            
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "GameTreesWindow";
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "GameTreesWindow",
        "Game Trees - With Walls",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return msg.wParam;
}
