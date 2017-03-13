#include "Danger.h"
#include "Engine.h"
#include "Evaluation Constants.h"
#include "Piece.h"
#include "Bitwise.h"
#include "FullEvaluator.h"
#include "Logger.h"

#include <iostream>
#include <math.h>
#include <random>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <strings.h>

using namespace std;

Engine::Engine()
{
    gameBoard = Board();
    evaluator = FullEvaluator();
}

Engine::Engine(Board loadBoard)
{
    gameBoard = loadBoard;
    evaluator = FullEvaluator();
}

Engine::~Engine()
{
}

Board Engine::getBoard()
{
    return gameBoard;
}

void Engine::setBoard(Board loadBoard)
{
    gameBoard = loadBoard;
}

Move Engine::minMax(int depth, Board searchBoard)
{
    int moveCount = 0;
    Move moveList[220];
    searchBoard.generateMoveArray(moveList, moveCount);

    Board newBoard;
    double moveScore;
    double bestScore;
    unsigned int bestIndex = 0;
    bestScore = 999;
    if(searchBoard.turn)
    {
        bestScore = -999;
    }
    for(int i=0; i<moveCount; i++)
    {
        newBoard = searchBoard.newCopy();
        newBoard.makeMove(moveList[i]);
        if(depth == 1)
        {
            moveScore = evaluator.evaluate(newBoard);
        }
        else
        {
            moveScore = minMax(depth-1, newBoard).getScore();
        }
        moveList[i].setScore(moveScore);
        if(moveScore > bestScore && searchBoard.turn)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
        else if(moveScore < bestScore && !searchBoard.turn)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
        else if(moveScore == bestScore && rand() % 2)
        {
            bestScore = moveScore;
            bestIndex = i;
        }
    }
    return moveList[bestIndex];
}

Move Engine::minMax(int depth)
{
    return(minMax(depth, gameBoard));
}

Move Engine::alphaBeta(int depth, Board searchBoard, double bound)
{
    TranspositionCache transposition = this->getTransposition(searchBoard);
    if (transposition.bestDepth >= depth)
    {
        return transposition.bestMove;
    }
    else if (transposition.cutoffDepth >= depth && // Cutoff info is good enough
        ((transposition.cutoffMove.score > bound && searchBoard.turn) || // We know it's a white cutoff
        ((transposition.cutoffMove.score < bound && !searchBoard.turn)))) // We know it's a black cutoff
    {
        return transposition.cutoffMove;
    }

    int moveCount = 0;
    Move moveList[220];
    searchBoard.generateMoveArray(moveList, moveCount);
    sortMoveList(moveList, moveCount, &searchBoard, transposition);

    Board newBoard;
    double defaultBound = 999;
    if(searchBoard.turn)
    {
        defaultBound = -999;
    }

    unsigned int bestIndex = 0;
    Move returnedMove;

    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
        returnedMove.setGameOverDepth(0);

        returnedMove.setScore(evaluator.evaluate(searchBoard));
        this->updateTranspositionBestIfDeeper(searchBoard, depth, returnedMove);
        return returnedMove;
    }

    for(int i=0; i<moveCount; i++)
    {
        newBoard = searchBoard.newCopy();
        newBoard.makeMove(moveList[i]);

        if(depth == 1)
        {
            evaluateMove(newBoard, moveList, i);
        }
        else
        {
            double newBound = moveList[bestIndex].score;
            if (i == 0)
            {
                newBound = defaultBound;
            }

            returnedMove = alphaBeta(depth - 1, newBoard, newBound);

            moveList[i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

        if (causesAlphaBetaBreak(moveList[i].score, bound, searchBoard.turn))
        {
            this->updateTranspositionCutoffIfDeeper(searchBoard, depth, moveList[i]);
            return(moveList[i]);
        }

        bestIndex = bestMove(moveList, bestIndex, i, searchBoard.turn);
    }

    this->updateTranspositionBestIfDeeper(searchBoard, depth, moveList[bestIndex]);
    return moveList[bestIndex];
}

Move Engine::alphaBeta(int depth)
{
    time_t timer = time(NULL);
    double bound = -999;
    if(gameBoard.turn)
    {
        bound = 999;
    }

    Move bestMove = alphaBeta(depth, gameBoard, bound);
    Logger::mainLog()->info("Alpha-beta search depth {0} chose: {1} after {2} seconds",
                                depth, bestMove.basicAlg(), difftime(time(NULL), timer));
    return bestMove;
}

Move Engine::iterativeSearch(int milliseconds)
{
    time_t timer = time(NULL);
    Move bestMove;
    int depth = 1;
    while (difftime(time(NULL), timer) < milliseconds / 2000.0)
    {
        bestMove = alphaBeta(depth++);
    }

    Logger::mainLog()->info("Iterative search depth {0} chose: {1} with score: {2} after {3} seconds",
                            depth - 1, bestMove.basicAlg(), bestMove.score, difftime(time(NULL), timer));

    return bestMove;
}

int Engine::chooseBetweenEqualMoves(Move* moveList, const int bestIndex, const int newIndex, const bool turn)
{
    int modifier = -1;
    if(turn)
    {
        modifier = 1;
    }

    int bestDepth = moveList[bestIndex].getGameOverDepth();
    //Good result
    if(moveList[newIndex].score == 999 * modifier)
    {
        if(moveList[newIndex].getGameOverDepth() < bestDepth || bestDepth == -1)
        {
            return newIndex;
        }
        else
        {
            return bestIndex;
        }
    }
    //Bad result
    else if(moveList[newIndex].score == -999 * modifier)
    {
        if(moveList[newIndex].getGameOverDepth() > bestDepth || bestDepth == -1)
        {
            return newIndex;
        }
        else
        {
            return bestIndex;
        }
    }
    else if(rand() % 2)
    {
        return newIndex;
    }
    else
    {
        return bestIndex;
    }
}

void Engine::evaluateMove(const Board evaluationBoard, Move* moveList, const int index)
{
    double moveScore = evaluator.evaluate(evaluationBoard);
    if(moveScore != 1000)
    {
        moveList[index].setScore(moveScore);
    }
    else
    {
        moveList[index].setGameOverDepth(1);
        moveList[index].setScore(0);
    }

    if(moveScore == 999 || moveScore == -999)
    {
        moveList[index].setGameOverDepth(1);
    }
}

int Engine::bestMove(Move* moveList, const int bestIndex, const int currentIndex, const bool turn)
{
    if(turn)
    {
        if (currentIndex == 0 || moveList[currentIndex].score > moveList[bestIndex].score)
        {
            return currentIndex;
        }
    }
    else
    {
        if (currentIndex == 0 || moveList[currentIndex].score < moveList[bestIndex].score)
        {
            return currentIndex;
        }
    }

    if(moveList[currentIndex].score == moveList[bestIndex].score)
    {
        return chooseBetweenEqualMoves(moveList, bestIndex, currentIndex, turn);
    }

    return bestIndex;
}

bool Engine::causesAlphaBetaBreak(const double score, const double bound, const bool turn)
{
    return (turn && score > bound) ||
            (!turn && score < bound);
}

std::string Engine::toAlg(int val)
{
    if(val < 0 || val > 7)
    {
        return "z";
    }

    std::string alpha[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    return(alpha[val]);
}

void Engine::sortMoveList(Move* rawList, int moveCount, Board* sortBoard, TranspositionCache transposition)
{
    const int maxPriority = 3;
    char movePriorities[230];

    for(int i = 0; i < moveCount; i++)
    {
        if (rawList[i] == transposition.bestMove)
        {
            movePriorities[i] = 0;
        }
        if (rawList[i] == transposition.cutoffMove)
        {
            movePriorities[i] = 1;
        }
        else if (rawList[i].isCapture(sortBoard))
        {
            movePriorities[i] = 2;
        }
        else
        {
            movePriorities[i] = 3;
        }
    }

    int sortedCount = 0;
    for (int priority = 0; priority < maxPriority; priority++)
    {
        for (int i = sortedCount; i < moveCount; i++)
        {
            if (movePriorities[i] == priority)
            {
                Move tempMove = rawList[sortedCount];
                int tempPriority = movePriorities[sortedCount];

                rawList[sortedCount] = rawList[i];
                movePriorities[sortedCount] = movePriorities[i];

                rawList[i] = tempMove;
                movePriorities[i] = tempPriority;

                sortedCount++;
            }
        }
    }
}

void Engine::updateTranspositionBestIfDeeper(Board newBoard, int depth, Move newMove)
{
    if (this->transpositionTable[newBoard].bestDepth < depth)
    {
        this->transpositionTable[newBoard].bestDepth = depth;
        this->transpositionTable[newBoard].bestMove = newMove;
    }
}

void Engine::updateTranspositionCutoffIfDeeper(Board newBoard, int depth, Move newMove)
{
    if (this->transpositionTable[newBoard].cutoffDepth < depth)
    {
        this->transpositionTable[newBoard].cutoffDepth = depth;
        this->transpositionTable[newBoard].cutoffMove = newMove;
    }
}

TranspositionCache Engine::getTransposition(Board lookupBoard)
{
    return this->transpositionTable[lookupBoard];
}
