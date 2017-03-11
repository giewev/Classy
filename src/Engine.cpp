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
    score = 0;
    evaluator = FullEvaluator();
}

Engine::Engine(Board loadBoard)
{
    gameBoard = loadBoard;
    score = 0;
    evaluator = FullEvaluator();
}

Engine::~Engine()
{
}


Board Engine::getBoard()
{
    return gameBoard;
}
int Engine::getScore()
{
    return score;
}

void Engine::setBoard(Board loadBoard)
{
    gameBoard = loadBoard;
}
void Engine::setScore(int loadScore)
{
    score = loadScore;
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
    double moveScore;
    double bestScore;
    int bestDepth = -1;
    unsigned int bestIndex = 0;
    Move returnedMove;

    bestScore = 999;
    if(searchBoard.turn)
    {
        bestScore = -999;
    }
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
            moveScore = evaluator.evaluate(newBoard);
            if(moveScore != 1000)
            {
                moveList[i].setScore(moveScore);
            }
            else
            {
                moveScore = 0;
                moveList[i].setGameOverDepth(1);
                moveList[i].setScore(0);
            }
            if(moveScore == 999 || moveScore == -999)
            {
                moveList[i].setGameOverDepth(1);
            }
        }
        else
        {
            returnedMove = alphaBeta(depth - 1, newBoard, bestScore);
            moveScore = returnedMove.getScore();
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
                if (moveScore > 0)
                {
                    moveScore--;
                }
                else
                {
                    moveScore++;
                }
            }
            moveList[i].setScore(moveScore);
        }
        if(searchBoard.turn)
        {
            //Alpha Beta break
            if(moveScore > bound)
            {
                this->updateTranspositionCutoffIfDeeper(searchBoard, depth, moveList[i]);
                return(moveList[i]);
            }
            //Best move weve found
            if(moveScore > bestScore)
            {
                bestDepth = moveList[i].getGameOverDepth();
                bestScore = moveScore;
                bestIndex = i;
            }
        }
        //Its blacks turn
        else
        {
            //Alpha Beta Break
            if(moveScore < bound)
            {
                this->updateTranspositionCutoffIfDeeper(searchBoard, depth, moveList[i]);
                return(moveList[i]);
            }
            //Best move weve found
            if(moveScore < bestScore)
            {
                bestDepth = moveList[i].getGameOverDepth();
                bestScore = moveScore;
                bestIndex = i;
            }
        }
        //Exactly equal, so flip a coin
        if(moveScore == bestScore)
        {

            //GameOver score handling
            int modifier = -1;
            if(searchBoard.turn)
            {
                modifier = 1;
            }
            //Good result
            if(moveScore == 999 * modifier)
            {
                if(moveList[i].getGameOverDepth() < bestDepth || bestDepth == -1)
                {
                    bestScore = moveScore;
                    bestIndex = i;
                    bestDepth = moveList[i].getGameOverDepth();
                }
            }	//Bad result
            else if(moveScore == -999 * modifier)
            {
                if(moveList[i].getGameOverDepth() > bestDepth || bestDepth == -1)
                {
                    bestScore = moveScore;
                    bestIndex = i;
                    bestDepth = moveList[i].getGameOverDepth();
                }
            }
            else if(rand() % 2)
            {
                bestScore = moveScore;
                bestIndex = i;
                bestDepth = moveList[i].getGameOverDepth();
            }
        }
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

std::string Engine::toAlg(int val)
{
    if(val < 0 || val > 7)
    {
        return "z";
    }

    std::string alpha[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    return(alpha[val]);
}
int Engine::fromAlg(char val)
{
    if(val > 95 && val < 105)
    {
        return int(9-(105 - val));
    }
    else if(val == '1')
    {
        return 1;
    }
    else if(val == '2')
    {
        return 2;
    }
    else if(val == '3')
    {
        return 3;
    }
    else if(val == '4')
    {
        return 4;
    }
    else if(val == '5')
    {
        return 5;
    }
    else if(val == '6')
    {
        return 6;
    }
    else if(val == '7')
    {
        return 7;
    }
    else if(val == '8')
    {
        return 8;
    }
    return 0;
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

double Engine::manhattenDistance(int x1, int y1, double x2, double y2)
{
    return fabs(x1 - x2) + fabs(y1 - y2);
}

void Engine::printMove(Move toPrint)
{
    std::cout << Engine::toAlg(toPrint.startX) << toPrint.startY
              << Engine::toAlg(toPrint.endX) << toPrint.endY << std::endl;
}

void Engine::updateBoard(Board newBoard)
{
    gameBoard = newBoard;
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
