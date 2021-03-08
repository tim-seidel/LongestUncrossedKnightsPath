#ifndef LUKPSTASRTOPTIMIZATION_H
#define LUKPSTARTOPTIMIZATION_H

#include "stdint.h"
#include <vector>

#include "Chessboard.h"

class LUKPStartOptimization {
public:
	uint8_t getStartingMoves(Chessboard&, std::pair<uint8_t, uint8_t>) const;
	std::vector<std::pair<uint8_t, uint8_t>> getStartingFields(Chessboard&) const;
};

#endif /* LUKPSTARTOPTIMIZATION_H */
