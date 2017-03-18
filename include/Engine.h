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

    Move minMax(int, const Board&);
    Move minMax(int);

    Move alphaBeta(int, const Board&, double);
    Move alphaBeta(const Board& boardState, int depth, double alpha, double beta);
    Move alphaBeta(int);
    Move iterativeSearch(int milliseconds);

    static std::string toAlg(int);
private:
    void updateTranspositionBestIfDeeper(const Board& searchBoard, int depth, Move newMove);
    void updateTranspositionCutoffIfDeeper(const Board& searchBoard, int depth, Move newMove);
    TranspositionCache getTransposition(const Board& lookupBoard);

    static int chooseBetweenEqualMoves(Move* moveList, int currentIndex, int newIndex, bool turn);
    void evaluateMove(const Board& evaluationBoard, Move* moveList, int index);
    double evaluatePosition(const Board& evaluationBoard);
    static int bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn);
    static bool causesAlphaBetaBreak(double score, double alpha, double beta, bool turn);
    static void sortMoveList(Move*, int, const Board&, const TranspositionCache&);
    static void updateAlphaBeta(double newScore, bool turn, double& alpha, double& beta);

    double quiesce(const Board& boardState, double alpha, double beta);
};
