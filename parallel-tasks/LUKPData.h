#ifndef DATA_H
#define DATA_H

#include "stdint.h"
#include <utility>
#include <vector>
#include <tbb/tbb.h>
#include <tbb/atomic.h>

#include "Chessboard.h"

class InputData {
/**
 * Data class that stores the information for a starting configuration.
 */
public:
	std::pair<uint8_t, uint8_t> field;
	uint8_t move;
};

/**
 * Data class that stores the calcuation results to share across tasks.
 */
struct OutputData {
public:
	Chessboard cboard;
};

#endif /* DATA_H */
