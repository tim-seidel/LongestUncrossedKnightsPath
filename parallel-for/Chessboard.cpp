#include "Chessboard.h"

/**
  * @brief  Default constructor
  */
Chessboard::Chessboard() : Chessboard(0, 0) { }

/**
  * @brief  Constructor of the chessboard
  * @param  m width
  * @param  n height
  */
Chessboard::Chessboard(uint8_t m, uint8_t n) 
: width(m), height(n), totalWidth(m + 2 * BORDER_OFFSET), totalHeight(n + 2 * BORDER_OFFSET), matrix(totalWidth * totalHeight, 0b11111111), length(0) {
  setBorder();
}

/**
  * @brief  Default destructor
  */
Chessboard::~Chessboard() { }

/**
  * @brief  Sets all fields with their moves as empty and sets the border to 0b11111111
  * @param  None
  * @retval None
  */
void Chessboard::setBorder() {
	for(int y = 0; y < this->height; y++){
    for(int x = 0; x < this->width; x++){
      this->matrix[(y + BORDER_OFFSET) * this->totalWidth + x + BORDER_OFFSET] = 0;
    }
  }
}

/**
  * @brief  Returns value of area on chessboard
  * @param  m x-position of chessboard
  * @param  n y-position of chessboard
  * @retval Moves in given position
  */
uint8_t Chessboard::at(uint8_t x, uint8_t y) const {
  return this->matrix[y * this->totalWidth + x];
}

std::pair<uint8_t, uint8_t> Chessboard::getRoot() const{
  return this->root;
}

/**
  * @brief  Sets the moves to a field. Increases the path length of the path by one.
  * @param  moves The moves to set
  * @param  field The coordinates of the field
  * @retval None
  * @see    getLength()
  */
void Chessboard::setField(uint8_t moves, std::pair<uint8_t, uint8_t> field) {
  this->matrix[field.second * this->totalWidth + field.first] = moves;
  this->length++;
}

/**
  * @brief  A method to update a field of the chessboard. This doesn't alter the path length.
  * @param  moves The moves to set
  * @param  field The coordinates of the field
  * @retval None
  * @see    setField()
  */
void Chessboard::updateField(uint8_t moves, std::pair<uint8_t, uint8_t> field){
  this->matrix[field.second * this->totalWidth + field.first] = moves;
}

/**
  * @brief  Sets a field as root. Increases the path length of the path by one.
  * @param  moves The moves to set
  * @param  _root The coordinates of the root
  * @retval None
  * @see    setField()
  * @see    getLength()
  */
void Chessboard::setRoot(uint8_t moves, std::pair<uint8_t, uint8_t> _root){
  this->root = _root;
  this->setField(moves, _root);
}

/**
  * @brief  Returns the width of the chessboard.
  * @param  None
  * @retval width
  */
uint8_t Chessboard::getWidth() const {
  return this->width;
}

/**
  * @brief  Returns the hight of the chessboard.
  * @param  None
  * @retval height
  */
uint8_t Chessboard::getHeight() const {
  return this->height;
}

/**
  * @brief  Returns the width of the chessboard, including the borders.
  * @param  None
  * @retval total width
  */
uint16_t Chessboard::getTotalWidth() const {
  return this->totalWidth;
}

/**
  * @brief  Returns the height of the chessboard, including the borders.
  * @param  None
  * @retval total height
  */
uint16_t Chessboard::getTotalHeight() const {
  return this->totalHeight;
}

/**
  * @brief  Returns the current paths length.
  * @param  None
  * @retval The length of the current path
  */
uint16_t Chessboard::getLength() const {
  return this->length;
}

/**
  * @brief  Prints the current chessboard to the console.
  * @param  None
  * @retval None
  */
void Chessboard::debug() const {
  std::cout << "Board [" << (int) this->getWidth() << "x" << (int) this->getHeight() << " | " << this->getLength() << "]" << std::endl;
  for(uint8_t y = 0; y<this->getHeight(); y++) {
    for(uint8_t x = 0; x < this->getWidth(); x++){
      if(!this->at(x+ BORDER_OFFSET, y+BORDER_OFFSET)){
        std::cout << "0        ";
      }else{
          std::cout << std::bitset<8>(this->at(x+BORDER_OFFSET, y+ BORDER_OFFSET)) << " ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

/**
  * @brief  Returns the target of a move from the knight. Does not alter the chessboard.
  * @param  node    Node to be moved
  * @param  source  The current field of the knight
  * @param  move    The move of the knight in the bit format
  * @retval The target node position
  */
std::pair<uint8_t, uint8_t> Chessboard::doMove(std::pair<uint8_t, uint8_t> source, uint8_t move) const{
    switch(move) {
        case 0b00000001:
            return std::make_pair(source.first+1, source.second+2);
        case 0b00000010:
            return std::make_pair(source.first+2, source.second+1);
        case 0b00000100:
            return std::make_pair(source.first+2, source.second-1);
        case 0b00001000:
            return std::make_pair(source.first+1, source.second-2);
        case 0b00010000:
            return std::make_pair(source.first-1, source.second-2);
        case 0b00100000:
            return std::make_pair(source.first-2, source.second-1); 
        case 0b01000000:
            return std::make_pair(source.first-2, source.second+1); 
        case 0b10000000:
            return std::make_pair(source.first-1, source.second+2);
        default: 
            return std::make_pair(source.first, source.first);
    }
}

/**
  * @brief  Rotates the move by 180 degrees.
  * @param  move The move represented in the bit format
  * @retval The rotated move in the bit format
  */
uint8_t Chessboard::rotate180(uint8_t move) {
    return (move << 4) | (move >> 4);   
}

/**
  * @brief  Checks if the given move finishes the path. Checks that the move isn't backwards.
  * @param  source        The start position of the move
  * @param  target        The target position of the move
  * @param  outgoing_move The move represented in the bit format
  * @retval true:   Node can be processed further
  *         false:  Node cannot be processed further
  */
bool Chessboard::isFinishable(std::pair<uint8_t, uint8_t> source, std::pair<uint8_t, uint8_t> target, uint8_t outgoing_move) {
  return (target.first == root.first && target.second == root.second)
		  && !(source.first == root.first && source.second == root.second)
		  && !isConnectionCrossesEdge(source, outgoing_move);
}

/**
  * @brief  Checks whether a field is already a part of the path.
  * @param  field   The position of the field to check
  * @retval true:   Node is currently not a part of the path
  *         false:  Node is already a part of the path
  */
bool Chessboard::isFieldReserved(std::pair<uint8_t, uint8_t> field) {
  return at(field.first, field.second) != 0b00000000;
}

/**
  * @brief  Checks whether a field contains a given knight move, which means that it is connected to the target field of that move.
  * Border fields are never treated as a connection.
  * @param  x The x coordinate of the field to check
  * @param  y The y coordinate fo the filed to check
  * @retval true:   Node is connected with that move
  *         false:  Node is not connected with that move
  */
bool Chessboard::isCandidateConnected( uint8_t x, uint8_t y, uint8_t moves) {
  uint8_t candidate = at(x, y);
  if (candidate != 0b11111111) {
    return (candidate & moves) != 0b00000000;
  }
  else return false;
}

/**
  * @brief  Checks if the edge to the node crosses another edge. Iterates over all cases (see figure 5).
  * @param  source  The position of the node to check
  * @param  moves   The edges to check a possible crossing with
  * @retval true:   The move crosses an existing connection
  *         false:  The move doesn't cross an existing connection
  */
bool Chessboard::isConnectionCrossesEdge(std::pair<uint8_t, uint8_t> source, uint8_t moves) {
  switch(moves) {
      case 0b00000001:
        // blue case, (x+1,y+1), 11100000
        return isCandidateConnected(source.first+1, source.second+1, 0b11100000) ||
        // orange case, (x,y+1), 00001110
        isCandidateConnected(source.first, source.second+1, 0b00001110) ||
        // purple case, (x,y+2), 00001100
        isCandidateConnected(source.first, source.second+2, 0b00001100) ||
        // green case, (x+1,y), 01000000
        isCandidateConnected(source.first+1, source.second, 0b01000000);

      case 0b00000010:
        // orange case, (x+1,y+1), 00011100
        return isCandidateConnected(source.first+1, source.second+1, 0b00011100) ||
        // blue case, (x+1,y), 11000001
        isCandidateConnected(source.first+1, source.second, 0b11000001) ||
        // purple case, (x+2,y), 11000000
        isCandidateConnected(source.first+2, source.second, 0b11000000) ||
        // green case, (x,y+1), 00001000
        isCandidateConnected(source.first, source.second+1, 0b00001000);

      case 0b00000100:
        // blue case, (x+1,y-1), 10000011
        return isCandidateConnected(source.first+1, source.second-1, 0b10000011) ||
        // orange case, (x+1,y), 00111000
        isCandidateConnected(source.first+1, source.second, 0b00111000) ||
        // purple case, (x+2,y), 00110000
        isCandidateConnected(source.first+2, source.second, 0b00110000) ||
        // green case, (x,y-1), 00000001
        isCandidateConnected(source.first, source.second-1, 0b00000001);

      case 0b00001000:
        // orange case, (x+1,y-1), 01110000
        return isCandidateConnected(source.first+1, source.second-1, 0b01110000) ||
        // blue case, (x,y-1), 00000111
        isCandidateConnected(source.first, source.second-1, 0b00000111) ||
        // purple case, (x,y-2), 00000011
        isCandidateConnected(source.first, source.second-2, 0b00000011) ||
        // green case, (x+1,y), 00100000
        isCandidateConnected(source.first+1, source.second, 0b00100000);

      case 0b00010000:
        // blue case, (x-1,y-1), 00001110
        return isCandidateConnected(source.first-1, source.second-1, 0b00001110) ||
        // orange case, (x,y-1), 11100000
        isCandidateConnected(source.first, source.second-1, 0b11100000) ||
        // purple case, (x,y-2), 11000000
        isCandidateConnected(source.first, source.second-2, 0b11000000) ||
        // green case, (x-1,y), 00000100
        isCandidateConnected(source.first-1, source.second, 0b00000100);

      case 0b00100000:
        // orange case, (x-1,y-1), 11000001
        return isCandidateConnected(source.first-1, source.second-1, 0b11000001) ||
        // blue case, (x-1,y), 00011100
        isCandidateConnected(source.first-1, source.second, 0b00011100) ||
        // purple case, (x-2,y), 00001100
        isCandidateConnected(source.first-2, source.second, 0b00001100) ||
        // green case, (x,y-1), 10000000
        isCandidateConnected(source.first, source.second-1, 0b10000000);

      case 0b01000000:
        // blue case, (x-1,y+1), 00111000
        return isCandidateConnected(source.first-1, source.second, 0b10000011) ||
        // orange case, (x-1,y), 10000011
        isCandidateConnected(source.first-1, source.second+1, 0b00111000) ||
        // purple case, (x-2,y), 00000011
        isCandidateConnected(source.first-2, source.second, 0b00000011) ||
        // green case, (x,y+1), 00010000
        isCandidateConnected(source.first, source.second+1, 0b00010000);

      case 0b10000000:
        // orange case, (x-1,y+1), 00000111
        return isCandidateConnected(source.first-1, source.second+1, 0b00000111) ||
        // blue case, (x,y+1), 01110000
        isCandidateConnected(source.first, source.second+1, 0b01110000) ||
        // purple case, (x,y+2), 00110000
        isCandidateConnected(source.first, source.second+2, 0b00110000) ||
        // green case, (x-1,y), 00000010
        isCandidateConnected(source.first-1, source.second, 0b00000010);

      default:
        return true;
    }
}
