#include "game.h"

static constexpr std::array<std::array<Game::Kick, 5>, 4> JLSTZ_KICKS =
{{
    // 0 -> R
    {{{ 0, 0}, {-1, 0}, {-1,-1}, { 0, 2}, {-1, 2}}},

    // R -> 2
    {{{ 0, 0}, { 1, 0}, { 1, 1}, { 0,-2}, { 1,-2}}},

    // 2 -> L
    {{{ 0, 0}, { 1, 0}, { 1,-1}, { 0, 2}, { 1, 2}}},

    // L -> 0
    {{{ 0, 0}, {-1, 0}, {-1, 1}, { 0,-2}, {-1,-2}}}
}};

static constexpr std::array<std::array<Game::Kick, 5>, 4> I_KICKS =
{{
    // 0 -> R
    {{{ 0, 0}, {-2, 0}, { 1, 0}, {-2, 1}, { 1,-2}}},

    // R -> 2
    {{{ 0, 0}, {-1, 0}, { 2, 0}, {-1,-2}, { 2, 1}}},

    // 2 -> L
    {{{ 0, 0}, { 2, 0}, {-1, 0}, { 2,-1}, {-1, 2}}},

    // L -> 0
    {{{ 0, 0}, { 1, 0}, {-2, 0}, { 1, 2}, {-2,-1}}}
}};

static constexpr std::array AllPieces = 
{
    Pieces::PieceType::I,
    Pieces::PieceType::J,
    Pieces::PieceType::L,
    Pieces::PieceType::O,
    Pieces::PieceType::S,
    Pieces::PieceType::T,
    Pieces::PieceType::Z
};

Pieces::PieceDefinition Game::GetNextPiece(int pos)
{
    if (mPieceBag.size() <= pos)
    {
        std::array newBag = AllPieces;

        std::shuffle(
            newBag.begin(),
            newBag.end(),
            mRandomEngine
        );

        mPieceBag.insert(
            mPieceBag.end(),
            newBag.begin(),
            newBag.end()
        );
    }

    Pieces::PieceType pType = mPieceBag[pos];
    
    int pIdx = Pieces().GetPieceIndex(pType);
    Pieces::PieceDefinition piece = Pieces().GetPieceDef(pIdx);
    return piece;
}

const Game::Grid& Game::GetGrid() const
{
    return mGrid;
}

const Game::ActivePiece& Game::GetActivePiece() const
{
    return mActivePiece;
}

void Game::SpawnPiece(const Pieces::PieceDefinition& pPiece)
{
    mPieceBag.pop_front(); // Remove next piece
    if (Game::CanPlace(4,0,0)){
        mActivePiece.pieceDef = pPiece;
        mActivePiece.rotation = 0;
        mActivePiece.x = 4;
        // cout << "Spawning Piece: " << static_cast<char>(mActivePiece.pieceDef.type) << "\n"; 
        switch (mActivePiece.pieceDef.type)
        {
            case Pieces::PieceType::I:
            case Pieces::PieceType::O:
                mActivePiece.y = 0;
                break;

            default:
                mActivePiece.y = 1;
                break;
        }
    }
    else{
        GameOver = true;
        cout << "\nGame Over!\n";
    }
}

int Game::GetSpawnY(const Pieces::PieceDefinition& pieceDef)
{
    const auto& shape = pieceDef.piece[0];

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (shape[row][col] != 0)
            {
                return -row;
            }
        }
    }

    return 0;
}

void Game::Tick()
{
    NumTicks++;

    if (LinesCleared >= Level * 10){
        Level++;
    } 

    double secondsPerRow =
        std::pow(
            0.8 - ((Level - 1) * 0.007),
            Level - 1
        );

    double framesPerRow = secondsPerRow * 60.0;

    if (framesPerRow >= 1.0)
    {
        if (NumTicks >= framesPerRow)
        {
            MoveDown();
            NumTicks = 0;
        }
    }
    else
    {
        // Gravity faster than one row per frame
        double rowsPerFrame = 1.0 / framesPerRow;

        int rowsToMove =
            static_cast<int>(rowsPerFrame);

        for (int i = 0; i < rowsToMove; i++)
        {
            MoveDown();
        }
    }
}

bool Game::CanPlace(int x, int y, int rotation)
{
    const auto& shape =
        mActivePiece.pieceDef.piece[rotation];

    for (int row = 0; row < 5; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (shape[row][col] == 0)
                continue;
 
            int boardX = x + (col - 2);
            int boardY = y + (row - 2);

            if (boardX < 0 || boardX >= 10)
                return false;

            if (boardY >= 20)
                return false;

            if (boardY >= 0 &&
                mGrid[boardY][boardX].has_value())
            {
                return false;
            }
        }
    }

    return true;
}

std::optional<std::pair<int, int>> Game::CanRotate(int x, int y, int newRotation)
{
    if (mActivePiece.pieceDef.type == Pieces::PieceType::O)
    {
        if (CanPlace(x, y, newRotation))
            return std::pair{0, 0};

        return std::nullopt;
    }

    const auto& kicks =
        mActivePiece.pieceDef.type == Pieces::PieceType::I
        ? I_KICKS[mActivePiece.rotation]
        : JLSTZ_KICKS[mActivePiece.rotation];

    for (const auto& [dx, dy] : kicks)
    {
        if (CanPlace(x + dx, y + dy, newRotation))
            return std::pair{dx, dy};
    }

    return std::nullopt;
}

void Game::ClearLines()
{
    int rowsCleared = 0;
    int points = 0;
    for (int row = 0; row < 20; row++)
    {
        bool isFullRow = true;
        for (int col = 0; col < 10; col++)
        {
            if (!mGrid[row][col].has_value())
            {
                isFullRow = false;
                break;
            }
        }

        if (isFullRow)
        {
            for (int r = row; r > 0; r--)
            {
                mGrid[r] = mGrid[r - 1];
            }
            mGrid[0].fill(std::nullopt);
            rowsCleared++;
            LinesCleared++; // Member variable
        }
    }
    switch (rowsCleared){
        case 1:
            points = 40 * (Level);
            Score += points;
            cout << "Single - Points scored: " << Score << "\n";
            break;
        case 2:
            points = 100 * (Level);
            Score += points;
            cout << "Double - Points scored: " << Score << "\n";
            break;
        case 3:
            points = 300 * (Level);
            Score += points;
            cout << "Triple - Points scored: " << Score << "\n";
            break;
        case 4:
            points = 1200 * (Level);
            Score += points;
            cout << "Tetris! - Points scored: " << Score << "\n";
            break;
    }
}

void Game::PlacePiece()
{
    for (int rows=0; rows < 5; rows++)
    {
        for (int cols=0; cols < 5; cols++)
        {
            if (mActivePiece.pieceDef.piece[mActivePiece.rotation][rows][cols] != 0)
            {
                int boardX = mActivePiece.x + (cols - 2);
                int boardY = mActivePiece.y + (rows - 2);

                // cout << "Locked Piece: " << static_cast<char>(mActivePiece.pieceDef.type) << " at position (" << boardX << ", " << boardY << ")\n";
                
                if(boardY >= 0 && boardY < 20 && boardX >= 0 && boardX < 10)
                {
                    mGrid[boardY][boardX] = mActivePiece.pieceDef;
                    
                }
            }
        }
    }
    Game::ClearLines();
    Game::SpawnPiece(GetNextPiece(1));
}

void Game::MoveDown()
{
    if (Game::CanPlace(mActivePiece.x, mActivePiece.y + 1, mActivePiece.rotation))
    {
        mActivePiece.y++;
    }
    else
    {
        Game::PlacePiece();
    }
    return;
}

void Game::MoveLeft()
{
    if (Game::CanPlace(mActivePiece.x - 1, mActivePiece.y, mActivePiece.rotation))
    {
        mActivePiece.x--;
    }
    else
    {
        // cout << "Invalid left move" << "\n";
    }
    return;
}

void Game::MoveRight()
{
    if (Game::CanPlace(mActivePiece.x + 1, mActivePiece.y, mActivePiece.rotation))
    {
        mActivePiece.x++;
    }
    else
    {
        // cout << "Invalid right move" << "\n";
    }
    return;
}

void Game::Rotate()
{
    int newRotation = (mActivePiece.rotation + 1) % 4;
    if (auto kick = CanRotate(mActivePiece.x, mActivePiece.y, newRotation))
    {
        mActivePiece.x += kick->first;
        mActivePiece.y += kick->second;
        mActivePiece.rotation = newRotation;
    }
    else
    {
        // cout << "Invalid rotation" << "\n";
    }
    return;
}

void Game::HardDrop()
{
    while (Game::CanPlace(mActivePiece.x, mActivePiece.y + 1, mActivePiece.rotation))
    {
        mActivePiece.y++;
    }
    Game::PlacePiece();
}

void Game::Update(std::vector<Game::GameAction> actions)
{
    if (!actions.empty()){
        GameAction action = actions.back();
        if (action == GameAction::Pause){
            GamePaused =! GamePaused;
            actions.pop_back();
            cout << "Pause\n";
        }
    }
        
    if (!GameOver && !GamePaused){
        Game::Tick();
        while (!actions.empty())
        {
            GameAction action = actions.back();
            actions.pop_back();

            switch (action)
            {
                case GameAction::MoveLeft:
                    Game::MoveLeft();
                    break;

                case GameAction::MoveRight:
                    Game::MoveRight();
                    break;

                case GameAction::MoveDown:
                    Game::MoveDown();
                    break;

                case GameAction::Rotate:
                    Game::Rotate();
                    break;

                case GameAction::HardDrop:
                    Game::HardDrop();
                    break;

                case GameAction::LevelUp:
                    Level++;
                    break;
                    
                case GameAction::SpawnRandomPiece:
                    Game::SpawnPiece(GetNextPiece(1));
                    break;
            }
        }
    }
    return;
}