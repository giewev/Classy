#include "Danger.h"
#include "Engine.h"
#include "Evaluation Constants.h"
#include "Piece.h"
#include "Bitwise.h"
#include "FullEvaluator.h"
#include "Logger.h"
#include "MoveSorter.h"

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

Move Engine::minMax(int depth, const Board& searchBoard)
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

Move Engine::alphaBeta(const Board& boardState, int depth, double alpha, double beta)
{
    TranspositionCache transposition = this->getTransposition(boardState);
    if (transposition.bestDepth >= depth)
    {
        return transposition.bestMove;
    }
    else if (transposition.cutoffDepth >= depth && // Cutoff info is good enough
        causesAlphaBetaBreak(transposition.cutoffMove.score, alpha, beta, boardState.turn))
    {
        return transposition.cutoffMove;
    }

    int moveCount = 0;
    Move moveList[220];
    boardState.generateMoveArray(moveList, moveCount);
    sortMoveList(moveList, moveCount, boardState, transposition);

    Board newBoard;
    unsigned int bestIndex = 0;
    Move returnedMove;

    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
        returnedMove.setGameOverDepth(0);

        returnedMove.setScore(evaluator.evaluate(boardState));
        if (returnedMove.score == 1000)
        {
            returnedMove.setScore(0);
        }

        this->updateTranspositionBestIfDeeper(boardState, depth, returnedMove);
        return returnedMove;
    }

    for(int i=0; i<moveCount; i++)
    {
        newBoard = boardState.newCopy();
        newBoard.makeMove(moveList[i]);

        if(depth == 1)
        {
            if (moveList[i].isCapture(boardState) && fabs(alpha) != 999 && fabs(beta) != 999)
            {
                moveList[i].score = quiesce(newBoard, alpha, beta);
            }
            else
            {
                evaluateMove(newBoard, moveList, i);
            }
        }
        else
        {
            returnedMove = alphaBeta(newBoard, depth - 1, alpha, beta);

            moveList[i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

        bestIndex = bestMove(moveList, bestIndex, i, boardState.turn);
        updateAlphaBeta(moveList[bestIndex].score, boardState.turn, alpha, beta);

        if (causesAlphaBetaBreak(moveList[i].score, alpha, beta, boardState.turn))
        {
            this->updateTranspositionCutoffIfDeeper(boardState, depth, moveList[i]);
            return(moveList[i]);
        }
    }

    this->updateTranspositionBestIfDeeper(boardState, depth, moveList[bestIndex]);
    return moveList[bestIndex];
}

Move Engine::alphaBeta(int depth)
{
    time_t timer = time(NULL);
    Move bestMove = alphaBeta(gameBoard, depth, -1000, 1000);
    Logger::mainLog()->info("Alpha-beta search on position [{0}] to a depth of {1} chose: {2} with score: {3} after {4} seconds",
                            gameBoard.outputFEN(), depth, bestMove.basicAlg(), bestMove.score, difftime(time(NULL), timer));
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
        Logger::mainLog()->flush();
    }

    Logger::mainLog()->info("Iterative search on position [{0}] to a depth of {1} chose: {2} with score: {3} after {4} seconds",
                            gameBoard.outputFEN(), depth - 1, bestMove.basicAlg(), bestMove.score, difftime(time(NULL), timer));

    return bestMove;
}

int Engine::chooseBetweenEqualMoves(Move* moveList, int bestIndex, int newIndex, bool turn)
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

void Engine::evaluateMove(const Board& evaluationBoard, Move* moveList, int index)
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

double Engine::evaluatePosition(const Board& evaluationBoard)
{
    double rawScore = evaluator.evaluate(evaluationBoard);
    if(rawScore == 1000)
    {
        rawScore = 0;
    }

    return rawScore;
}

int Engine::bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn)
{
    if (bestIndex == -1)
    {
        return currentIndex;
    }

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

bool Engine::causesAlphaBetaBreak(double score, double alpha, double beta, bool turn)
{
    return (turn && score >= beta) ||
            (!turn && score <= alpha);
}

void Engine::updateAlphaBeta(double score, bool turn, double& alpha, double& beta)
{
    if (turn)
    {
        if (score > alpha)
        {
            alpha = score;
        }
    }
    else
    {
        if (score < beta)
        {
            beta = score;
        }
    }
}

double Engine::quiesce(const Board& boardState, double alpha, double beta)
{
    double staticScore = evaluator.lazyEvaluate(boardState);
    if (causesAlphaBetaBreak(staticScore, alpha, beta, boardState.turn))
    {
        return staticScore;
    }

    updateAlphaBeta(staticScore, boardState.turn, alpha, beta);

    int moveCount = 0;
    Move moveList[220];
    boardState.generateCaptureMoves(moveList, moveCount);
    sortMoveList(moveList, moveCount, boardState, TranspositionCache());
    int bestIndex = -1;
    for (int i = 0; i < moveCount; i++)
    {
        Board moveBoard = boardState.newCopy();
        moveBoard.makeMove(moveList[i]);
        double score = quiesce(moveBoard, alpha, beta);
        if (causesAlphaBetaBreak(score, alpha, beta, boardState.turn))
        {
            return score;
        }

        updateAlphaBeta(score, boardState.turn, alpha, beta);
        bestIndex = bestMove(moveList, bestIndex, i, boardState.turn);
    }

    if (bestIndex == -1)
    {
        return staticScore;
    }
    else if ((boardState.turn && staticScore > moveList[bestIndex].score) ||
        (!boardState.turn && staticScore < moveList[bestIndex].score))
    {
        return staticScore;
    }
    else
    {
        return moveList[bestIndex].score;
    }
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

void Engine::sortMoveList(Move* rawList, int moveCount, const Board& sortBoard, const TranspositionCache& transposition)
{
    MoveSorter sorter = MoveSorter(rawList, moveCount, sortBoard, transposition);
    sorter.sortMoves();
}

void Engine::updateTranspositionBestIfDeeper(const Board& newBoard, int depth, Move newMove)
{
    this->clearTranspositionIfFull();
    if (this->transpositionTable[newBoard].bestDepth < depth)
    {
        this->transpositionTable[newBoard].bestDepth = depth;
        this->transpositionTable[newBoard].bestMove = newMove;
    }
}

void Engine::updateTranspositionCutoffIfDeeper(const Board& newBoard, int depth, Move newMove)
{
    this->clearTranspositionIfFull();
    if (this->transpositionTable[newBoard].cutoffDepth < depth)
    {
        this->transpositionTable[newBoard].cutoffDepth = depth;
        this->transpositionTable[newBoard].cutoffMove = newMove;
    }
}

void Engine::clearTranspositionIfFull()
{
    int eraseDepth = 1;
    while (this->transpositionTable.size() > this->transTableMax)
    {
        for (auto entry = this->transpositionTable.begin(); entry != this->transpositionTable.end();)
        {
            if (entry->second.bestDepth <= eraseDepth && entry->second.cutoffDepth <= eraseDepth)
            {
                entry = this->transpositionTable.erase(entry);
            }
            else
            {
                entry++;
            }
        }

        eraseDepth++;
    }
}

TranspositionCache Engine::getTransposition(const Board& lookupBoard)
{
    return this->transpositionTable[lookupBoard];
}
