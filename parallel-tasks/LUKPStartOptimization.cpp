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
      //Just take the lower left quadrant, the others can be rotated or mirrored on the horizontal and vertical axis.
      //For uneven sides the axis itself is also included.
      //Mirroring on the diagonal axis only works for boards with equal width and height
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
  if(width == height && root.first == root.second)	  { moves &= 0b00011110; } //Diagonal axis
  if((width % 2 == 1) && (root.first == (width/2)))	  { moves &= 0b00001111; } //Vertical axis
  if((height % 2 == 1) && (root.second == (height/2)))	  { moves &= 0b00111100; } //Horizontal axis

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
