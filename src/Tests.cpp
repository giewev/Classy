#include <string>
#include <assert.h>
#include "Board.h"
#include "Piece.h"
#include "Engine.h"
#include "Move.h"
#include "Bitwise.h"
#include "Danger.h"
#include "ZobristHasher.h"

void startingPerft_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);

    double expectedPerfts[10] = { 1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860 };

    for(int i = 0; i < 5; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void kiwipetePerft_test()
{
    std::string kiwipeteFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board testBoard = Board();
    testBoard.loadFEN(kiwipeteFEN);

    double expectedPerfts[10] = { 1, 48, 2039, 97862, 4085603, 193690690, 8031647685 };

    for(int i = 0; i < 5; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void endgamePerft_test()
{
    std::string endgameFEN = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    Board testBoard = Board();
    testBoard.loadFEN(endgameFEN);

    double expectedPerfts[10] = { 1, 14, 191, 2812, 43238, 674624, 11030083, 178633661};

    for(int i = 0; i < 5; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void loadStartingPosition_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);

    assert(testBoard.getSquareType(0, 0) == PieceType::Rook);
    assert(testBoard.getSquareType(1, 0) == PieceType::Knight);
    assert(testBoard.getSquareType(2, 0) == PieceType::Bishop);
    assert(testBoard.getSquareType(3, 0) == PieceType::Queen);
    assert(testBoard.getSquareType(4, 0) == PieceType::King);
    assert(testBoard.getSquareType(5, 0) == PieceType::Bishop);
    assert(testBoard.getSquareType(6, 0) == PieceType::Knight);
    assert(testBoard.getSquareType(7, 0) == PieceType::Rook);

    assert(testBoard.getSquareType(0, 7) == PieceType::Rook);
    assert(testBoard.getSquareType(1, 7) == PieceType::Knight);
    assert(testBoard.getSquareType(2, 7) == PieceType::Bishop);
    assert(testBoard.getSquareType(3, 7) == PieceType::Queen);
    assert(testBoard.getSquareType(4, 7) == PieceType::King);
    assert(testBoard.getSquareType(5, 7) == PieceType::Bishop);
    assert(testBoard.getSquareType(6, 7) == PieceType::Knight);
    assert(testBoard.getSquareType(7, 7) == PieceType::Rook);

    for (int i = 0; i < 8; i++)
    {
        assert(testBoard.getSquareType(i, 1) == PieceType::Pawn);
        assert(testBoard.getSquareType(i, 6) == PieceType::Pawn);

        assert(testBoard.getSquareColor(i, 0) == true);
        assert(testBoard.getSquareColor(i, 1) == true);
        assert(testBoard.getSquareColor(i, 6) == false);
        assert(testBoard.getSquareColor(i, 7) == false);
    }
}

void noMovesInMate_test()
{
    std::string mateFEN = "4k3/4Q3/5K2/8/8/8/8/8 b - - 0 0";

    Board testBoard = Board();
    testBoard.loadFEN(mateFEN);

    int moveCount = 0;
    Move moveList[220];
    testBoard.generateMoveArray(moveList, moveCount);
    assert(moveCount == 0);
}

void basicMateInOnePuzzle_test_1()
{
    std::string matePuzzleFEN = "4k3/Q7/5K2/8/8/8/8/8 w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 1; depth < 5; depth++)
    {
        Move bestMove = engine.alphaBeta(depth);
        assert(bestMove.startX == 0);
        assert(bestMove.startY == 6);
        assert(bestMove.endX == 4);
        assert(bestMove.endY == 6);
    }
}

void basicMateInOnePuzzle_test_2()
{
    std::string matePuzzleFEN = "4k3/R7/5K2/8/8/8/8/7R w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 1; depth < 5; depth++)
    {
        Move bestMove = engine.alphaBeta(depth);
        assert(bestMove.startX == 7);
        assert(bestMove.startY == 0);
        assert(bestMove.endX == 7);
        assert(bestMove.endY == 7);
    }
}

void promotionMateInOnePuzzle_test_1()
{
    std::string promotionMatePuzzleFEN = "8/P7/1B6/8/8/6K1/8/7k w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(promotionMatePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 1; depth < 5; depth++)
    {
        Move bestMove = engine.alphaBeta(depth);

        assert(bestMove.startX == 0);
        assert(bestMove.startY == 6);
        assert(bestMove.endX == 0);
        assert(bestMove.endY == 7);
        assert(bestMove.promotion == 1 || bestMove.promotion == 3);
    }
}

void mateInThreePuzzle_test_1()
{
    std::string matePuzzleFEN = "1K3Q2/8/8/4k3/8/N7/N7/7B w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    // Solution requires 3 white moves (5 ply)
    Move bestMove = engine.alphaBeta(5);
    assert(bestMove.startX == 0);
    assert(bestMove.startY == 1);
    assert(bestMove.endX == 2);
    assert(bestMove.endY == 0);
}

void avoidMatePuzzle_test_1()
{
    std::string avoidMatePuzzleFEN = "8/2k5/K7/8/1r6/8/8/8 w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(avoidMatePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 2; depth < 5; depth++)
    {
        Move bestMove = engine.alphaBeta(depth);
        assert(bestMove.startX == 0);
        assert(bestMove.startY == 5);
        assert(bestMove.endX == 0);
        assert(bestMove.endY == 4);
    }
}

void zobristConsistancy_test_helper(std::string originalFEN, Move testMove)
{
    Board testBoard = Board();
    testBoard.loadFEN(originalFEN);

    ZobristHasher updateZobrist = ZobristHasher(testBoard);
    updateZobrist.update(testBoard, testMove);

    Board moveBoard = testBoard.newCopy();
    moveBoard.makeMove(testMove);

    ZobristHasher moveZobrist = ZobristHasher(updateZobrist);
    moveZobrist.load(moveBoard);
    assert(moveZobrist.hashValue == updateZobrist.hashValue);
}

void zobristConsistancy_test()
{
    std::string zobristTestFEN = "2n1k3/1P6/8/5Pp1/8/8/8/R3K2R w KQ g6 0 2";
    Move promotionMove = Move(1, 6, 2, 7, 1);
    Move enPassantCaptureMove = Move(5, 4, 6, 5);
    Move castingRuinedByKingMove = Move(4, 0, 4, 1);
    Move castingRuinedByRookMove = Move(0, 0, 1, 0);

    zobristConsistancy_test_helper(zobristTestFEN, promotionMove);
    zobristConsistancy_test_helper(zobristTestFEN, enPassantCaptureMove);
    zobristConsistancy_test_helper(zobristTestFEN, castingRuinedByKingMove);
    zobristConsistancy_test_helper(zobristTestFEN, castingRuinedByRookMove);
}

void runAllTests()
{
    loadStartingPosition_test();
    startingPerft_test();
    kiwipetePerft_test();
    endgamePerft_test();
    noMovesInMate_test();
    basicMateInOnePuzzle_test_1();
    basicMateInOnePuzzle_test_2();
    promotionMateInOnePuzzle_test_1();
    mateInThreePuzzle_test_1();
    avoidMatePuzzle_test_1();
    zobristConsistancy_test();
}
