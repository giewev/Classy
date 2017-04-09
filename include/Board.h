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
    Board newCopy() const;

    void countPieces() const;
    int pieceCount() const;
    int pieceCount(bool) const;
    int pieceCount(PieceType) const;
    int pieceCount(PieceType, bool) const;

    Piece getEP() const;
    Piece getSquare(int, int) const;
    Piece findKing(bool) const;
    int getKingX(bool) const;
    int getKingY(bool) const;

    PieceType getSquareType(int, int) const;
    bool getSquareColor(int, int) const;
    bool getSquareMoved(int, int) const;

    void setSquare(Piece, int, int);
    void setSquare(Piece, int);
    void setSquare(PieceType type, bool color, int x, int y);
    void setEP(Piece);
    void setEP(int, int, bool);
    void setKingLocation(bool, int, int);

    size_t getHashCode() const;
    void loadFEN(std::string);
    std::string outputFEN() const;
    double perft(int) const;
    void generateMoveArray(Move*, int&) const;
    void generateCaptureMoves(Move* moveList, int& moveCounter) const;
    int gameOverCheck() const;
    void makeMove(Move);

    bool operator==(const Board&) const;

    bool squareIsPopulated(int, int) const;
    bool squareIsType(int, int, int) const;
    double dividePerft(int) const;
    char convertToAlg(int) const;
    bool getCastlingRights(bool, bool) const;
    char getCastlingRights() const;
    void setCastlingRights(bool, bool, bool);
    void setCastlingRights(char);
    static void throwIfOutOfBounds(int x, int y);

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(turn, allPieces, pieces, castlingRights, kingCoordinates, EPdata, moveCounter, halfMoveCounter, lastMoveCapture, hasher);
    }
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
