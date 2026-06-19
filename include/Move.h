#ifndef MOVE_H
#define MOVE_H

class Move {
public:
    int fromRow, fromCol;
    int toRow, toCol;
    bool isJump;
    char player;

    // Construct a new Move object with all parameters
    Move(int fr, int fc, int tr, int tc, bool jump, char p) 
        : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), isJump(jump), player(p) {}

    // Construct a default Move object
    Move() : fromRow(-1), fromCol(-1), toRow(-1), toCol(-1), isJump(false), player(' ') {}
};

#endif // MOVE_H
