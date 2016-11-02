
#include <string>
#include <vector>

struct Move;
class Board;

class Piece{

public:
	bool moved;
	int type;
	int xPos, yPos;//coordinates
	bool color;//True is White, False is Black

	Piece();//Simple constructor
	Piece(bool);//Also sets color
	Piece(int, int, bool);//sets coordinates and Color
	Piece(int);
	Piece(int, int, int, bool);

	virtual ~Piece();

	int getX(){
		return(xPos);
	}
	int getY(){
		return(yPos);
	}
	bool getColor(){
		return(color);
	}

	static void generateMoves(std::vector<Move>&, int x, int y, Board&);

	static void appendMoveArray(Move*, int&, int x, int y, Board&);
	static void kingMoveArray(Move*, int&, int x, int y, Board&);
	static void queenMoveArray(Move*, int&, int x, int y, Board&);
	static void pawnMoveArray(Move*, int&, int x, int y, Board&);
	static void bishopMoveArray(Move*, int&, int x, int y, Board&);
	static void knightMoveArray(Move*, int&, int x, int y, Board&);
	static void rookMoveArray(Move*, int&, int x, int y, Board&);

	bool isSafe(Board*);
	bool operator!=(Piece);

	virtual bool hasMoved();
	virtual void setMoved(bool);
	virtual const bool isNull();
};

