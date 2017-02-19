
#include <iostream>
#include <vector>
#include <inttypes.h>
struct Move;
class Piece;
enum PieceType : char;

#define bitBoard uint64_t

class Board
{
    char castlingRights;
    int kingCoordinates;
public:
    int EPdata;
    bool turn;
    bool lastMoveCapture;
    int moveCounter;
    int halfMoveCounter;
    bitBoard pieces[7];
    bitBoard movedBoard;
    bitBoard allPieces;

    Board();
    Board(int);
    Board(Piece**);
    ~Board();

    Board newCopy();

    int pieceCount();
    int pieceCount(bool);
    int pieceCount(int);
    int pieceCount(int, bool);

    Piece findKing(bool);

    Piece getEP();
    Piece getSquare(int, int);
    Piece getSquare(int);
    int getKingX(bool);
    int getKingY(bool);
    PieceType getSquareType(int, int);
    bool getSquareColor(int, int);
    bool getSquareMoved(int, int);
    void setSquare(Piece, int, int);
    void setSquare(Piece, int);
    void setSquare(PieceType type, bool color, int x, int y);
    void setEP(Piece);
    void setEP(int, int, bool);
    void setKingLocation(bool, int, int);

    void loadFEN(std::string);
    std::string outputFEN();
    double perft(int);
    std::vector<Move> generateMoves();
    void generateMoveArray(Move*, int&);
    int gameOverCheck();
    void makeMove(Move);
    void countPieces();

    bool operator==(const Board);

    bool nullSquare(int, int);
    bool squareIsPopulated(int, int);
    bool squareIsType(int, int, int);
    void deleteSquare(int, int);
    double dividePerft(int);
    char convertToAlg(int);
    bool getCastlingRights(bool, bool);
    char getCastlingRights();
    void setCastlingRights(bool, bool, bool);
    void setCastlingRights(char);
};


