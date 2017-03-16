#include <inttypes.h>
#include <time.h>
#include <iostream>
#include <unordered_map>
#include "FullEvaluator.h"
#include "TranspositionCache.h"
#include "Board.h"
struct Move;
class HashData;
class HashTable;

#define killTableSize 3
#define bitBoard uint64_t

class Engine
{
    Board gameBoard;
    FullEvaluator evaluator;
    std::unordered_map<Board, TranspositionCache> transpositionTable;
public:
    Engine();
    Engine(Board);
    ~Engine();

    Board getBoard();
    void setBoard(Board);

    Move minMax(int, Board);
    Move minMax(int);

    Move alphaBeta(int, Board, double);
    Move alphaBeta(Board boardState, int depth, double alpha, double beta);
    Move alphaBeta(int);
    Move iterativeSearch(int milliseconds);

    void sortMoveList(Move*, int, Board*, TranspositionCache);

    static std::string toAlg(int);
private:
    void updateTranspositionBestIfDeeper(Board searchBoard, int depth, Move newMove);
    void updateTranspositionCutoffIfDeeper(Board searchBoard, int depth, Move newMove);
    TranspositionCache getTransposition(Board lookupBoard);

    static int chooseBetweenEqualMoves(Move* moveList, const int currentIndex, const int newIndex, const bool turn);
    void evaluateMove(const Board evaluationBoard, Move* moveList, const int index);
    static int bestMove(Move* moveList, const int bestIndex, const int currentIndex, const bool turn);
    static bool causesAlphaBetaBreak(const double score, const double alpha, const double beta, const bool turn);
};
