#include "LUKPIterator.h"

/**
  * @brief  The constuctor for the class that is used in the tbb parallel_for loop.
  * @param  input   The input data that the parallel instances can access to start their calculatons
  * @param  output  The output data that the parallel instances can access to store their results
  */
LUKPIterator::LUKPIterator(std::vector<ThreadInputData>& _input, std::vector<OutputData>& _output) : input(_input), output(_output){ }

/**
  * @brief  The method that is called by the parallel_for loop for a provided range.
  *         The values of this range get caculated sequential in its parallel instance.
  *         For each item in the range a backtracking is started for the given start configuration (provided by the input array).
  * @param  r The range that represents the items in the input array.
  * @retval None
  */
void LUKPIterator::operator()(const tbb::blocked_range<int>& r) const{
	for (int i = r.begin(); i != r.end(); i++){
		ThreadInputData data = input[i];
		Chessboard cpy = data.cboard;

		backtracking(cpy, data.source, data.incomming_move, &output[i]);
	}
}

/**
  * @brief  The actual bactracking. In each iteration a node is processed.
  * @param  cboard        The current copy of the chessboard
  * @param  source        The current field
  * @param  incoming_move The current move that leads to source
  * @param  out           A reference to the output object to store the results within the current instance
  * @retval None
  */
void LUKPIterator::backtracking(Chessboard &cboard, std::pair<uint8_t, uint8_t> source, uint8_t incoming_move, OutputData* out) const{
  for(uint8_t outgoing_move = 0b00000001; outgoing_move > 0; outgoing_move <<= 1) {
    std::pair<uint8_t, uint8_t> target = cboard.doMove(source, outgoing_move);

    if(!cboard.isFieldReserved(target) && !cboard.isConnectionCrossesEdge(source, outgoing_move)) {
      //The next move is valid, so it gets stored in a new board copy
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
      cpy.setField(moves, source);

      //The next backtracking iteration with the new move and target node gets started
      this->backtracking(cpy, target, outgoing_move, out);
    }else if(cboard.isFinishable(source, target, outgoing_move)) {
      //The board can be finised. The finishing move gets stored in a new board copy
      uint8_t moves = outgoing_move | cboard.rotate180(incoming_move);
      Chessboard cpy = cboard;
      std::pair<uint8_t, uint8_t> root = cpy.getRoot();
      cpy.setField(moves, source);
      cpy.updateField((cpy.at(root.first, root.second) | cboard.rotate180(outgoing_move)), root);

      //Check if the completed path is of an important length
      if(cpy.getLength() == out->longestPathLength){
        out->longestPathCount++;
      }else if(cpy.getLength() > out->longestPathLength){
        out->longestPathLength = cpy.getLength();
        out->longestPathCount = 1;
        out->longestPathBoard = cpy;
      }
    } 
    //else { Field is used or crosses path -> do nothing}
  }
}
