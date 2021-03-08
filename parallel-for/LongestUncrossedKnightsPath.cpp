#include "LongestUncrossedKnightsPath.h"

/**
  * @brief  Constructor of algorithm
  * @param  m	width of the chessboard
  * @param  n 	height of the chessboard
  * @retval None
  */
LongestUncrossedKnightsPath::LongestUncrossedKnightsPath(uint8_t m, uint8_t n) :chessboard(m, n) {}

/**
  * @brief  Default destructor
  */
LongestUncrossedKnightsPath::~LongestUncrossedKnightsPath() {}

void LongestUncrossedKnightsPath::setThreadLimit(uint16_t threads){
	this->n_thread_limit = threads;
}

/**
  * @brief  Starts the algorithm with a parallel_for implementation
  * @retval None
  */
void LongestUncrossedKnightsPath::run() {
	//Get the number of threads and check if it should be limited in this run for testing purposes.
	uint32_t hardware_threads = std::thread::hardware_concurrency();
	if(this->n_thread_limit > 0 && this->n_thread_limit < hardware_threads){
		hardware_threads = this->n_thread_limit;
	}

	//Based on the number of threads, calculate a number of start configurations to increase the number of tasks that can be run parallel.
	//This is used to balance the load between the threads, because the sub trees of a start field can be from different depth.
	uint32_t min_tasks = hardware_threads * chessboard.getWidth() * chessboard.getHeight() * (chessboard.getWidth() + chessboard.getHeight())/4;
	uint32_t max_tasks = (hardware_threads + 1) * chessboard.getWidth() * chessboard.getHeight() * (chessboard.getWidth() + chessboard.getHeight())/4;
	std::queue<ThreadInputData> startingCombinations = this->startOptimization.getStartingCombinations(this->chessboard, hardware_threads, min_tasks, max_tasks);

	//The start configurations get transformed into a vector and for each configuraton an output object gets created.
	std::vector<ThreadInputData> input;
	std::vector<OutputData> output;
	while(!startingCombinations.empty())
	{
		input.push_back(startingCombinations.front());
	 	OutputData out; output.push_back(out);
		startingCombinations.pop();
	}

	//Run the parallel_for loop
	if(!input.empty()){
		tbb::task_scheduler_init init(n_thread_limit > 0 ? n_thread_limit : tbb::task_scheduler_init::automatic);
		LUKPIterator iterator(input, output);
		tbb::simple_partitioner sp;
		//Run through each start configuration.
		tbb::parallel_for(tbb::blocked_range<int>(0, input.size(), 1), iterator, sp);
	}
	this->collectAndExportResults(output);
}

/**
  * @brief  Calculate the result based on each sub result for each parallel_for instance.
  * @param 	output The individual output of the parallel_for instances
  * @retval None
  */
void LongestUncrossedKnightsPath::collectAndExportResults(std::vector<OutputData>& output){
	//Take the calcuates from the start optimization into consideration
	longestPathLength = startOptimization.longestPathLength;
	longestPathCount = startOptimization.longestPathCount;
	longestPathBoard = startOptimization.longestPathBoard;
	
	//Iterate over all outputs
	for(int i=0; i<output.size(); i++){
		if(output[i].longestPathLength == longestPathLength){
			longestPathCount += output[i].longestPathCount;
		}else if(output[i].longestPathLength > longestPathLength){
			longestPathCount = output[i].longestPathCount;
			longestPathLength = output[i].longestPathLength;
			longestPathBoard = output[i].longestPathBoard;
		}
	}

	std::cout << "Longest path is " << (int) longestPathLength << " with " << (int) longestPathCount << " equal length occurrences." << std::endl;
}
