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
    int score;
    FullEvaluator evaluator;
    std::unordered_map<Board, TranspositionCache> transpositionTable;
public:
    clock_t doneTime;
    int maxDepth;
    double nullBound;

    Engine();
    Engine(Board);
    ~Engine();

    void updateBoard(Board);
    void hashPosition(Board*, Move);
    void hashPosition(Board*, Move, int);
    Move getHash(Board*);

    Board getBoard();
    int getScore();

    void setBoard(Board);
    void setScore(int);

    Move minMax(int, Board);
    Move minMax(int);

    Move alphaBeta(int, Board, double);
    Move alphaBeta(int);
    Move alphaBeta(int, double);
    Move iterativeSearch(int milliseconds);

    void sortMoveList(Move*, int, Board*);

    static double manhattenDistance(int, int, double, double);

    static void printMove(Move);
    static std::string toAlg(int);
    static int fromAlg(char);

    static void trimTop(bitBoard&, int);
    static void trimBottom(bitBoard&, int);
    static void trimRight(bitBoard&, int);
    static void trimLeft(bitBoard&, int);
private:
    void updateTranspositionBestIfDeeper(Board searchBoard, int depth, Move newMove);
    void updateTranspositionCutoffIfDeeper(Board searchBoard, int depth, Move newMove);
    TranspositionCache getTransposition(Board lookupBoard);
};
