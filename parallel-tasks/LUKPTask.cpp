#include "LUKPTask.h"

LUKPTask::LUKPTask(Chessboard &cboard, std::pair<uint8_t, uint8_t> source, uint8_t incoming_move, tbb::atomic<uint16_t>* longestPathLength, tbb::atomic<uint16_t>* longestPathCount, tbb::atomic<OutputData*>* longestPathBoard) 
  : cboard(cboard), source(source), incoming_move(incoming_move), longestPathLength(longestPathLength), longestPathCount(longestPathCount), longestPathBoard(longestPathBoard){ }

tbb::task* LUKPTask::execute(){
  tbb::task_list children;
  uint8_t child_count = 0;
  uint16_t currentLongestPathLength = 0, currentLongestPathCount = 0;
  Chessboard currentLongestPathBoard;

  for(uint8_t outgoing_move = 0b00000001; outgoing_move > 0; outgoing_move <<= 1) {
    std::pair<uint8_t, uint8_t> target = cboard.doMove(this->source, outgoing_move);

    if(cboard.isFinishable(this->source, target, outgoing_move)) {
      uint8_t moves = outgoing_move | cboard.rotate180(this->incoming_move);

      Chessboard cpy = this->cboard;
      std::pair<uint8_t, uint8_t> root = cpy.getRoot();

      cpy.setField(moves, this->source);
      cpy.updateField((cpy.at(root.first, root.second) | cboard.rotate180(outgoing_move)), root);

      if(cpy.getLength() == currentLongestPathLength){
        this->longestPathCount++;
      }
      else if(cpy.getLength() > currentLongestPathLength){
        currentLongestPathLength = cpy.getLength();
        currentLongestPathCount = 1; 
        currentLongestPathBoard = cpy;
      }
    } else if(!cboard.isFieldReserved(target) && !cboard.isConnectionCrossesEdge(this->source, outgoing_move)) {
      uint8_t moves = outgoing_move | cboard.rotate180(this->incoming_move);

      Chessboard cpy = this->cboard;
      cpy.setField(moves, this->source);

      LUKPTask* child = new (tbb::task::allocate_child()) LUKPTask(cpy, target, outgoing_move, this->longestPathCount, this->longestPathLength, this->longestPathBoard);
      children.push_back(*child);
      child_count++;
    }
    //else { Field is used or crosses path -> do nothing}
  }

  //Start child tasks and block
  if(child_count > 0){
    tbb::task::set_ref_count((int)child_count + 1);
    tbb::task::spawn_and_wait_for_all(children);
  }

  //Check if the completed path is of an important length
  if(currentLongestPathLength == *this->longestPathLength){
    *this->longestPathCount += currentLongestPathCount;
  }
  if(currentLongestPathLength > *this->longestPathLength){
    *this->longestPathLength = currentLongestPathLength;
    *this->longestPathCount = 1;
    (*this->longestPathBoard)->cboard = currentLongestPathBoard;
  }

  return nullptr;
}
