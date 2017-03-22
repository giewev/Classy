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
public:
    Engine();
    Engine(Board);
    ~Engine();

    Board getBoard();
    void setBoard(Board);

    Move alphaBeta(const Board& boardState, int depth, double alpha, double beta);
    Move alphaBeta(int);
    Move iterativeSearch(int milliseconds);

    static std::string toAlg(int);
private:
    Board gameBoard;
    FullEvaluator evaluator;
    std::unordered_map<Board, TranspositionCache> transpositionTable;
    unsigned long transTableMax = 1024 * 1024 * 64;

    void updateTranspositionBestIfDeeper(const Board& searchBoard, int depth, Move newMove);
    void updateTranspositionCutoffIfDeeper(const Board& searchBoard, int depth, Move newMove);
    void clearTranspositionIfFull();
    TranspositionCache getTransposition(const Board& lookupBoard);

    static int chooseBetweenEqualMoves(Move* moveList, int currentIndex, int newIndex);
    void evaluateMove(const Board& evaluationBoard, Move* moveList, int index);
    double evaluatePosition(const Board& evaluationBoard);
    static int bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn);
    static bool causesAlphaBetaBreak(double score, double alpha, double beta, bool turn);
    static void sortMoveList(Move*, int, const Board&, const TranspositionCache&);
    static void updateAlphaBeta(double newScore, bool turn, double& alpha, double& beta);

    double quiesce(const Board& boardState, double alpha, double beta);
};
