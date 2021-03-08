#ifndef LONGESTUNCROSSEDKNIGHTSPATH_H
#define LONGESTUNCROSSEDKNIGHTSPATH_H

#include <iostream>
#include <stdint.h>

#include "Chessboard.h"
#include "FileExporter.h"

class LongestUncrossedKnightsPath {
 
    private:
        Chessboard chessboard;

        uint16_t longestPathLength = 0;
        std::vector<Chessboard> longestPathBoards;

        uint8_t getStartingMoves(Chessboard&, std::pair<uint8_t, uint8_t>);
        void startBacktracking(Chessboard&, std::pair<uint8_t, uint8_t>);
        void backtracking(Chessboard&, std::pair<uint8_t, uint8_t>, uint8_t);

        void checkForLongestPath(Chessboard&);

    public:
        LongestUncrossedKnightsPath(uint8_t, uint8_t);
        virtual ~LongestUncrossedKnightsPath();

        void run();
        void runFrom(uint8_t x, uint8_t y);

        void exportLongestPath() const;
};

#endif /* LONGESTUNCROSSEDKNIGHTSPATH_H */
