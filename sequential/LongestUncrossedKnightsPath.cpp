#include "LongestUncrossedKnightsPath.h"

/**
  * @brief  Constructor of algorithm
  * @param  m	width of the chessboard
  * @param  n 	height of the chessboard
  * @retval None
  */
LongestUncrossedKnightsPath::LongestUncrossedKnightsPath(uint8_t m, uint8_t n) : chessboard(m, n) { }


/**
  * @brief  Default destructor
  */
LongestUncrossedKnightsPath::~LongestUncrossedKnightsPath() {
}


/**
  * @brief  Runs the algorithm for all starting fields.
  * Start field optimization: Iterates over all fields of the chessboard and determins which fields need to be selected as starting fields for the algoritmn.
  * This takes symmetries into concideration. For details see chapter 2.1 figure 6.
  * @param  None
  * @retval None
  */
void LongestUncrossedKnightsPath::run() {
  for(uint8_t x = 0; x <= (chessboard.getWidth()  - 1)/2; ++x){
     for(uint8_t y = 0; y<= (chessboard.getHeight() - 1)/2; ++y){
      Chessboard cboard = chessboard;
      //Just take the lower left quadrant, the others can be rotated or mirrored on the horizontal and vertical axis.
      //For uneven sides the axis itself is also included.
      //Mirroring on the diagonal axis only works for boards with equal width and height
      if(y <= x || chessboard.getWidth() != chessboard.getHeight()){
        startBacktracking(cboard, std::make_pair(x + BORDER_OFFSET, y + BORDER_OFFSET));
      }
    }
  }  
}

/**
  * @brief  Starts the algorithm from a single starting field
  * @retval None
  */
void LongestUncrossedKnightsPath::runFrom(uint8_t x, uint8_t y){
  this->startBacktracking(this->chessboard, std::make_pair(x + BORDER_OFFSET, y + BORDER_OFFSET));
}

uint8_t LongestUncrossedKnightsPath::getStartingMoves(Chessboard &cboard, std::pair<uint8_t, uint8_t> root) {
  uint8_t moves = 0b00000000; uint8_t count = 0; uint8_t delete_count = 0;
  uint8_t width = cboard.getTotalWidth(); uint8_t height = cboard.getTotalHeight();

  //Search for valid moves
  for(uint8_t move = 0b00000001; move > 0; move <<= 1) {
	std::pair<uint8_t, uint8_t> target = cboard.doMove(root, move);

	if(cboard.at(target.first, target.second) != 0b11111111) { moves |= move;}
  }

  //Optimize symmetric moves (diagonal, vertical, horizontal)
  if(width == height && root.first == root.second)	{ moves &= 0b00011110; }
  if((width % 2 == 1) && (root.first == (width/2)))	{ moves &= 0b00001111; }
  if((height % 2 == 1) && (root.second == (height/2))){ moves &= 0b00111100; }

  for(uint8_t move = 0b00000001; move > 0; move <<= 1) { if((move & moves) != 0) { count++; } }
  if(count < 2) { return moves; }

  //Remove last move
  for(uint8_t move = 0b00000001; move > 0; move <<= 1){
	  if((move & moves) != 0){ delete_count++; }
	  if(count == delete_count){ moves &= (~move); break; }
  }

  return moves;
}

/**
  * @brief  Starts the internal backtarcking process.
  * @param  cboard  The current chessboard instance
  * @param  root    The root to start the backtracking from
  * @retval None
  */
void LongestUncrossedKnightsPath::startBacktracking(Chessboard &cboard, std::pair<uint8_t, uint8_t> root) {
  uint8_t moves = this->getStartingMoves(cboard, root);

  for(uint8_t move = 0b00000001; move > 0; move <<= 1) {
    if((move & moves) != 0){
      Chessboard cpy = cboard;
      cpy.setRoot(move, root);
	  
	  std::pair<uint8_t, uint8_t> target = cboard.doMove(cpy.getRoot(), move);
      backtracking(cpy, target, move);
    }
  }
}

/**
  * @brief  The actual bactracking. In each iteration a node is processed.
  * @param  cboard        The current copy of the chessboard
  * @param  source        The current field
  * @param  incoming_move The current move that leads to source
  * @retval None
  */
void LongestUncrossedKnightsPath::backtracking(Chessboard &cboard, std::pair<uint8_t, uint8_t> source, uint8_t incoming_move) {

  for(uint8_t outgoing_move = 0b00000001; outgoing_move > 0; outgoing_move <<= 1) {
    std::pair<uint8_t, uint8_t> target = cboard.doMove(source, outgoing_move);

    if(!cboard.isFieldReserved(target) && !cboard.isConnectionCrossesEdge(source, outgoing_move)) {
      //The next move is valid, so it gets stored in a new board copy
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
      cpy.setField(moves, source);

      //The next backtracking iteration with the new move and target node gets started
      this->backtracking(cpy, target, outgoing_move);
    }else if(cboard.isFinishable(source, target, outgoing_move)) {
      //The board can be finised. The finishing move gets stored in a new board copy
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
	    std::pair<uint8_t, uint8_t> root = cpy.getRoot();
      cpy.setField(moves, source);
      cpy.updateField((cpy.at(root.first, root.second) | cboard.rotate180(outgoing_move)), root);

      //Check if the completed path is of an important length
      if(cpy.getLength() == this->longestPathLength){
        this->longestPathBoards.push_back(cpy);
      }else if(cpy.getLength() > this->longestPathLength){
        this->longestPathLength = cpy.getLength();
        this->longestPathBoards.clear();
        this->longestPathBoards.push_back(cpy);
      }
    }
    //else { Field is used or crosses path -> do nothing}
  }
}


/**
  * @brief  Exports the longest path.
  * @param  None
  * @retval None
  */
void LongestUncrossedKnightsPath::exportLongestPath() const{
  std::cout << "Longest Path is " << this->longestPathLength << " with " << this->longestPathBoards.size() << " variations." << std::endl;

  FileExporter fileExporter;
  Chessboard out = longestPathBoards[0];
  fileExporter.exportPath(out, 1);
}
