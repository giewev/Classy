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
    ZobristHasher(const ZobristHasher &original);
    void load(Board);
    void update(Board, Move);

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

    void updateEnPassant(Board prevBoard, Move nextMove);
    void updateCastling(Board prevBoard, Move nextMove);
    void updatePieces(Board prevBoard, Move nextMove);

    void togglePiece(int x, int y, int pieceType, bool color);
    void toggleTurn();
    void toggleEnPassant(int file);
    void toggleCastlingRights(bool color, bool side);
};

#endif // ZOBRISTHASHER_H
