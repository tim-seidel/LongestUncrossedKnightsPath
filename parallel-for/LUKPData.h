#ifndef DATA_H
#define DATA_H

#include "stdint.h"
#include <utility>
#include <vector>

#include "Chessboard.h"

/**
 * Data class that stores the information for a parallel instance to start with the backtracking of a subtree.
 */
class ThreadInputData {
public:
	Chessboard cboard;
	std::pair<uint8_t, uint8_t> source;
	uint8_t incomming_move = 0;
};

/**
 * Data class that store the calcuation results of a parallel instance that calculates a subtree.
 */
class OutputData {
public:
	uint16_t longestPathLength = 0;
	uint16_t longestPathCount = 0;
	Chessboard longestPathBoard;
};

#endif /* DATA_H */
