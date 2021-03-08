#ifndef LONGESTUNCROSSEDKNIGHTSPATH_H
#define LONGESTUNCROSSEDKNIGHTSPATH_H

#include <iostream>
#include <stdint.h>
#include <queue>
#include <thread>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/partitioner.h"
#include "tbb/task_scheduler_init.h"

#include "Chessboard.h"
#include "LUKPData.h"
#include "LUKPStartOptimization.h"
#include "LUKPIterator.h"

class LongestUncrossedKnightsPath {
 
private:
	uint16_t n_thread_limit = 0;

	Chessboard chessboard;
	LUKPStartOptimization startOptimization;

	void collectAndExportResults(std::vector<OutputData>&);

public:
	uint16_t longestPathLength = 0;
    uint16_t longestPathCount = 0;
   	Chessboard longestPathBoard;
	
	LongestUncrossedKnightsPath(uint8_t, uint8_t);
	virtual ~LongestUncrossedKnightsPath();

	void setThreadLimit(uint16_t);
	void run();
};

#endif /* LONGESTUNCROSSEDKNIGHTSPATH_H */
