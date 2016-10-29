
#include <inttypes.h>
#define bitBoard uint64_t

const bitBoard firstFile = 0x0000000000000000FF;
const bitBoard centerBoard = 0x0000001818000000;
const bitBoard outsideRim = 0xFF818181818181FF;
const bitBoard firstRank = 0x0101010101010101;

const bitBoard whitePawnChains[6] = {
	0x0408000000000000,
	0x0004080000000000,
	0x0000040800000000,

	0x0000000000000804,
	0x0000000000080400,
	0x0000000008040000
};
const bitBoard blackPawnChains[6] = {
	0x2010000000000000,
	0x0020100000000000,
	0x0000201000000000,

	0x0000000000001020,
	0x0000000000102000,
	0x0000000010200000
};
const bitBoard whiteSemiCenter = 0x00000C04040C0000;
const bitBoard blackSemiCenter = 0x0000302020300000;
const bitBoard a8h1 = 0x0102040810204080;
const bitBoard a1h8 = 0x8040201008040201;


const bitBoard knightMovement = 0x0000000A1100110A;
const bitBoard pawnAttack = 0x0000000000010001;

const bitBoard BishopSquareTable[4] = {
	1800008181000018,
	2418814242811824,
	0x42A55A24245AA542,
	0x8142241818244281
};

namespace bitwise{
	void trimBottom(bitBoard& toTrim, int layers);
	void trimTop(bitBoard& toTrim, int layers);
	void trimLeft(bitBoard& toTrim, int layers);
	void trimRight(bitBoard& toTrim, int layers);

	int countBits(bitBoard data);

	bitBoard knightSquares(int x, int y);

	int firstBit(bitBoard input);
	int lastBit(bitBoard input);

	int getX(int raw);
	int getY(int raw);
}
