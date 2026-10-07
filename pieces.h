#pragma once

#include <map>
#include <array>

// ——————————————————————————–
// Pieces
// ——————————————————————————–

class Pieces
{
    public:
        enum class PieceType : char
        {
            O = 'O',
            I = 'I',
            L = 'L',
            J = 'J',
            S = 'S',
            Z = 'Z',
            T = 'T'
        };
        
        using Row = std::array<int,5>; // 5* column
        using Rotation = std::array<Row,5>; // 5* rows
        using Piece = std::array<Rotation,4>; // 4* rotations

        struct PieceDefinition
        {
            PieceType type; // associated piece name
            Piece piece; // piece definition
        };
        
        Piece& GetPiece(int type);
        PieceDefinition GetPieceDef(int idx);
        char GetPieceChar(int idx);
        int GetNumPieces();
        int GetPieceIndex(PieceType type);

    private:
        static std::array<PieceDefinition,7> mPieces; // 7* Pieces with names
        
    
};