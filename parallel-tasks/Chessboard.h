#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <vector>
#include <bitset>
#include <iostream>

#define BORDER_OFFSET 2

class Chessboard {
    private:   
        uint8_t width;
        uint8_t height;

        uint16_t totalWidth;
        uint16_t totalHeight;
        
        std::vector<uint8_t> matrix;
        std::pair<uint8_t, uint8_t> root;

        uint16_t length = 0;

    public:
        Chessboard();
        Chessboard(uint8_t, uint8_t);
        virtual ~Chessboard();

        void setBorder();
        void setField(uint8_t, std::pair<uint8_t, uint8_t>);
        void updateField(uint8_t, std::pair<uint8_t, uint8_t>);
        void setRoot(uint8_t moves, std::pair<uint8_t, uint8_t>);

        uint8_t at(uint8_t, uint8_t) const;
        std::pair<uint8_t, uint8_t> getRoot() const;
        
        uint8_t getWidth() const;
        uint8_t getHeight() const;
        uint16_t getLength() const;

        uint16_t getTotalWidth() const;
        uint16_t getTotalHeight() const;

        void debug() const;

        //Moves
        std::pair<uint8_t, uint8_t> doMove(std::pair<uint8_t, uint8_t>, uint8_t) const;
        uint8_t rotate180(uint8_t) const;

        //Field checking
        bool isFinishable(std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>, uint8_t) const;
        bool isFieldReserved(std::pair<uint8_t, uint8_t>) const;
        bool isCandidateConnected(uint8_t, uint8_t, uint8_t) const;
        bool isConnectionCrossesEdge(std::pair<uint8_t, uint8_t>, uint8_t) const;
};

#endif /* CHESSBOARD_H */
