#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <inttypes.h>
#include "ZobristHasher.h"

struct Move;
class Piece;
enum PieceType : char;

#define bitBoard uint64_t

class Board
{
public:
    bool turn;
    bitBoard pieces[7];
    bitBoard allPieces;

    Board();
    Board(int);
    ~Board();
    Board newCopy();

    void countPieces();
    int pieceCount();
    int pieceCount(bool);
    int pieceCount(PieceType);
    int pieceCount(PieceType, bool);

    Piece getEP();
    Piece getSquare(int, int);
    Piece findKing(bool);
    int getKingX(bool);
    int getKingY(bool);

    PieceType getSquareType(int, int) const;
    bool getSquareColor(int, int) const;
    bool getSquareMoved(int, int);

    void setSquare(Piece, int, int);
    void setSquare(Piece, int);
    void setSquare(PieceType type, bool color, int x, int y);
    void setEP(Piece);
    void setEP(int, int, bool);
    void setKingLocation(bool, int, int);

    size_t getHashCode() const;
    void loadFEN(std::string);
    std::string outputFEN();
    double perft(int);
    void generateMoveArray(Move*, int&);
    int gameOverCheck();
    void makeMove(Move);

    bool operator==(const Board&) const;

    bool squareIsPopulated(int, int) const;
    bool squareIsType(int, int, int);
    double dividePerft(int);
    char convertToAlg(int);
    bool getCastlingRights(bool, bool);
    char getCastlingRights();
    void setCastlingRights(bool, bool, bool);
    void setCastlingRights(char);
    static void throwIfOutOfBounds(int x, int y);
private:
    char castlingRights;
    int kingCoordinates;
    int EPdata;
    int moveCounter;
    int halfMoveCounter;
    bool lastMoveCapture;
    ZobristHasher hasher;
};

namespace std
{
template <> struct hash<Board>
{
    size_t operator()(const Board& board) const
    {
        return board.getHashCode();
    }
};
}

#endif
