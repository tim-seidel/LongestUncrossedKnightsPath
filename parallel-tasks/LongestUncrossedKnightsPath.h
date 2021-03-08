#ifndef LONGESTUNCROSSEDKNIGHTSPATH_H
#define LONGESTUNCROSSEDKNIGHTSPATH_H

#include <iostream>
#include <stdint.h>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/partitioner.h"
#include "tbb/task_scheduler_init.h"

#include "Chessboard.h"
#include "LUKPData.h"
#include "LUKPStartOptimization.h"
#include "LUKPTask.h"
#include "FileExporter.h"

class LongestUncrossedKnightsPath {
 
private:
	uint8_t n_threads = 0;

	Chessboard chessboard;

	void runInternal(std::vector<InputData>&);

public:
	LongestUncrossedKnightsPath(uint8_t, uint8_t);
	virtual ~LongestUncrossedKnightsPath();

	void limitThreads(uint8_t);
	void run();
};

#endif /* LONGESTUNCROSSEDKNIGHTSPATH_H */
