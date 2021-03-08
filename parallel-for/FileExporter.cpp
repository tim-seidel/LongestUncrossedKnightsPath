#include "FileExporter.h"

/**
  * @brief  This function returns one move of the root node. 
  *         Because the path it closed, it does not matter which direction is actually chosen, so the first one is taken.
  * @param  moves: The move values of the root
  * @retval The first move that leaves the root
  */
uint8_t FileExporter::getRootMove(uint8_t moves){
    for(uint8_t move = 0b00000001; move>0;move<<=1){
        if((move & moves) != 0){
            return move;
        }
    }
    return 0;
}

/**
  * @brief  This function calculates the next move of a field. It prevents to move backwards.
  * @param  cboard          The finised board
  * @param  moves           The move values of the field
  * @param  previousMove    The previos move that lead to the current field.
  * @retval The move to the next field
  */
uint8_t FileExporter::getNextMove(Chessboard& cboard, uint8_t moves, uint8_t previousMove){
    uint8_t rotatedMove = cboard.rotate180(previousMove);
    for(uint8_t move = 0b00000001; move>0;move<<=1){
        if((move != rotatedMove) && ((move & moves) != 0)){
            return move;
        }
    }
    return 0;
}

/**
  * @brief  This function stores the path of a chessboard in a file.
  * @param  cboard  The chessboard of the path to store
  * @param  number  An additional number that gets added to the file name to allow multiple path files of the same board dimension
  */
void FileExporter::exportPath(Chessboard &cboard, uint16_t number = 1)
{
    std::pair<uint8_t, uint8_t> root = cboard.getRoot();
    //Create and open the file
    char filename[30];
    sprintf(filename, "./paths/%ix%i_%i.path", cboard.getWidth(), cboard.getHeight(), number);
    std::ofstream file(filename);

    //Write board information
    file << "m=" << (int) cboard.getWidth() << std::endl;
    file << "n=" << (int) cboard.getHeight() << std::endl;

    //Getting the first root move
    uint8_t movingMove = getRootMove(cboard.at(root.first, root.second));

    //Iterate over all moves
    std::pair<uint8_t, uint8_t> currentField = root;
    uint16_t move = 0;
    while (!(currentField.first == root.first && currentField.second == root.second) || move == 0)
    {
        //Write the field coordinates to the file
        file << "(" << (int) currentField.first-2 << "," << (int) currentField.second-2 << ")" << std::endl;

        //Get next field and the correct next move, that is not the backwards path
        currentField = cboard.doMove(currentField, movingMove);
        movingMove = getNextMove(cboard, cboard.at(currentField.first, currentField.second), movingMove);
    
        //Break if there was an error with the path, to prevent an infinity loop and file wirting
        move++;
        if(move > cboard.getLength()){
            file << "Path was corrupt. Aborting!" << std::endl;
            break;
        }
        if(movingMove == 0){
            file << "Path was not completed. Aborting!" << std::endl;
        }
    }
    
    file.close();
}
