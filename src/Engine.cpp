#include "Board.h"
#include "Danger.h"
#include "Engine.h"
#include "Evaluation Constants.h"
#include "Move.h"
#include "Piece.h"
#include "Bitwise.h"

#include <iostream>
#include <math.h>
#include <random>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <strings.h>
#include "FullEvaluator.h"

using namespace std;

Engine::Engine(){
	gameBoard = new Board();
	score = 0;
	principal = new Move[1];
	killers = new short[1][killTableSize];
	killersIndex = new int[1];
	evaluator = FullEvaluator();
}

Engine::Engine(Board* loadBoard){
	gameBoard = loadBoard;
	score = 0;
	principal = new Move[1];
	killers = new short[1][killTableSize];
	killersIndex = new int[1];
	evaluator = FullEvaluator();
}

Engine::~Engine(){

	//if (killers){
		//delete[] killers;
	//}
	//if (principal != NULL){
		//delete[] principal;
	//}
	/*
	if (moveTriangle != NULL){
		for (int i = 0; i < maxDepth; i++){
			if (moveTriangle[i] != NULL){
				delete[] moveTriangle[i];
			}
		}
		delete[] moveTriangle;
	}
	*/
}


Board* Engine::getBoard(){
	return gameBoard;
}
int Engine::getScore(){
	return score;
}

void Engine::setBoard(Board* loadBoard){
	gameBoard = loadBoard;
}
void Engine::setScore(int loadScore){
	score = loadScore;
}

Move Engine::minMax(int depth, Board* searchBoard){
	int moveCount = 0;
	Move moveList[220];
	searchBoard->generateMoveArray(moveList, moveCount);

	Board* newBoard;
	double moveScore;
	double bestScore;
	unsigned int bestIndex = 0;
	bestScore = 999;
	if(searchBoard->turn){
		bestScore = -999;
	}
	for(int i=0; i<moveCount; i++){
		newBoard = searchBoard->newCopy();
		newBoard->makeMove(moveList[i]);
		if(depth == 1){
			moveScore = evaluator.evaluate(*newBoard);
		}
		else{
			moveScore = minMax(depth-1, newBoard).getScore();
		}
		moveList[i].setScore(moveScore);
		if(moveScore > bestScore && searchBoard->turn){
			bestScore = moveScore;
			bestIndex = i;
		}
		else if(moveScore < bestScore && !searchBoard->turn){
			bestScore = moveScore;
			bestIndex = i;
		}
		else if(moveScore == bestScore && rand() % 2){
			bestScore = moveScore;
			bestIndex = i;
		}
		delete newBoard;
	}
	return moveList[bestIndex];
}
Move Engine::minMax(int depth){
	return(minMax(depth, gameBoard));
}

double Engine::extend(Board* extendBoard){
	int moveCount = 0;
	Move moveList[220];
	extendBoard->generateMoveArray(moveList, moveCount);

	double bestScore = 999;
	if (extendBoard->turn){
		bestScore = -999;
	}

	for (int i = 0; i < moveCount; i++){
		Board* newBoard = extendBoard->newCopy();
		newBoard->makeMove(moveList[i]);
		double score = evaluator.evaluate(*newBoard);
		if (extendBoard->turn){
			if (score >= bestScore){
				bestScore = score;
			}
		}
		else{
			if (score <= bestScore){
				bestScore = score;
			}
		}
		delete newBoard;
	}
	return bestScore;
}
double Engine::extend(Board* extendBoard, double bound){
	int moveCount = 0;
	Move moveList[220];
	extendBoard->generateMoveArray(moveList, moveCount);

	double bestScore = 999;
	if (extendBoard->turn){
		bestScore = -999;
	}

	for (int i = 0; i < moveCount; i++){
		Board* newBoard = extendBoard->newCopy();
		newBoard->makeMove(moveList[i]);
		double score = evaluator.evaluate(*newBoard);
		if (extendBoard->turn){
			if (score >= bestScore){
				bestScore = score;
				if (bestScore > bound){
					delete newBoard;
					return bestScore;
				}
			}
		}
		else{
			if (score <= bestScore){
				bestScore = score;
				if (bestScore < bound){
					delete newBoard;
					return bestScore;
				}
			}
		}
		delete newBoard;
	}
	return bestScore;
}

Move Engine::alphaBeta(int depth, Board* searchBoard, double bound){
	int moveCount = 0;
	Move moveList[220];
	searchBoard->generateMoveArray(moveList, moveCount);
	//sortMoveList(moveList, moveCount, searchBoard);

	Board* newBoard;
	double moveScore;
	double bestScore;
	int bestDepth = -1;
	unsigned int bestIndex = 0;
	Move returnedMove;

	bestScore = 999;
	if(searchBoard->turn){
		bestScore = -999;
	}
		//Checkmate or StaleMate
	if(moveCount == 0){
		returnedMove = Move();
		returnedMove.setGameOverDepth(0);

		returnedMove.setScore(evaluator.evaluate(*searchBoard));

		return returnedMove;
	}

	for(int i=0; i<moveCount; i++){
		newBoard = searchBoard->newCopy();
		newBoard->makeMove(moveList[i]);

		if(depth == 1){
			moveScore = evaluator.evaluate(*newBoard);

			returnedMove = Move();
			if(moveScore != 1000){
				moveList[i].setScore(moveScore);
			}
			else{
				moveScore = 0;
				moveList[i].setGameOverDepth(1);
				moveList[i].setScore(0);
			}
			if(moveScore == 999 || moveScore == -999){
				moveList[i].setGameOverDepth(1);
			}
		}
		else{
			returnedMove = alphaBeta(depth - 1, newBoard, bestScore);
			moveScore = returnedMove.getScore();
			if(returnedMove.getGameOverDepth() != -1){
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
		if(searchBoard->turn){
				//Alpha Beta break
			if(moveScore > bound){
				return(moveList[i]);
			}
				//Best move weve found
			if(moveScore > bestScore){
				bestDepth = moveList[i].getGameOverDepth();
				bestScore = moveScore;
				bestIndex = i;
			}
		}
			//Its blacks turn
		else{
				//Alpha Beta Break
			if(moveScore < bound){
				return(moveList[i]);
			}
				//Best move weve found
			if(moveScore < bestScore){
				bestDepth = moveList[i].getGameOverDepth();
				bestScore = moveScore;
				bestIndex = i;
			}
		}
			//Exactly equal, so flip a coin
		if(moveScore == bestScore){

				//GameOver score handling
			int modifier = -1;
			if(searchBoard->turn){
				modifier = 1;
			}
				//Good result
			if(moveScore == 999 * modifier){
				if(moveList[i].getGameOverDepth() < bestDepth || bestDepth == -1){
					bestScore = moveScore;
					bestIndex = i;
					bestDepth = moveList[i].getGameOverDepth();
				}
			}	//Bad result
			else if(moveScore == -999 * modifier){
				if(moveList[i].getGameOverDepth() > bestDepth || bestDepth == -1){
					bestScore = moveScore;
					bestIndex = i;
					bestDepth = moveList[i].getGameOverDepth();
				}
			}
			else if(rand() % 2){
				bestScore = moveScore;
				bestIndex = i;
				bestDepth = moveList[i].getGameOverDepth();
			}
		}
		delete newBoard;
	}
	return moveList[bestIndex];
}

Move Engine::alphaBeta(int depth){
	if(gameBoard->turn){
		return alphaBeta(depth, gameBoard, 999);
	}
	else{
		return alphaBeta(depth, gameBoard, -999);
	}
}
Move Engine::alphaBeta(int depth, double bound){
	return alphaBeta(depth, gameBoard, bound);
}

Move Engine::iterDeepSearch(float runTime){
	doneTime = (clock_t) (clock() + runTime * CLOCKS_PER_SEC);

	Move tempMove = Move();

	int searchDepth = 1;
	//delete[] principal;
	principal = new Move[2];
	while(doneTime > clock()){
		searchDepth++;
		delete[] killers;
		delete[] killersIndex;

		killers = new short[searchDepth+1][killTableSize];
		killersIndex = new int[searchDepth + 1];
		for(int i=0; i<searchDepth; i++){
			for (int j = 0; j < killTableSize; j++){
				killers[i+1][j] = -1;
			}
			killersIndex[i+1] = 0;
		}

		maxDepth = searchDepth;
		//std::cout << "info depth " << searchDepth << " score cp " << 0 << std::endl;

		tempMove = alphaBeta(searchDepth);

		if(tempMove != Move()){
		}
		else{
			searchDepth--;
		}
		if (tempMove.getGameOverDepth() != -1){
			break;
		}
	}
	/*
	std::cout << "info string searched to a depth of " << searchDepth << std::endl;
	std::cout << "info depth " << searchDepth;
	if (gameBoard->turn){
		std::cout << " score cp " << round(principal[0].getScore() * 100) << std::endl;
	}
	else{
		std::cout << " score cp " << round(principal[0].getScore() * (-100)) << std::endl;
	}
	*/
	return principal[0];
}

std::string Engine::toAlg(int val){
	if(val < 0 || val > 8){
		return "z";
	}
	std::string alpha[9] = {"z", "a", "b", "c", "d", "e", "f", "g", "h"};
	return(alpha[val]);
}
int Engine::fromAlg(char val){
	if(val > 95 && val < 105){
		return int(9-(105 - val));
	}
	else if(val == '1'){
		return 1;
	}
	else if(val == '2'){
		return 2;
	}
	else if(val == '3'){
		return 3;
	}
	else if(val == '4'){
		return 4;
	}
	else if(val == '5'){
		return 5;
	}
	else if(val == '6'){
		return 6;
	}
	else if(val == '7'){
		return 7;
	}
	else if(val == '8'){
		return 8;
	}
	return 0;
}

void Engine::sortMoveList(Move* rawList, int moveCount, Board* sortBoard){
	int captures = 0;
	int nonCaptures = 0;
	Move* captureList = new Move[230];
	Move* nonCaptureList = new Move[230];

	for(int i = 0; i < moveCount; i++){
		if(rawList[i].isCapture(sortBoard)){
			captureList[captures] = rawList[i];
			captures++;
		}
		else{
			nonCaptureList[nonCaptures] = rawList[i];
			nonCaptures++;
		}
	}

	for(int i = 0; i < captures; i++){
		rawList[i] = captureList[i];
	}

	for(int i = 0; i < nonCaptures; i++){
		rawList[i + captures] = nonCaptureList[i];
	}
}

void Engine::addKiller(Move killMove, int depth){
	killers[depth][killersIndex[depth]] =   killMove.endY-1;
	killers[depth][killersIndex[depth]] <<= 3;
	killers[depth][killersIndex[depth]] |=  killMove.endX-1;
	killers[depth][killersIndex[depth]] <<= 3;
	killers[depth][killersIndex[depth]] |= killMove.startY-1;
	killers[depth][killersIndex[depth]] <<= 3;
	killers[depth][killersIndex[depth]] |= killMove.startX-1;
	killersIndex[depth]++;
	killersIndex[depth] %= killTableSize;
}
bool Engine::inKillers(Move testMove, int depth){
	int fromX, fromY, toX, toY;
	for (int i = 0; i < killTableSize; i++){
		if (killers[depth][i] == -1){
			return false;
		}

		fromX = (killers[depth][i] & 7) + 1;
		fromY = ((killers[depth][i] >> 3) & 7) + 1;
		toX = ((killers[depth][i] >> 6) & 7) + 1;
		toY = ((killers[depth][i] >> 9) & 7) + 1;

		if (fromX != testMove.startX){
			continue;
		}
		if (fromY != testMove.startY){
			continue;
		}
		if (toX != testMove.endX){
			continue;
		}
		if (toY != testMove.endY){
			continue;
		}
		return true;
	}
	return false;
}

double Engine::manhattenDistance(int x1, int y1, double x2, double y2){
	return fabs(x1 - x2) + fabs(y1 - y2);
}

void Engine::printMove(Move toPrint){
	std::cout << Engine::toAlg(toPrint.startX) << toPrint.startY
		<< Engine::toAlg(toPrint.endX) << toPrint.endY << std::endl;
}

void Engine::updateBoard(Board* newBoard){
	gameBoard = newBoard;
}
