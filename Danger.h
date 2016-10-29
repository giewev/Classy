
#include <vector>
class Piece;
class Board;
struct Move;

class Danger{

	int pinCoordinates[8];
	int pinDirections[8];
	int safeSquares[7];
	bool check;
	bool doubleCheck;
	int pinCount;
	int safeSquareCount;
	Board* dataBoard;

	void addAttacker(int, int);
	void addPin(int, int);
	void addPin(int);
public:
	Danger();
	Danger(Board*);
	Danger(Board*, Piece);
	~Danger();
	
	int defenderX;
	int defenderY;

	void loadData(Board*, Piece);
	void setSafeSquare(int index,int x,int y);

	int getPinCount();
	int getSafeSquareCount();
	int getPinX(int);
	int getPinY(int);
	int getSafeX(int);
	int getSafeY(int);
	bool getCheck();
	bool getDoubleCheck();
	Board* getBoard();

	bool inSafeSquares(int, int);
	bool inPinCoordinates(int, int);
	bool movePinned(Move);

	void printSafeSquares();
	
};
