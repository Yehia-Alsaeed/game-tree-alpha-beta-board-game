#ifndef GAME_TREE_H
#define GAME_TREE_H

#include <vector>
#include <utility>
#include "Move.h"
#include "Stack.h"


const int BOARD_SIZE = 5;
const char PLAYER_A = 'A';
const char PLAYER_B = 'B';
const char EMPTY = '.';
const char WALL = 'W';


// recursive backtracking
 
class GameTree {
private:
    std::vector<std::vector<char>> board;
    char currentPlayer;
    int playerATokensExited;
    int playerBTokensExited;
    int maxDepth; 
    std::vector<std::pair<int, int>> walls; 

public:
    
    GameTree(const std::vector<std::vector<char>>& initialBoard, 
             char player, 
             int aTokensExited, 
             int bTokensExited,
             const std::vector<std::pair<int, int>>& wallPositions,
             int depth = 8) // Increased depth for smarter AI
        : board(initialBoard), 
          currentPlayer(player), 
          playerATokensExited(aTokensExited), 
          playerBTokensExited(bTokensExited),
          walls(wallPositions),
          maxDepth(depth) {}

    bool isWall(int row, int col) const {
        for (const auto& wall : walls) {
            if (wall.first == row && wall.second == col) {
                return true;
            }
        }
        return false;
    }

    
    bool isGoodState(int depth = 0) {
        // Base case
        if (depth >= maxDepth) {
            return evaluatePosition() > 0;
        }
        
       
        if (isGameOver()) {
            return getWinner() == currentPlayer;
        }
        
      
        std::vector<Move> moves = getValidMoves();
        for (const Move& move : moves) {
            GameTree nextState = applyMove(move);
            if (!nextState.isGoodState(depth + 1)) {
                return true;  
            }
        }
        
        return false;  
    }
    
    
    bool isBadState(int depth = 0) {
        
        if (depth >= maxDepth) {
            return evaluatePosition() < 0;
        }
        
        
        if (isGameOver()) {
            return getWinner() != currentPlayer;
        }
        
        std::vector<Move> moves = getValidMoves();
        if (moves.empty()) {
            return true;  
        }
        
        for (const Move& move : moves) {
            GameTree nextState = applyMove(move);
            if (!nextState.isGoodState(depth + 1)) {
                return false;
            }
        }
        
        return true;  
    }
    
    
    Move findBestMoveWithBacktracking() {
        std::vector<Move> validMoves = getValidMoves();
        if (validMoves.empty()) return Move();
        
        
        Move bestMove = validMoves[0];
        int bestScore = -100000;
        int alpha = -100000;
        int beta = 100000;
        
       
        for (const Move& move : validMoves) {
            GameTree nextState = applyMove(move);
            int score = alphaBetaMin(nextState, alpha, beta, 1);
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            
            alpha = std::max(alpha, bestScore);
        }
        
        return bestMove;
    }
    
   
    int alphaBetaMax(const GameTree& state, int alpha, int beta, int depth) {
        
        if (state.isGameOver() || depth >= maxDepth) {
            return state.evaluatePosition();
        }
        
        std::vector<Move> moves = state.getValidMoves();
        if (moves.empty()) {
            return state.evaluatePosition();
        }
        
        int value = -100000;
        
        for (const Move& move : moves) {
            GameTree nextState = state.applyMove(move);
            value = std::max(value, alphaBetaMin(nextState, alpha, beta, depth + 1));
            
            if (value >= beta) {
                return value; 
            }
            
            alpha = std::max(alpha, value);
        }
        
        return value;
    }
    
    // Alpha-beta pruning 
    int alphaBetaMin(const GameTree& state, int alpha, int beta, int depth) {
     
        if (state.isGameOver() || depth >= maxDepth) {
            return state.evaluatePosition();
        }
        
        std::vector<Move> moves = state.getValidMoves();
        if (moves.empty()) {
            return state.evaluatePosition();
        }
        
        int value = 100000;
        
        for (const Move& move : moves) {
            GameTree nextState = state.applyMove(move);
            value = std::min(value, alphaBetaMax(nextState, alpha, beta, depth + 1));
            
            if (value <= alpha) {
                return value; 
            }
            
            beta = std::min(beta, value);
        }
        
        return value;
    }
    
  
    int getDepth() const {
        return maxDepth;
    }
    
    
    GameTree applyMove(const Move& move) const {
 
        std::vector<std::vector<char>> newBoard = board;
        int newPlayerATokensExited = playerATokensExited;
        int newPlayerBTokensExited = playerBTokensExited;
        
       
        newBoard[move.fromRow][move.fromCol] = EMPTY;
        
        // Handle jump
        if (move.isJump) {
            int jumpedRow = (move.fromRow + move.toRow) / 2;
            int jumpedCol = (move.fromCol + move.toCol) / 2;
            newBoard[jumpedRow][jumpedCol] = EMPTY;
        }
        
        
        bool tokenExited = false;
        if (move.player == PLAYER_A) {
            
            if (move.toCol == BOARD_SIZE - 1) {
                tokenExited = true;
                newPlayerATokensExited++;
            }
        } else if (move.player == PLAYER_B) {
          
            if (move.toRow == BOARD_SIZE - 1) {
                tokenExited = true;
                newPlayerBTokensExited++;
            }
        }
        
        
        if (!tokenExited) {
            newBoard[move.toRow][move.toCol] = move.player;
        }
        
        
        char nextPlayer = (move.player == PLAYER_A) ? PLAYER_B : PLAYER_A;
        
        
        return GameTree(newBoard, nextPlayer, newPlayerATokensExited, newPlayerBTokensExited, walls, maxDepth - 1);
    }
    
    
    bool isGameOver() const {
        
        int tokenLimit = 3; 
        
        
        int playerAOnBoard = 0;
        int playerBOnBoard = 0;
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) playerAOnBoard++;
                else if (board[row][col] == PLAYER_B) playerBOnBoard++;
            }
        }
        
        
        int playerATotal = playerAOnBoard + playerATokensExited;
        int playerBTotal = playerBOnBoard + playerBTokensExited;
        
        
        return (playerATokensExited == tokenLimit || 
                playerBTokensExited == tokenLimit ||
                playerATotal == 0 ||
                playerBTotal == 0);
    }
    
    
    char getWinner() const {
        if (!isGameOver()) return ' ';
        
        int tokenLimit = 3; 
        
        
        int playerAOnBoard = 0;
        int playerBOnBoard = 0;
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) playerAOnBoard++;
                else if (board[row][col] == PLAYER_B) playerBOnBoard++;
            }
        }
        
      
        int playerATotal = playerAOnBoard + playerATokensExited;
        int playerBTotal = playerBOnBoard + playerBTokensExited;
        
      
        if (playerATokensExited == tokenLimit) return PLAYER_A;
        if (playerBTokensExited == tokenLimit) return PLAYER_B;
        
 
        if (playerBTotal == 0) return PLAYER_A;
        if (playerATotal == 0) return PLAYER_B;
        
        return ' ';
    }
    
   
    std::vector<Move> getValidMoves() const {
        std::vector<Move> validMoves;
        
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == currentPlayer) {
                   
                    if (currentPlayer == PLAYER_A) {
                       
                        if (col + 1 < BOARD_SIZE && board[row][col + 1] == EMPTY && !isWall(row, col + 1)) {
                            validMoves.push_back(Move(row, col, row, col + 1, false, PLAYER_A));
                        }
                        
                        
                        if (col + 2 < BOARD_SIZE && board[row][col + 1] == PLAYER_B && 
                            board[row][col + 2] == EMPTY && !isWall(row, col + 2)) {
                            validMoves.push_back(Move(row, col, row, col + 2, true, PLAYER_A));
                        }
                    }
                    
                    else {
                       
                        if (row + 1 < BOARD_SIZE && board[row + 1][col] == EMPTY && !isWall(row + 1, col)) {
                            validMoves.push_back(Move(row, col, row + 1, col, false, PLAYER_B));
                        }
                        
                        
                        if (row + 2 < BOARD_SIZE && board[row + 1][col] == PLAYER_A && 
                            board[row + 2][col] == EMPTY && !isWall(row + 2, col)) {
                            validMoves.push_back(Move(row, col, row + 2, col, true, PLAYER_B));
                        }
                    }
                }
            }
        }
        
        return validMoves;
    }
    
    // Evaluate the current position heuristically 
    int evaluatePosition() const {
        int score = 0;
        
        
        int tokenLimit = 3; 
        int playerARemaining = tokenLimit - playerATokensExited;
        int playerBRemaining = tokenLimit - playerBTokensExited;
        
        
        int playerAOnBoard = 0;
        int playerBOnBoard = 0;
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) playerAOnBoard++;
                else if (board[row][col] == PLAYER_B) playerBOnBoard++;
            }
        }
        
        
        int playerATotal = playerAOnBoard + playerATokensExited;
        int playerBTotal = playerBOnBoard + playerBTokensExited;
        
        
        if (playerATotal == 0) {
   
            return (currentPlayer == PLAYER_B) ? 10000 : -10000;
        }
        if (playerBTotal == 0) {
            
            return (currentPlayer == PLAYER_A) ? 10000 : -10000;
        }
        
        
        if (currentPlayer == PLAYER_A) {
            score += (playerBRemaining - playerARemaining) * 15;
          
            score += (playerAOnBoard - playerBOnBoard) * 8;
        } else {
            score += (playerARemaining - playerBRemaining) * 15;
          
            score += (playerBOnBoard - playerAOnBoard) * 8;
        }
        
        // Position evaluation 
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (board[row][col] == PLAYER_A) {
                  
                    int positionValue = col * col * 3;
                    if (currentPlayer == PLAYER_A) {
                        score += positionValue;
                    } else {
                        score -= positionValue;
                    }
                    
                    
                    if (isWall(row-1, col) || isWall(row+1, col) || 
                        isWall(row, col-1) || isWall(row, col+1)) {
                        if (currentPlayer == PLAYER_A) {
                            score -= 5; 
                        } else {
                            score += 5;
                        }
                    }
                } else if (board[row][col] == PLAYER_B) {
                   
                    int positionValue = row * row * 3;
                    if (currentPlayer == PLAYER_B) {
                        score += positionValue;
                    } else {
                        score -= positionValue;
                    }
                    
                   
                    if (isWall(row-1, col) || isWall(row+1, col) || 
                        isWall(row, col-1) || isWall(row, col+1)) {
                        if (currentPlayer == PLAYER_B) {
                            score -= 5; 
                        } else {
                            score += 5;
                        }
                    }
                }
            }
        }
        
 
        std::vector<Move> moves = getValidMoves();
        for (const Move& move : moves) {
            if (move.isJump) {
                score += 10;  
            }
        }
        
    
       
        if (currentPlayer == PLAYER_A) {
            for (int row = 0; row < BOARD_SIZE; row++) {
                for (int col = 0; col < BOARD_SIZE; col++) {
                    if (board[row][col] == PLAYER_A) {
                       
                        bool clearPath = true;
                        for (int c = col + 1; c < BOARD_SIZE; c++) {
                            if (board[row][c] != EMPTY || isWall(row, c)) {
                                clearPath = false;
                                break;
                            }
                        }
                        if (clearPath) {
                            score += 20; 
                        }
                    }
                }
            }
        }
       
        else {
            for (int row = 0; row < BOARD_SIZE; row++) {
                for (int col = 0; col < BOARD_SIZE; col++) {
                    if (board[row][col] == PLAYER_B) {
                        
                        bool clearPath = true;
                        for (int r = row + 1; r < BOARD_SIZE; r++) {
                            if (board[r][col] != EMPTY || isWall(r, col)) {
                                clearPath = false;
                                break;
                            }
                        }
                        if (clearPath) {
                            score += 20; 
                        }
                    }
                }
            }
        }
        
        return score;
    }
    
  
    const std::vector<std::pair<int, int>>& getWalls() const {
        return walls;
    }
};

#endif 
