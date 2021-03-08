#ifndef LUKPSTASRTOPTIMIZATION_H
#define LUKPSTARTOPTIMIZATION_H

#include "stdint.h"
#include <vector>
#include <queue>

#include "Chessboard.h"
#include "LUKPData.h"

class LUKPStartOptimization {

private:
	std::vector<std::pair<uint8_t, uint8_t>> getStartingFields(Chessboard&) const;
    uint8_t getStartingMoves(Chessboard&, std::pair<uint8_t, uint8_t>) const;
	std::vector<ThreadInputData> getNextMoves(Chessboard&, std::pair<uint8_t, uint8_t>, uint8_t);

public:
    uint16_t longestPathLength = 0;
    uint16_t longestPathCount = 0;
    Chessboard longestPathBoard;

    std::queue<ThreadInputData> getStartingCombinations(Chessboard&, uint16_t, uint32_t, uint32_t);
};

#endif /* LUKPSTARTOPTIMIZATION_H */
