#include <random>
#include "ZobristHasher.h"
#include "Board.h"
#include "Move.h"
#include "Bitwise.h"
#include "Piece.h"

std::mt19937_64 ZobristHasher::hashGenerator;

ZobristHasher::ZobristHasher()
{
    for (int i = 0; i < 8; i++)
    {
        this->enPassantHashCodes[i] = generateHashCode();
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                this->whitePieceHashCodes[i][j][k] = generateHashCode();
                this->blackPieceHashCodes[i][j][k] = generateHashCode();
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        this->castlingHashCodes[i] = generateHashCode();
    }

    this->turnHashCode = generateHashCode();
}

ZobristHasher::ZobristHasher(Board board) : ZobristHasher()
{
    this->load(board);
}

void ZobristHasher::load(Board board)
{
    this->hashValue = 0ll;
    this->loadPieces(board);
    this->loadCastlingRights(board);
    this->loadEnPassant(board);
    if (board.turn)
    {
        this->hashValue ^= this->turnHashCode;
    }
}

void ZobristHasher::update(Move moveToApply)
{
    throw "Not Implemented";
}

void ZobristHasher::loadPieces(Board board)
{
    for (int i = 1; i <= 8; i ++)
    {
        for (int j = 1; j <= 8; j++)
        {
            int pieceType = board.getSquareType(i, j);
            if (pieceType != EMPTY)
            {
                this->togglePiece(i, j, pieceType - 1, board.getSquareColor(i, j));
            }
        }
    }
}

void ZobristHasher::loadCastlingRights(Board board)
{
    bool boolValues[2] = { true, false };
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (board.getCastlingRights(boolValues[i], boolValues[j]))
            {
                this->toggleCastlingRights(boolValues[i], boolValues[j]);
            }
        }
    }
}

void ZobristHasher::loadEnPassant(Board board)
{
    Piece ep = board.getEP();
    if (ep.type != EMPTY)
    {
        this->toggleEnPassant(ep.getY() - 1);
    }
}

void ZobristHasher::togglePiece(int x, int y, int pieceType, bool color)
{
    if (color)
    {
        this->hashValue ^= this->whitePieceHashCodes[x][y][pieceType - 1];
    }
    else
    {
        this->hashValue ^= this->blackPieceHashCodes[x][y][pieceType - 1];
    }
}

void ZobristHasher::toggleTurn()
{
    this->hashValue ^= this->turnHashCode;
}

void ZobristHasher::toggleEnPassant(int file)
{
    this->hashValue ^= this->enPassantHashCodes[file];
}

void ZobristHasher::toggleCastlingRights(bool color, bool side)
{
    int castlingIndex = 0;
    if (color)
    {
        castlingIndex += 2;
    }

    if (side)
    {
    castlingIndex++;
    }

    this->hashValue ^= this->castlingHashCodes[castlingIndex];
}

long long ZobristHasher::generateHashCode()
{
    std::uniform_int_distribution<long long> distribution;
    return distribution(ZobristHasher::hashGenerator);
}
