#ifndef LUKPTASK_H
#define LUKPTASK_H

#include "stdint.h"
#include <vector>
#include <valarray>

#include "tbb/task.h"
#include <tbb/tbb.h>
#include <tbb/atomic.h>

#include "LongestUncrossedKnightsPath.h"

class LUKPTask: public tbb::task {
	Chessboard cboard;
	std::pair<uint8_t, uint8_t> source;
	uint8_t incoming_move;

	//Atomic varialbes to share data storage between tasks
	tbb::atomic<uint16_t>* longestPathLength;
	tbb::atomic<uint16_t>* longestPathCount;
	tbb::atomic<OutputData*>* longestPathBoard;
public:
	
	LUKPTask(Chessboard&, std::pair<uint8_t, uint8_t>, uint8_t, tbb::atomic<uint16_t>*, tbb::atomic<uint16_t>*, tbb::atomic<OutputData*>*);
	tbb::task* execute();
};

#endif /* LUKPTASK_H */
