#include <inttypes.h>
#include <time.h>
#include <iostream>
class Board;
struct Move;
class HashData;
class HashTable;

#define killTableSize 3
#define bitBoard uint64_t

class Engine{
	Board* gameBoard;
	int score;
public:
	clock_t doneTime;
	Move* principal;
	int maxDepth;
	double nullBound;
	short (*killers)[killTableSize];
	int* killersIndex;

	Engine();
	Engine(Board*);
	~Engine();

	void updateBoard(Board*);
	void hashPosition(Board*, Move);
	void hashPosition(Board*, Move, int);
	Move getHash(Board*);

	Board* getBoard();
	int getScore();

	void setBoard(Board*);
	void setScore(int);

	double lazyEval(Board*);
	double evaluate(Board*);
	Move minMax(int, Board*);
	Move minMax(int);

	double extend(Board*);
	double extend(Board*, double);

	Move alphaBeta(int, Board*, double);
	Move alphaBeta(int);
	Move alphaBeta(int, double);
	Move iterDeepSearch(float);

	void sortMoveList(Move*, int, Board*);
	void addKiller(Move, int);
	bool inKillers(Move, int);

	double manhattenDistance(int, int, double, double);
	
	static void printMove(Move);
	static std::string toAlg(int);
	static int fromAlg(char);

	static void trimTop(bitBoard&, int);
	static void trimBottom(bitBoard&, int);
	static void trimRight(bitBoard&, int);
	static void trimLeft(bitBoard&, int);
};

static double diffClock(clock_t clock1,clock_t clock2)
{
    double diffticks=clock1-clock2;
    double diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
    return diffms / 1000.0;
}

static double sumClock(clock_t clock1, float clock2)
{
	clock2*=CLOCKS_PER_SEC;
    double sumTicks=clock1+clock2;
    double sumS=(sumTicks)/(CLOCKS_PER_SEC);
    return sumS;
}

