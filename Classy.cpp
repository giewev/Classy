#include <iostream>
#include <string>
#include <iomanip>
#include "Board.h"
#include "Engine.h"
#include "Danger.h"
#include "Move.h"

int main(int argc, char *argv[]){
	//std::string loadedFEN = std::string(argv[1]) + " " + std::string(argv[2]);
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
	//std::string errorCheck = "r1b1kb1r/pppp2pp/8/4pp2/8/8/PPP1K1qP/RNBQ1BNR w";

	if(std::string(argv[7]) == "d"){
		std::cout << "debugging" << std::endl;
		Board* testBoard = new Board();
		testBoard->loadFEN(startingFEN);
		Engine player = Engine(testBoard);

		std::cout << std::fixed << std::setprecision(0);

		//std::cout << player.alphaBeta(5).basicAlg() << std::endl;

		double expectedPerfts[10] = { 1, 20, 400, 8902, 197281, 4865609, 119060324};

		for(int i=0; i<7; i++){
			std::cout << (testBoard->perft(i) == expectedPerfts[i]) << std::endl;
		}
		return 0;
	}

	//std::cout << std::string(argv[7]) << std::endl;

	//std::cout << loadedFEN << std::endl;
	//std::cout << std::string(argv[2]).length() << std::endl;

	//Board* testBoard = new Board();
	//testBoard->loadFEN(loadedFEN);
	//Engine player = Engine(testBoard);


	//Danger data = Danger(testBoard);
	//std::cout << data.getCheck() << std::endl;

	//std::cout << testBoard->perft(1) << std::endl;
	//return 0;


	//std::cout << player.alphaBeta(5).basicAlg() << std::endl;
	return 0;
}

/*
	string perftTest = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w";
	string promotionPerft = "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b";
	string endGameTestSimple = "1k6/1P6/1K6/8/8/8/8/8 w";
	string endGameTest2Rooks = "8/2R5/8/1K2k3/8/8/2R5/8 w";
	string endGameTestKBNK = "1k6/8/2BK4/2N5/8/8/8/8 w";
	string evalTest = "r1bqkbnr/ppp1pppp/2n5/3p4/3P4/2N5/PPP1PPPP/R1BQKBNR w";
	string crashTest = "rn2kb1r/pQp1nqpp/5p2/8/3P4/2N5/PPPB1PPP/1R4K1 b";
	string crashTest2 = "4r1k1/2p4p/p2b1p2/6p1/6b1/PPNq2P1/1B3P1N/2K5 b - - 0 34";
	string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	string tomGame = "3r2k1/1p4p1/p1b5/2B1pp1p/PP5P/R3K1P1/3r1P2/4R3 w - f6 1 30";
*/
