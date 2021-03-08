#ifndef FILEEXPORTER_H
#define FILEEXPORTER_H

#include <iostream>
#include <fstream>

#include "Chessboard.h"

class FileExporter
{
private:
    uint8_t getRootMove(uint8_t);
    uint8_t getNextMove(Chessboard&, uint8_t, uint8_t);

public:
    void exportPath(Chessboard &, uint16_t);
};

#endif /* FILEEXPORTER_H */