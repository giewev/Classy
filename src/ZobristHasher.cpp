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

ZobristHasher::ZobristHasher(const ZobristHasher& original)
{
    for (int i = 0; i < 8; i++)
    {
        this->enPassantHashCodes[i] = original.enPassantHashCodes[i];
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                this->whitePieceHashCodes[i][j][k] = original.whitePieceHashCodes[i][j][k];
                this->blackPieceHashCodes[i][j][k] = original.blackPieceHashCodes[i][j][k];
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        this->castlingHashCodes[i] = original.castlingHashCodes[i];
    }

    this->turnHashCode = original.turnHashCode;
    this->hashValue = original.hashValue;
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

void ZobristHasher::loadPieces(Board board)
{
    for (int i = 0; i < 8; i ++)
    {
        for (int j = 0; j < 8; j++)
        {
            PieceType pieceType = board.getSquareType(i, j);
            if (pieceType != PieceType::Empty)
            {
                this->togglePiece(i, j, pieceType, board.getSquareColor(i, j));
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
    if (ep.type != PieceType::Empty)
    {
        this->toggleEnPassant(ep.getY());
    }
}

void ZobristHasher::togglePiece(int x, int y, PieceType pieceType, bool color)
{
    if (x > 7 || x < 0 || y > 7 || y < 0 || pieceType > 6 || pieceType <= 0)
    {
        throw "Coordinates out of bounds";
    }

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

void ZobristHasher::update(Board prevBoard, Move nextMove)
{
    this->toggleTurn();
    this->updateEnPassant(prevBoard, nextMove);
    this->updateCastling(prevBoard, nextMove);
    this->updatePieces(prevBoard, nextMove);
}

void ZobristHasher::updateEnPassant(Board prevBoard, Move nextMove)
{
    Piece prevEP = prevBoard.getEP();
    if (prevEP.type != PieceType::Empty)
    {
        this->toggleEnPassant(prevEP.getY());
    }

    if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == PieceType::Pawn &&
        abs(nextMove.endY - nextMove.startY) == 2)
    {
        this->toggleEnPassant(nextMove.startY);
    }
}

void ZobristHasher::updateCastling(Board prevBoard, Move nextMove)
{
    if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == PieceType::King)
    {
        bool kingColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
        if (prevBoard.getCastlingRights(kingColor, true))
        {
            this->toggleCastlingRights(kingColor, true);
        }

        if (prevBoard.getCastlingRights(kingColor, false))
        {
            this->toggleCastlingRights(kingColor, false);
        }
    }
    else if (prevBoard.getSquareType(nextMove.startX, nextMove.startY) == PieceType::Rook)
    {
        bool rookColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
        if ((nextMove.startY == 0 && rookColor == true) ||
            (nextMove.startY == 7 && rookColor == false))
        {
            if (nextMove.startX == 0 && prevBoard.getCastlingRights(rookColor, false))
            {
                this->toggleCastlingRights(rookColor, false);
            }
            else if (nextMove.startX == 7 && prevBoard.getCastlingRights(rookColor, true))
            {
                this->toggleCastlingRights(rookColor, true);
            }
        }
    }
}

void ZobristHasher::updatePieces(Board prevBoard, Move nextMove)
{
    PieceType movingPieceType = prevBoard.getSquareType(nextMove.startX, nextMove.startY);
    bool movingPieceColor = prevBoard.getSquareColor(nextMove.startX, nextMove.startY);
    this->togglePiece(nextMove.startX, nextMove.startY, movingPieceType, movingPieceColor);
    this->togglePiece(nextMove.endX, nextMove.endY, movingPieceType, movingPieceColor);

    PieceType capturePieceType = prevBoard.getSquareType(nextMove.endX, nextMove.endY);
    if (capturePieceType != PieceType::Empty)
    {
        this->togglePiece(nextMove.endX, nextMove.endY, capturePieceType, !movingPieceColor);
    }

    if (movingPieceType == PieceType::Pawn)
    {
        Piece prevEP = prevBoard.getEP();
        if (prevEP.type != PieceType::Empty &&
            nextMove.startY == prevEP.yPos &&
            nextMove.endX == prevEP.xPos)
        {
            this->togglePiece(prevEP.xPos, prevEP.yPos, PieceType::Pawn, prevEP.color);
        }

        if (nextMove.endY == 0 || nextMove.endY == 7)
        {
            PieceType promotionType;
            switch (nextMove.promotion)
            {
            case(1):
                promotionType = PieceType::Queen;
                break;
            case(2):
                promotionType = PieceType::Knight;
                break;
            case(3):
                promotionType = PieceType::Bishop;
                break;
            case(4):
                promotionType = PieceType::Rook;
                break;
            default:
                throw "Unknown Promotion Value";
            }

            this->togglePiece(nextMove.endX, nextMove.endY, movingPieceType, movingPieceColor);
            this->togglePiece(nextMove.endX, nextMove.endY, promotionType, movingPieceColor);
        }
    }

    if (movingPieceType == PieceType::King && abs(nextMove.startX - nextMove.endX) == 2)
    {
        if (nextMove.endX == 6)
        {
            this->togglePiece(7, nextMove.endY, PieceType::Rook, movingPieceColor);
            this->togglePiece(5, nextMove.endY, PieceType::Rook, movingPieceColor);
        }
        else if (nextMove.endX == 2)
        {
            this->togglePiece(0, nextMove.endY, PieceType::Rook, movingPieceColor);
            this->togglePiece(3, nextMove.endY, PieceType::Rook, movingPieceColor);
        }
    }
}

long long ZobristHasher::generateHashCode()
{
    std::uniform_int_distribution<long long> distribution;
    return distribution(ZobristHasher::hashGenerator);
}
