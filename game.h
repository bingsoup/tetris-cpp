#pragma once

#include <vector>

#include <iostream>
using std::cout;

#include "pieces.h"

#include <random>

#include <optional>

#include <algorithm>

#include <utility>

#include <deque>

// ——————————————————————————–
// Game
// ——————————————————————————–

class Game 
{
    public:
        enum class GameAction
        {
            MoveLeft,
            MoveRight,
            MoveDown,
            Rotate,
            HardDrop,
            Pause,
            LevelUp,
            SpawnRandomPiece
        };

        void Update(std::vector<Game::GameAction> actions);
        void DisplayPieceData();
        Pieces::PieceDefinition GetNextPiece(int pos);
        void SpawnPiece(const Pieces::PieceDefinition&);
        
        int GetSpawnY(const Pieces::PieceDefinition &pieceDef);

        bool CanPlace(int x, int y, int rotation);

        std::optional<std::pair<int, int>> CanRotate(int x, int y, int rotation);

        void PlacePiece();

        void ClearLines();

        void MoveDown();
        void MoveLeft();
        void MoveRight();
        void Rotate();
        void HardDrop();

        using GridCell = std::optional<Pieces::PieceDefinition>;
        using Grid = std::array<std::array<GridCell, 10>, 20>;

        using Kick = std::pair<int, int>;

        using PieceBag = std::deque<Pieces::PieceType>;


        const Grid& GetGrid() const;

        struct ActivePiece
        {
            Pieces::PieceDefinition pieceDef;
            int rotation = 0;
            int x = 0;
            int y = 0;
        };

        const ActivePiece& GetActivePiece() const;
        int NumTicks = 0;
        
        void Tick();

        bool GameOver = false;
        bool GamePaused = false;

        int Level = 1;
        int LinesCleared = 0;
        int Score = 0;

    private:
        Grid mGrid{};
        ActivePiece mActivePiece{};
        PieceBag mPieceBag{};
        std::mt19937 mRandomEngine{std::random_device{}()};
};