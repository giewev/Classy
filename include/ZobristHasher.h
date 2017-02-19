#ifndef ZOBRISTHASHER_H
#define ZOBRISTHASHER_H
#include <random>
class Board;
class Move;

class ZobristHasher
{
public:
    long long hashValue;

    ZobristHasher();
    ZobristHasher(Board board);
    void load(Board);
    void update(Move);

protected:
private:
    static std::mt19937_64 hashGenerator;
    static long long generateHashCode();

    long long whitePieceHashCodes[8][8][6];
    long long blackPieceHashCodes[8][8][6];
    long long castlingHashCodes[4];
    long long enPassantHashCodes[8];
    long long turnHashCode;

    void loadPieces(Board board);
    void loadEnPassant(Board board);
    void loadCastlingRights(Board board);

    void togglePiece(int x, int y, int pieceType, bool color);
    void toggleTurn();
    void toggleEnPassant(int file);
    void toggleCastlingRights(bool color, bool side);
};

#endif // ZOBRISTHASHER_H
