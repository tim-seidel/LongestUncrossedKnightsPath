#include "LongestUncrossedKnightsPath.h"

/**
  * @brief  Constructor of algorithm
  * @param  m	width of the chessboard
  * @param  n 	height of the chessboard
  * @retval None
  */
LongestUncrossedKnightsPath::LongestUncrossedKnightsPath(uint8_t m, uint8_t n) : chessboard(Chessboard(m , n)) { }

/**
  * @brief  Default destructor
  */
LongestUncrossedKnightsPath::~LongestUncrossedKnightsPath() {
}

void LongestUncrossedKnightsPath::limitThreads(uint8_t threads){
	this->n_threads = threads;
}

/**
  * @brief  Runs the algorithm for all starting fields.
  * It takes the optimization from the LUKPStartOptimization and stores them as start configuration to calculate the paths.
  * @param  None
  * @retval None
  */
void LongestUncrossedKnightsPath::run() {
  LUKPStartOptimization startOptimization;

	std::vector<InputData> input;
	std::vector<std::pair<uint8_t, uint8_t>> fields = startOptimization.getStartingFields(this->chessboard);
	for(int i=0; i<fields.size(); i++){
		uint8_t moves = startOptimization.getStartingMoves(this->chessboard, fields[i]);
		for(uint8_t move = 0b00000001; move > 0; move <<= 1) {
			InputData in;

			if((moves & move) == 0) continue;

			in.field = fields[i];
			in.move = move;

			input.push_back(in);
		}
	}

	this->runInternal(input);
}


/**
  * @brief  Starts the internal backtarcking for all start configurations
  * @param  input	The start configurations
  * @retval None
  */
void LongestUncrossedKnightsPath::runInternal(std::vector<InputData>& input){
	std::cout << "Running with " << input.size() << " starting field+move options." << std::endl;
	if(n_threads > 0){
		std::cout << "Limiting number of threads to " << (int) n_threads << "." << std::endl;
	}
	tbb::task_scheduler_init init(n_threads > 0 ? n_threads : tbb::task_scheduler_init::automatic);

	//The atomic variables that contain the longest path stats
	tbb::atomic<uint16_t> longestPathLength = 0;   
  	tbb::atomic<uint16_t> longestPathCount = 0;
	tbb::atomic<OutputData*> longestPathBoard = new OutputData();
	
	//Iterate over all start configurations
	for(int i=0; i<input.size(); i++){
		std::pair<uint8_t, uint8_t> field = input[i].field;
		uint8_t move = input[i].move;

		//Create the setup
		Chessboard cboard = chessboard;
		cboard.setRoot(move, field);
		std::pair<uint8_t, uint8_t> target = cboard.doMove(field, move);
		
		//Start the root task for the start optimization
		LUKPTask* root = new (tbb::task::allocate_root()) LUKPTask(cboard, target, move, &longestPathLength, &longestPathCount, &longestPathBoard);
		tbb::task::spawn_root_and_wait(*root);
	}

	FileExporter fileExporter;
	fileExporter.exportPath(longestPathBoard->cboard, 1);

  	std::cout << "Longest path is " << (int) longestPathLength << " with " << (int) longestPathCount << " equal length occurrences." << std::endl;
}
