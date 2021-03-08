#ifndef LUKPITERATOR_H
#define LUKPITERATOR_H

#include "stdint.h"
#include <vector>

#include "tbb/blocked_range.h"

#include "LongestUncrossedKnightsPath.h"

class LUKPIterator {

	std::vector<ThreadInputData>& input;
	std::vector<OutputData>& output;

public:
	LUKPIterator(std::vector<ThreadInputData>&, std::vector<OutputData>&);
	void operator()(const tbb::blocked_range<int>& r) const;

	void backtracking(Chessboard&, std::pair<uint8_t, uint8_t>, uint8_t, OutputData*) const;
};

#endif /* LUKPITERATION_H */
