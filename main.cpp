#include <iostream>
using std::cout;
using std::size;

#include <vector>
using std::vector;

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "game.h"

// Window
const int SDL_ORIGIN_WIDTH = 1000;
const int SDL_ORIGIN_HEIGHT = 560;
const int TARGET_FPS = 60;

// Grid
const float GRID_HEIGHT = 560.0f;
const float GRID_WIDTH = 280.0f;
const int GRID_ROWS = 20;
const int GRID_COLS = 10;
const float GRID_CELL_WIDTH = 28.0f;
const float GRID_CELL_HEIGHT = 28.0f;

// Upcoming
const float UPCOMING_WIDTH = (6 * GRID_CELL_WIDTH + 1.0f);
const float UPCOMING_HEIGHT = (10 * GRID_CELL_HEIGHT + 1.0f);

// Text
const float TEXTBOX_WIDTH = (6 * GRID_CELL_WIDTH + 1.0f);
const float TEXTBOX_HEIGHT = (5 * GRID_CELL_WIDTH + 1.0f);

class SDLException final : public std::runtime_error 
{
    public:
        explicit SDLException(const std::string &message) : std::runtime_error(message + '\n' + SDL_GetError()){}
};

struct SDLApplication
{
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
    TTF_TextEngine *mTextEngine;
    TTF_Font *mRenderFont;

    bool mRunning=true;

    // UI element origins
    float mGridOriginX, mGridOriginY;
    float mUpcomingOriginX, mUpcomingOriginY;
    float mTextBoxOriginX, mTextBoxOriginY;

    int mWindowWidth,mWindowHeight;
    const int mFrameDelay = 1000 / TARGET_FPS;
    Game mGame;
    vector<Game::GameAction> mActions;

    SDLApplication(){
        if (!SDL_Init(SDL_INIT_VIDEO)){
            throw SDLException("SDL_Init Failed");
        }

        if (!TTF_Init()){
            throw SDLException("TTF_Init Failed");
        }

        mWindow = {SDL_CreateWindow("Tetris", SDL_ORIGIN_WIDTH, SDL_ORIGIN_HEIGHT, SDL_WINDOW_RESIZABLE)};
        if (!mWindow){
            throw SDLException("Window Creation Failed");
        }

        SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        mSurface = SDL_LoadBMP("assets/tetris.bmp");
        if (!mSurface){
            throw SDLException("Failed to load surface");
        }

        mRenderer = SDL_CreateRenderer(mWindow, nullptr);
        if(!mRenderer){
            throw SDLException("Failed to get window surface");
        }

        mTextEngine = TTF_CreateRendererTextEngine(mRenderer);
        if (!mTextEngine){
            throw SDLException("Failed to create text engine");
        }

        mRenderFont = TTF_OpenFont("assets/Roboto-Medium.ttf", 100.0f);
        if (!mRenderFont){
            throw SDLException(SDL_GetError());
        }

        SDL_SetRenderDrawBlendMode(mRenderer,SDL_BLENDMODE_BLEND);
    }

    ~SDLApplication(){
        SDL_Quit();
        TTF_Quit();
    }

    void SetPieceColour(Pieces::PieceType type, int alpha){
        switch (type)
        {
            case Pieces::PieceType::I:
                SDL_SetRenderDrawColor(mRenderer, 145,215,227,alpha);
                break;

            case Pieces::PieceType::J:
                SDL_SetRenderDrawColor(mRenderer, 138,173,244,alpha);
                break;

            case Pieces::PieceType::L:
                SDL_SetRenderDrawColor(mRenderer, 245,169,127,alpha);
                break;

            case Pieces::PieceType::O:
                SDL_SetRenderDrawColor(mRenderer, 238,212,159,alpha);
                break;

            case Pieces::PieceType::S:
                SDL_SetRenderDrawColor(mRenderer, 166,218,149,alpha);
                break;
                
            case Pieces::PieceType::Z:
                SDL_SetRenderDrawColor(mRenderer, 237,135,150,alpha);
                break;

            case Pieces::PieceType::T:
                SDL_SetRenderDrawColor(mRenderer, 198,160,246,alpha);
                break;
        }
    }

    void RenderBackground(){
        SDL_SetRenderDrawColor(mRenderer, 36,39,58,255);
        SDL_RenderClear(mRenderer);
    }
    
    void RenderGridBackground(){
        // Set window size
        SDL_GetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);
        if(!mWindowWidth or !mWindowHeight){
            throw SDLException("Failed to get window size");
        }
        // Calculate top-left of grid
        mGridOriginX = (mWindowWidth / 2) - (GRID_WIDTH / 2);
        mGridOriginY = (mWindowHeight / 2) - (GRID_HEIGHT / 2);

        // Render grid rect
        SDL_SetRenderDrawColor(mRenderer, 24,25,38,255);
        SDL_FRect rect{
            mGridOriginX,
            mGridOriginY,
            GRID_WIDTH,
            GRID_HEIGHT
        };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    void RenderUpcomingContainer(){
        // Origin is (11,0) cells away from grid origin
        mUpcomingOriginX = mGridOriginX + (11 * GRID_CELL_WIDTH);
        mUpcomingOriginY = mGridOriginY;

        // Render upcoming background
        SDL_FRect rect{
            mUpcomingOriginX,
            mUpcomingOriginY,
            UPCOMING_WIDTH,
            UPCOMING_HEIGHT
        };
        SDL_SetRenderDrawColor(mRenderer, 30,32,48,255);
        SDL_RenderFillRect(mRenderer, &rect);

        // Render upcoming border
        SDL_SetRenderDrawColor(mRenderer, 54, 58, 79, 255);
        SDL_RenderRect(mRenderer, &rect);
    }

    void RenderTextContainer(){
        // Origin is (11,11) cells away from grid origin
        mTextBoxOriginX = mGridOriginX + (11 * GRID_CELL_WIDTH);
        mTextBoxOriginY = mGridOriginY + (11 * GRID_CELL_HEIGHT);

        // Render textbox background
        SDL_FRect rect{
            mTextBoxOriginX,
            mTextBoxOriginY,
            TEXTBOX_WIDTH,
            TEXTBOX_HEIGHT
        };
        SDL_SetRenderDrawColor(mRenderer, 30,32,48,255);
        SDL_RenderFillRect(mRenderer, &rect);

        // Render textbox border
        SDL_SetRenderDrawColor(mRenderer, 54, 58, 79, 255);
        SDL_RenderRect(mRenderer, &rect);
    }

    void RenderGridPieces(){
        const Game::Grid& grid = mGame.GetGrid();
        for(int row = 0; row < 20; row++){
            for(int col = 0; col < 10; col++){

                // Skip empty cells
                if(!grid[row][col].has_value()){
                    continue; 
                }

                // Find type of piece from grid
                Pieces::PieceType pieceType = grid[row][col].value().type;
                float pieceX = mGridOriginX + (col * GRID_CELL_WIDTH);
                float pieceY = mGridOriginY + (row * GRID_CELL_HEIGHT);

                SDL_FRect pieceRect{
                    pieceX,
                    pieceY,
                    GRID_CELL_WIDTH + 1.0f,
                    GRID_CELL_HEIGHT + 1.0f
                };

                SetPieceColour(pieceType,255); // Set correct colour
                SDL_RenderFillRect(mRenderer, &pieceRect);
            }
        }
    }

    void RenderPiece(bool ghost){
        const Game::ActivePiece& activePiece = mGame.GetActivePiece();
        const Pieces::Rotation& shape = activePiece.pieceDef.piece[activePiece.rotation];

        for(int row = 0; row < 5; row++){
            for(int col = 0; col < 5; col++){
                if(shape[row][col] != 0){
                    SetPieceColour(activePiece.pieceDef.type, 255);

                    // If its a ghost piece, fall as far as possible
                    int aPX = activePiece.x;
                    int aPY = activePiece.y;
                    if (ghost){
                        while(mGame.CanPlace(aPX, aPY + 1, activePiece.rotation)){
                            aPY += 1;
                            SetPieceColour(activePiece.pieceDef.type, 80);
                        }
                    }

                    int cellX = aPX + (col - 2);
                    int cellY = aPY + (row - 2);
                    if(cellX < 0 || cellX >= 10 || cellY < 0 || cellY >= 20){
                        continue; // Skip out-of-bounds cells
                    }

                    float pieceX = mGridOriginX + (cellX * GRID_CELL_WIDTH);
                    float pieceY = mGridOriginY + (cellY * GRID_CELL_HEIGHT);
                    SDL_FRect pieceRect{
                        pieceX,
                        pieceY,
                        GRID_CELL_WIDTH + 1.0f,
                        GRID_CELL_HEIGHT + 1.0f
                    };

                    SDL_RenderFillRect(mRenderer, &pieceRect);
                }
            }
        }
    }

    void RenderUpcomingPieces(){
        // Render next piece
        for (int i = 0; i < 3; i++){
            const Pieces::PieceDefinition nextPiece = mGame.GetNextPiece(i+1);
            const Pieces::Rotation nextShape = nextPiece.piece[0];

            for(int row = 0; row < 5; row++){
                for(int col = 0; col < 5; col++){
                    if(nextShape[row][col] != 0){
                        bool typeI = nextPiece.type == Pieces::PieceType::I;
                        bool typeIorO = 
                            typeI ||
                            nextPiece.type == Pieces::PieceType::O;

                        // Origin is (11,0) cells away from grid origin
                        // I and O pieces are 1 row higher
                        int cellX = col + 11;
                        int cellY = 
                            typeIorO 
                            ? (row - 1 + (i * 3)) 
                            : (row + (i * 3));

                        // I or O piece should be half cell offset right
                        float pieceX = 
                            typeIorO
                            ? mGridOriginX + (cellX * GRID_CELL_WIDTH)
                            : mGridOriginX + (cellX * GRID_CELL_WIDTH) + (GRID_CELL_WIDTH / 2);

                        // I piece should be half cell offset down
                        float pieceY = 
                            typeI
                            ? mGridOriginY + (cellY * GRID_CELL_HEIGHT) + (GRID_CELL_HEIGHT / 2)
                            : mGridOriginY + (cellY * GRID_CELL_HEIGHT);

                        SDL_FRect pieceRect{
                            pieceX,
                            pieceY,
                            GRID_CELL_WIDTH + 1.0f,
                            GRID_CELL_HEIGHT + 1.0f
                        };

                        SetPieceColour(nextPiece.type,255);
                        SDL_RenderFillRect(mRenderer, &pieceRect);
                        SDL_SetRenderDrawColor(mRenderer, 54, 58, 79, 255);
                        SDL_RenderRect(mRenderer, &pieceRect);
                    }
                }
            }
        }
    }

    void RenderGridLines(){
        SDL_SetRenderDrawColor(mRenderer, 54, 58, 79, 255);
        
        // Vertical lines
        for (int col = 0; col <= GRID_COLS; col++){
            float lineX = mGridOriginX + GRID_CELL_WIDTH * col;

            SDL_RenderLine(
                mRenderer,
                lineX,
                mGridOriginY,
                lineX,
                mGridOriginY + GRID_HEIGHT
            );
        }

        // Horizontal lines
        for (int row = 0; row <= GRID_ROWS; row++){
            float lineY = mGridOriginY + GRID_CELL_HEIGHT * row;

            SDL_RenderLine(
                mRenderer,
                mGridOriginX,
                lineY,
                mGridOriginX + GRID_WIDTH,
                lineY
            );
        }
    }

    void RenderText(){
        std::string renderString;
        int textSizeX, textSizeY;

        // Game Paused & Game Over
        TTF_SetFontSize(mRenderFont, 100.0f);
        if (mGame.GamePaused or mGame.GameOver){
            renderString = mGame.GameOver ? "Game Over" : "Paused";
            TTF_Text *centerText = 
                TTF_CreateText(
                    mTextEngine,
                    mRenderFont,
                    renderString.c_str(), 
                    renderString.length()
                );

            TTF_SetTextColor(centerText,202,211,245,255);
            if(TTF_GetTextSize(centerText,&textSizeX,&textSizeY)){
                // Center text
                float textOffsetX = (mWindowWidth / 2) - (textSizeX / 2);
                float textOffsetY = (mWindowHeight / 2) - (textSizeY / 2);
                TTF_DrawRendererText(centerText, textOffsetX, textOffsetY);
            }
        }

        // Score
        TTF_SetFontSize(mRenderFont, 20.0f);
        renderString = "Score: " + std::to_string(mGame.Score);
        TTF_Text *scoreText = 
            TTF_CreateText(
                mTextEngine, 
                mRenderFont, 
                renderString.c_str(), 
                renderString.length()
            );

        TTF_SetTextColor(scoreText,184,192,224,255);
        if(TTF_GetTextSize(scoreText,&textSizeX,&textSizeY)){
            // Center x inside of textbox
            float textOffsetX = std::round(mTextBoxOriginX + (TEXTBOX_WIDTH / 2) - (textSizeX / 2));
            float textOffsetY = std::round(mTextBoxOriginY + 20.0f);
            TTF_DrawRendererText(scoreText, textOffsetX, textOffsetY);
        }

        // Level
        TTF_SetFontSize(mRenderFont, 20.0f);
        renderString = "Level: " + std::to_string(mGame.Level);
        TTF_Text *levelText = 
            TTF_CreateText(
                mTextEngine, 
                mRenderFont, 
                renderString.c_str(), 
                renderString.length()
            );

        TTF_SetTextColor(levelText,184,192,224,255);
        if(TTF_GetTextSize(levelText,&textSizeX,&textSizeY)){
            // Center x inside of textbox, y at the bottom of textbox - 2 * fontsize
            float textOffsetX = std::round(mTextBoxOriginX + (TEXTBOX_WIDTH / 2) - (textSizeX / 2));
            float textOffsetY = std::round((mTextBoxOriginY + TEXTBOX_HEIGHT) - 60.0f);
            TTF_DrawRendererText(levelText, textOffsetX, textOffsetY);
        }
        
    }

    void Input(){
        mActions.clear();    // Clear the event queue
        SDL_Event event{0}; // Zero initialised event

        // Event Loop
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                mRunning=false;
            }
            if(event.type == SDL_EVENT_KEY_DOWN){
                switch(event.key.key){
                    case SDLK_LEFT:
                        mActions.push_back(Game::GameAction::MoveLeft);
                        break;

                    case SDLK_RIGHT:
                        mActions.push_back(Game::GameAction::MoveRight);
                        break;

                    case SDLK_UP:
                        mActions.push_back(Game::GameAction::Rotate);
                        break;
                        
                    case(SDLK_DOWN):
                        mActions.push_back(Game::GameAction::MoveDown);
                        break;

                    case(SDLK_SPACE):
                        mActions.push_back(Game::GameAction::HardDrop);
                        break;

                    case(SDLK_A):
                        mActions.push_back(Game::GameAction::LevelUp);
                        break;
                    
                    case(SDLK_LSHIFT):
                        mActions.push_back(Game::GameAction::SpawnRandomPiece);
                        break;

                    case(SDLK_ESCAPE):
                        mActions.push_back(Game::GameAction::Pause);
                        break;
                }
            }
        }
    }

    void Update(){
        if (!mGame.GameOver){
            mGame.Update(mActions);
        }
    }

    void Render(){
        // Backgrounds
        RenderBackground();
        RenderGridBackground();
        
        // Containers
        RenderUpcomingContainer();
        RenderTextContainer();

        // Pieces
        RenderGridPieces();     // Existing Pieces
        RenderUpcomingPieces(); // Upcoming Pieces
        RenderPiece(false);     // Active Piece
        RenderPiece(true);      // Ghost Piece

        RenderGridLines();

        RenderText();

        SDL_RenderPresent(mRenderer);
    }

    void MainLoop(){
        Uint64 fps=0;
        Uint64 lastTick = SDL_GetTicks();

        // Spawn initial piece
        mGame.SpawnPiece(mGame.GetNextPiece(1));

        while(mRunning){
            Uint64 frameStart = SDL_GetTicks();

            Input();
            Update();
            Render();

            fps++;
            Uint64 frameTime = SDL_GetTicks() - frameStart;
            if (mFrameDelay > frameTime) {
                SDL_Delay(mFrameDelay - frameTime); // Cap FPS
            }

            if (frameStart > lastTick + 1000) {
                    std::string title = "Tetris - FPS: " + std::to_string(fps); // Display FPS
                    SDL_SetWindowTitle(mWindow, title.c_str());
                    fps=0;
                    lastTick = frameStart;
            }
        }
    }
};

int main()
{
    SDLApplication app;
    app.MainLoop();
    return 0;
}