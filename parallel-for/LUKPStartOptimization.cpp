#include "LUKPStartOptimization.h"

/**
  * @brief  Start field optimization: Iterates over all fields of the chessboard and determins which fields need to be selected as starting fields for the algoritmn.
  * This takes symmetries into concideration. For details see chapter 2.1 figure 6.
  * @param  cboard The current copy of the chessboard
  * @retval The list of positions of the start fields.
  */
std::vector<std::pair<uint8_t, uint8_t>> LUKPStartOptimization::getStartingFields(Chessboard& cboard) const{
  std::vector<std::pair<uint8_t, uint8_t>> fields;
  for(uint8_t x = 0; x <= (cboard.getWidth()  - 1)/2; ++x){
	  for(uint8_t y = 0; y<= (cboard.getHeight() - 1)/2; ++y){
      // Just take the lower left quadrant, the others can be rotated or mirrored on the horizontal and vertical axis.
      // For uneven sides the axis itself is also included.
      // Mirroring on the diagonal axis only works for boards with equal width and height
      if(y <= x || cboard.getWidth() != cboard.getHeight()){
        fields.push_back(std::make_pair(x + BORDER_OFFSET, y + BORDER_OFFSET));
      }
	  }
  }
  return fields;
}

/**
  * @brief  Start move optimization: Determins all moves for a given start field that need to be selected as starting moves for the algorithm.
  * This takes symmetries into concideration. For details see chapter 2.1 figure 6.
  * @param  cboard  The current copy of the chessboard
  * @param  root    The current root (start field)
  * @retval The starting moves as in the bit format
  */
uint8_t LUKPStartOptimization::getStartingMoves(Chessboard& cboard, std::pair<uint8_t, uint8_t> root) const{
  uint8_t moves = 0b00000000; uint8_t count = 0; uint8_t delete_count = 0;
  uint8_t width = cboard.getTotalWidth(); uint8_t height = cboard.getTotalHeight();

  //Search for valid moves
  for(uint8_t move = 0b00000001; move > 0; move <<= 1) {
    std::pair<uint8_t, uint8_t> target = cboard.doMove(root, move);

    if(cboard.at(target.first, target.second) != 0b11111111) { moves |= move;}
  }

  //Optimize symmetric moves (diagonal, vertical, horizontal)
  if(width == height && root.first == root.second)	  { moves &= 0b00011110; }  //Diagonal axis
  if((width % 2 == 1) && (root.first == (width/2)))	  { moves &= 0b00001111; }  //Vertical axis 
  if((height % 2 == 1) && (root.second == (height/2))){ moves &= 0b00111100; }  //Horizontal axis

  for(uint8_t move = 0b00000001; move > 0; move <<= 1) { if((move & moves) != 0) { count++; } }
  if(count < 2) { return moves; }

  //If there is more than one move, the last move can always be removed,
  //Because it gets reached by paths from the other start moves as their last move.
  //(Because path direction doesn't matter.)
  for(uint8_t move = 0b00000001; move > 0; move <<= 1){
	  if((move & moves) != 0){ delete_count++; }
	  if(count == delete_count){ moves &= (~move); break; }
  }

  return moves;
}

/**
  * @brief  Calculates all valid next moves from a given state of the chessboard from any current field. It basically calculates one backtracking step.
  *         Due to the fact that this can end the path, the stats (length, count, path) of the finished board need to be stored aswell. 
  *         This method gets used to calculate enough start configurations for the available threads to use as starting nodes/tasks.
  * @param  cboard        The current copy of the chessboard
  * @param  source        The current field
  * @param  incoming_move The current move that leads to the source
  * @retval A list of the valid next move configurations (field, move, and the current chessboard state)
  */
std::vector<ThreadInputData> LUKPStartOptimization::getNextMoves(Chessboard& cboard, std::pair<uint8_t, uint8_t> source, uint8_t incoming_move){
  std::vector<ThreadInputData> next;
  for(uint8_t outgoing_move = 0b00000001; outgoing_move > 0; outgoing_move <<= 1) {
    std::pair<uint8_t, uint8_t> target = cboard.doMove(source, outgoing_move);

    if(!cboard.isFieldReserved(target) && !cboard.isConnectionCrossesEdge(source, outgoing_move)) {
      //The next move is valid, so it gets stored in a new board copy
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
      cpy.setField(moves, source);

      //Store the valid move as start configuration and add it to the queue
      ThreadInputData in;
      in.cboard = cpy;
      in.source = target;
      in.incomming_move = outgoing_move;
      next.push_back(in);
    } else if(cboard.isFinishable(source, target, outgoing_move)) {
      //The board can be finised. The finishing move gets stored in a new board copy.
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
      std::pair<uint8_t, uint8_t> root = cpy.getRoot();
      cpy.setField(moves, source);
      cpy.updateField((cpy.at(root.first, root.second) | cboard.rotate180(outgoing_move)), root);

      //Check if the completed path is of an important length
      if(cpy.getLength() == longestPathLength){
        longestPathCount++;
      }else if(cpy.getLength() > longestPathLength){
        this->longestPathLength = cpy.getLength();
        this->longestPathCount = 1;
        this->longestPathBoard = cpy;
      }
    }
    //else { Field is used or crosses path -> do nothing}
  }
  return next;
}

/**
  * @brief  This function calculates start configurations (field + move) for the algorithm. 
  * For this, the start fields with their valid start moves are calculated first. 
  * To increase the number of start configurations, so that all threads have enough tasks,
  * further successors are calculated until they are in the given and previously calculated interval [min_tasks, max_tasks].
  * Within this interval this function tries to find a number of task, which is a multiple of the number of availalbe hardware threads. 
  * If this is not possible the nearest number below max_tasks is taken. The successors are calculated with breadth-first search to balance out the subtrees.
  * After each step the number of current start configurations gets checked, whether it is in the desired interval. 
  * If it is to small, the first configuration is extracted from queue and all successors get appended. If it is to large, the process gets stopped.
  * Since with this computation finished paths can occur, the results of these are already stored temporarily, in order to be able to access them afterwards.
  * @param  cboard            The current copy of the chessboard
  * @param  hardware_threads  The number of hardware threads
  * @param  min_tasks         The minimal number of start configurations that need to be calulated to get used as parallel_for instances (subtrees)
  * @param  max_tasks         The maxium number of start configurations that can be calulated and to get used as parallel_for instances (subtrees)
  * @retval A list of all start configurations (subtrees) between min_tasks and max_tasks
  */
std::queue<ThreadInputData> LUKPStartOptimization::getStartingCombinations(Chessboard& cboard, uint16_t hardware_threads, uint32_t min_tasks, uint32_t max_tasks){
  std::queue<ThreadInputData> startingCombinations;
  
  //Get all start fields that need to be included into the algorithm.
  std::vector<std::pair<uint8_t, uint8_t>> fields = this->getStartingFields(cboard);
  //For each of them, get their valid start moves and store them as start configration.
	for(int i=0; i<fields.size(); i++){
		uint8_t moves = this->getStartingMoves(cboard, fields[i]);
		for(uint8_t move = 0b00000001; move > 0; move <<= 1) {
			if((moves & move) == 0) continue;

      ThreadInputData data;
      std::pair<uint8_t, uint8_t> root = fields[i];

      Chessboard cpy = cboard;
      //Store the start configuration
      cpy.setRoot(move, root);
      data.cboard = cpy; data.source = cpy.doMove(root, move); data.incomming_move = move;
      startingCombinations.push(data);
	  }
  }

  //For small boards it needs to be checked if there are more options.
	while(!startingCombinations.empty()){
    //Get the first start configuration for the breadth-first search.
		ThreadInputData data = startingCombinations.front();
    //Get its valid next moves as start configurations
		std::vector<ThreadInputData> next = this->getNextMoves(data.cboard, data.source, data.incomming_move);

    //Check if the new children fit in the intervall.
    //Otherwise the maximum is reaches and the loop gets exited. This is the first exit condition
    uint32_t size = startingCombinations.size() + next.size() - 1;
		if(size > max_tasks) break; //This move would exceed max tasks

    //Only if all of thex next child moves/start configrations fit in the intervall, they can replace the parent 
    //and be inserted at the end of the queue. Otherwise information would be lost.
    startingCombinations.pop();
		for(int n = 0; n< next.size(); n++){
			ThreadInputData data = next[n];
			startingCombinations.push(data);
		}

    //Check if the minimum number of start configurations is reached and if it is a multiple of the available threads.
    //Then the loop gets exited. This is the second exit condition.
		if(((size % hardware_threads) == 0) && (size >= min_tasks)) break;
	}

  return startingCombinations;
}
