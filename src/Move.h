#include <string>
class Danger;
class Board;

struct Move{
	int startX, startY;
	int endX, endY;
	int promotion;
	double score;
	int gameOverDepth;
	int scoreDepth;

public:

	Move();
	Move(int, int, int, int);
	Move(int, int, int, int, int);
	
	//void setEnPassent(bool);
	void setScore(double);
	void setGameOverDepth(int);

	double getScore();
	int getGameOverDepth();

	bool operator==(Move);
	bool operator!=(Move);

	bool operator<(Move);
	bool operator>(Move);
	static bool bigger(Move, Move);

	bool isSafe(Danger);
	std::string basicAlg();
	bool isCapture(Board*);
};

