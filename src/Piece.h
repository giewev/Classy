
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

	bool isSafe(Board*);
	bool operator!=(Piece);

	virtual bool hasMoved();
	virtual void setMoved(bool);
	virtual const bool isNull();
	void generateMoves(std::vector<Move>&, Board&);
	void kingMoves(std::vector<Move>&, Board&);
	void queenMoves(std::vector<Move>&, Board&);
	void pawnMoves(std::vector<Move>&, Board&);
	void bishopMoves(std::vector<Move>&, Board&);
	void knightMoves(std::vector<Move>&, Board&);
	void rookMoves(std::vector<Move>&, Board&);

	void appendMoveArray(Move*, int&, Board&);
	void kingMoveArray(Move*, int&, Board&);
	void queenMoveArray(Move*, int&, Board&);
	void pawnMoveArray(Move*, int&, Board&);
	void bishopMoveArray(Move*, int&, Board&);
	void knightMoveArray(Move*, int&, Board&);
	void rookMoveArray(Move*, int&, Board&);

};

