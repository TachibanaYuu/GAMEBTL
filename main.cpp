#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>

#include "Defs.hpp"
#include "Structures.hpp"

//CLASS
class ButtonInteract
{
public:
    ButtonInteract();
    ~ButtonInteract();

    void setButtonPosition(int buttonPosX, int buttonPosY);

    void pressRestartButton();
    void pressBackButton();
    void pressSpeakerButton();

    void handleInput();

    void renderingButton(int iLocation, int jLocation);

    void renderingLoss(int iLocation, int jLocation);
private:
    SDL_Point buttonPosition;
};
//VARS
SDL_Window* gameWindow;
SDL_Renderer* gameRenderer;
SDL_Event gameEvent;

TTF_Font* gameFont;

bool isMuting = false;
Mix_Chunk* clickSound = nullptr;
Mix_Music* lossSound = nullptr;
Mix_Music* victorySound = nullptr;
Mix_Music* bgMusic = nullptr;

GameMode gameMode;
GameState gameStatus;
TimeControl timer;

int boardSizeX = 0, boardSizeY = 0;
int mineNumbers = 0;

std::vector <std::vector <int> > gameBoard(4, std::vector<int>(3, 0));
std::vector <std::vector <int> > board (4, std::vector<int>(3, 0));
std::vector <std::vector <ButtonInteract> > square(4, std::vector<ButtonInteract>(3));

int leftMines = mineNumbers;
int leftTiles = boardSizeX * boardSizeY;

int dx(0), dy(0);
int numX(0), numY(0);
int timeNumX(0);

Graphics startScene;
Graphics modeScene;
Graphics instructionScene;
Graphics easyBoard;
Graphics mediumBoard;
Graphics hardBoard;
Graphics backButton;
Graphics UnMuteSpeaker;
Graphics MuteSpeaker;
Graphics victoryFaceDisplay;
Graphics mainFaceDisplay;
Graphics lossFaceDisplay;
Graphics restartButton;
Graphics instructionButton;
Graphics beginButton;
Graphics exitButton;
Graphics beginButtonColored;
Graphics instructionButtonColored;
Graphics exitButtonColored;
Graphics easyModeButton;
Graphics mediumModeButton;
Graphics hardModeButton;
Graphics easyModeButtonColored;
Graphics mediumModeButtonColored;
Graphics hardModeButtonColored;
Graphics boardTiles;
Graphics Numbers;
SDL_Rect NumberSprite[MAX_NUMBER_SPRITE];
SDL_Rect TileStatus[MAX_TILE_STATE];

ButtonInteract backIcon;
ButtonInteract restartIcon;
ButtonInteract speaker;

//FUNCS
void initGame();
void loadGameAssets();
void setStartScene();
void showStartScene();
void showModeScene();
void showInstructionScene();
void getPlayerInput();
void positingButton();
void gamePlaying();
void clearSDL();

int main(int argc, char* argv[])
{
    initGame();
    loadGameAssets();

    showStartScene();

    while (gameStatus.isNotClosed)
    {
        if (gameStatus.inInstruction) showInstructionScene();

        if (gameStatus.isChoosingMode) showModeScene();

        while (gameStatus.isNotOver)
        {
            getPlayerInput();
            positingButton();
            gamePlaying();
        }

    }

    clearSDL();

    return 0;
}

void logErrorAndExit(const std::string msg, const std::string mistake)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, msg.c_str(), mistake.c_str());
    SDL_Quit();
}
bool canClick(int mousePosX, int mousePosY, int surroundX, int surroundY, int ww, int hh)
{
    if (mousePosX <= surroundX || mousePosX >= surroundX + ww
        || mousePosY <= surroundY || mousePosY >= surroundY + hh)
            return false;
    return true;
}

Graphics::Graphics()
{
    grTexture = nullptr;
    grWidth = 0;
    grHeight = 0;
}
Graphics::~Graphics()
{
    cleanGraphics();
}
void Graphics::cleanGraphics()
{
    if(grTexture != nullptr)
    {
        SDL_DestroyTexture(grTexture);
        grTexture = nullptr;
        grWidth = 0;
        grHeight = 0;
    }
}
int Graphics::getGrWidth()
{
    return grWidth;
}
int Graphics::getGrHeight()
{
    return grHeight;
}
bool Graphics::loadFromFile(const std::string filepath)
{
    cleanGraphics();

    SDL_Texture* storedGrTexture = nullptr;
    SDL_Surface* storedGrSurface = IMG_Load(filepath.c_str());

    SDL_SetColorKey(storedGrSurface, SDL_TRUE, SDL_MapRGB(storedGrSurface->format, 0, 0xFF, 0xFF));

    storedGrTexture = SDL_CreateTextureFromSurface(gameRenderer, storedGrSurface);
    if (storedGrTexture != nullptr){
        grWidth = storedGrSurface->w;
        grHeight = storedGrSurface->h;
    }
    SDL_FreeSurface(storedGrSurface);

    grTexture = storedGrTexture;

    return (grTexture != nullptr);
}
bool Graphics::loadFromRenderedText(std::string grText, SDL_Color crayon)
{
    cleanGraphics();

    SDL_Surface* storedGrTextSurface = TTF_RenderText_Solid(gameFont, grText.c_str(), crayon);
    if (storedGrTextSurface != nullptr)
    {
        grTexture = SDL_CreateTextureFromSurface(gameRenderer, storedGrTextSurface);
        if (grTexture != nullptr)
        {
            grWidth = storedGrTextSurface->w;
            grHeight = storedGrTextSurface->h;
        }
        SDL_FreeSurface(storedGrTextSurface);
    }
    return (grTexture != nullptr);
}
void Graphics::render(int renderPosX, int renderPosY, SDL_Rect* clip)
{
    SDL_Rect renderSpace = {renderPosX, renderPosY, grWidth, grHeight};

    if (clip != nullptr)
    {
        renderSpace.w = clip->w;
        renderSpace.h = clip->h;
    }

    SDL_RenderCopy(gameRenderer, grTexture, clip, &renderSpace);
}
void randomMine(int &bomb)
{
    while (bomb < mineNumbers)
    {
        int iLocation = rand() % boardSizeX;
        int jLocation = rand() % boardSizeY;
        if (board[iLocation][jLocation] == BOMB_TILE) continue;
        board[iLocation][jLocation] = BOMB_TILE;
        bomb++;
    }
}
void generateBoard()
{
    srand(time(0));
    int bomb = 0;

    for (int i = 0; i < boardSizeX; i++)
    {
        for (int j = 0; j < boardSizeY; j++)
        {
            gameBoard[i][j] = UNPRESSED_TILE;
            board[i][j] = PRESSED_TILE;
        }
    }
    randomMine(bomb);

    for (int i = 0; i < boardSizeX; i++)
    {
        for (int j = 0; j < boardSizeY; j++)
        {
            if (board[i][j] == BOMB_TILE) continue;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    int posX = i + x;
                    int posY = j + y;
                    if (posX < 0 || posX > boardSizeX-1 || posY < 0 || posY > boardSizeY-1) continue;
                    if (board[posX][posY] == BOMB_TILE) board[i][j]++;

                }
            }
        }
    }
}
void winCondition()
{
    if (leftTiles == mineNumbers) gameStatus.isWinning = true;
}
void playAgain()
{
    timer.start();
    generateBoard();
    Mix_HaltMusic();
    leftMines = mineNumbers;
    leftTiles = boardSizeX * boardSizeY;
    gameStatus.isWinning = false;
    gameStatus.isLosing = false;
    gameStatus.v_playAgain = false;
}
void GameResponse()
{
    if (gameStatus.v_playAgain == true) playAgain();
    if (gameStatus.isLosing == true)
    {
        timer.pause();
        lossFaceDisplay.render(boardSizeX*TILE_SIZE/2 - 20, numY);
        for (int i = 0; i < boardSizeX; i++)
        {
            for (int j = 0; j < boardSizeY; j++)
            {
                square[i][j].renderingLoss(i, j);
            }
        }
    }
    if (gameStatus.isWinning == true)
    {
        timer.pause();
        victoryFaceDisplay.render(boardSizeX * TILE_SIZE / 2 - 20, numY);
        for (int i = 0; i < boardSizeX; i++)
        {
            for (int j = 0; j < boardSizeY; j++)
            {
                square[i][j].renderingLoss(i, j);
            }
        }
    }
}
void mineManager()
{
    int tmp = leftMines;
    if (leftMines < 10)
    {
        Numbers.render(numX, numY, &NumberSprite[0]);
        for (int i = 0; i <= 9; i++)
        {
            if (i == leftMines) Numbers.render(numX + TILE_SIZE, numY, &NumberSprite[i]);
        }
    }
    else
    {
        int i = 0;
        while (tmp > 0)
        {
            int x = tmp%10;
            tmp/=10;
            Numbers.render(numX + (1-i)*TILE_SIZE, numY, &NumberSprite[x]);
            i++;
        }
    }
}
void timeDeal()
{
    int tmp = timer.getTicks() / MAX_SEC;
    if (tmp < 10)
    {
        for (int i = 0; i <= 0; i++)
        {
            if (i == tmp) Numbers.render(timeNumX, numY, &NumberSprite[tmp]);
        }
    }
    else
    {
        int i = 0;
        while (tmp > 0)
        {
            int x = tmp%10;
            tmp/=10;
            Numbers.render(timeNumX - i*TILE_SIZE, numY, &NumberSprite[x]);
            i++;
        }
    }

}
void setGameMode(int x, int y, int n, int ddx, int ddy, int nummX, int nummY, int dtx)
{
    boardSizeX = x;
    boardSizeY = y;
    mineNumbers = n;
    leftMines = n;
    leftTiles = x*y;
    dx = ddx;
    dy = ddy;
    numX = nummX;
    numY = nummY;
    timeNumX = dtx;

    square.resize(boardSizeX);
    for (int i = 0; i < boardSizeX; i++)
    {
        square[i].resize(boardSizeY);
    }
    gameBoard.resize(boardSizeX);
    for (int i = 0; i < boardSizeX; i++)
    {
        gameBoard[i].resize(boardSizeY);
    }
    board.resize(boardSizeX);
    for (int i = 0; i < boardSizeX; i++)
    {
        board[i].resize(boardSizeY);
    }
}
void renderSquare()
{
    for (int i = 0; i < boardSizeX; i++)
    {
        for (int j = 0; j < boardSizeY; j++)
        {
            square[i][j].renderingButton(i, j);
        }
    }
}
void spoil(int iLocation, int jLocation)
{
    if (gameBoard[iLocation][jLocation] == UNPRESSED_TILE
        || gameBoard[iLocation][jLocation] == FLAG_TILE)
    {
        if (gameBoard[iLocation][jLocation] == FLAG_TILE) return;
        gameBoard[iLocation][jLocation] = board[iLocation][jLocation];
        if (gameBoard[iLocation][jLocation] != BOMB_TILE) leftTiles--;
        if (gameBoard[iLocation][jLocation] == PRESSED_TILE)
        {
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    int posX = iLocation + x;
                    int posY = jLocation + y;
                    if (posX < 0 || posX > boardSizeX - 1 || posY < 0 || posY > boardSizeY-1) continue;
                    spoil(posX, posY);
                }
            }
        }
    }
}
ButtonInteract::ButtonInteract()
{
    buttonPosition.x = 0;
    buttonPosition.y = 0;
}
ButtonInteract::~ButtonInteract()
{

}
void ButtonInteract::setButtonPosition(int buttonPosX, int buttonPosY)
{
    buttonPosition.x = buttonPosX;
    buttonPosition.y = buttonPosY;
}
void ButtonInteract::pressBackButton()
{
    if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        int mousePosX, mousePosY;
        SDL_GetMouseState(&mousePosX, &mousePosY);
        if(canClick(mousePosX, mousePosY, buttonPosition.x, buttonPosition.y, BACK_BUTTON_DEF, BACK_BUTTON_DEF))
        {
            if (gameEvent.button.button == SDL_BUTTON_LEFT)
            {
                SDL_SetWindowSize(gameWindow, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
                gameStatus.isNotOver = false;
                gameStatus.isChoosingMode = true;
                gameStatus.isWinning = false;
                gameStatus.isLosing = false;
                gameMode.easy = false;
                gameMode.medium = false;
                gameMode.hard = false;
            }
        }
    }
}
void ButtonInteract::pressRestartButton()
{
    if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        int mousePosX, mousePosY;
        SDL_GetMouseState(&mousePosX, &mousePosY);
        if (canClick(mousePosX, mousePosY, buttonPosition.x, buttonPosition.y, RESTART_BUTTON_DEF, RESTART_BUTTON_DEF))
        {
            if (gameEvent.button.button == SDL_BUTTON_LEFT)
            {
                playAgain();
            }
        }
    }
}
void ButtonInteract::pressSpeakerButton()
{
    if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        int mousePosX, mousePosY;
        SDL_GetMouseState(&mousePosX, &mousePosY);
        if (canClick(mousePosX, mousePosY, buttonPosition.x, buttonPosition.y, SPEAKER_BUTTON_DEF, SPEAKER_BUTTON_DEF))
        {
            if (gameEvent.button.button == SDL_BUTTON_LEFT)
            {
                if (isMuting == true) isMuting = false;
                else isMuting = true;
            }
        }
    }
}
void ButtonInteract::handleInput()
{
    if (gameEvent.type == SDL_MOUSEMOTION
        || gameEvent.type == SDL_MOUSEBUTTONDOWN
        || gameEvent.type == SDL_MOUSEBUTTONUP)
    {
        int mousePosX, mousePosY;
        SDL_GetMouseState(&mousePosX, &mousePosY);

        int iLocation = (mousePosX - dx) / TILE_SIZE;
        int jLocation = (mousePosY - dy) / TILE_SIZE;

        if(canClick(mousePosX, mousePosY, buttonPosition.x, buttonPosition.y, TILE_SIZE, TILE_SIZE))
        {
            if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if (gameEvent.button.button == SDL_BUTTON_LEFT)
                {
                    spoil(iLocation, jLocation);
                    if (leftTiles == mineNumbers) Mix_PlayMusic(victorySound, 1);
                    if (gameBoard[iLocation][jLocation] == BOMB_TILE)
                    {
                        gameStatus.isLosing = true;
                        Mix_PlayMusic(lossSound, 1);
                    }
                    else Mix_PlayChannel(-1, clickSound, 0);
                }
                if (gameEvent.button.button == SDL_BUTTON_RIGHT)
                {
                    Mix_PlayChannel(-1, clickSound, 0);
                    if (gameBoard[iLocation][jLocation] >= 10)
                    {
                        if (gameBoard[iLocation][jLocation] == FLAG_TILE)
                        {
                            gameBoard[iLocation][jLocation] = UNPRESSED_TILE;
                            leftMines+=1;
                        }
                        else
                        {
                            gameBoard[iLocation][jLocation] = FLAG_TILE;
                            leftMines-=1;
                        }
                    }
                }
            }
        }
    }
}
void ButtonInteract::renderingButton(int iLocation, int jLocation)
{
    boardTiles.render(buttonPosition.x, buttonPosition.y, &TileStatus[gameBoard[iLocation][jLocation]]);
}
void ButtonInteract::renderingLoss(int iLocation, int jLocation)
{
    boardTiles.render(buttonPosition.x, buttonPosition.y, &TileStatus[board[iLocation][jLocation]]);
}
void initIMG()
{
    if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
        logErrorAndExit("sdl_img", IMG_GetError());
}
void initFont()
{
    if(TTF_Init() == -1)
        logErrorAndExit("sdl_ttf", TTF_GetError());
}
void initAudio()
{
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        logErrorAndExit("SDL_mixer", Mix_GetError());
}
void initGame()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        logErrorAndExit("initSDL", SDL_GetError());

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    gameWindow = SDL_CreateWindow(GAME_TITLE,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT,
                                  SDL_WINDOW_SHOWN);
    if (gameWindow == nullptr) logErrorAndExit("createWindow", SDL_GetError());

    gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gameRenderer == nullptr) logErrorAndExit("createRenderer", SDL_GetError());

    initIMG();
    initFont();
    initAudio();
}
void loadFont()
{
    gameFont = TTF_OpenFont("resources/font/font.ttf", FONT_PTSIZE);
    if (gameFont == nullptr) logErrorAndExit("fontload", TTF_GetError());
}
void loadAudio()
{
    clickSound = Mix_LoadWAV("resources/audio/clickSound.wav");
    if (clickSound == nullptr) logErrorAndExit("clickSound", Mix_GetError());

    victorySound = Mix_LoadMUS("resources/audio/victorySound.wav");
    if (victorySound == nullptr) logErrorAndExit("victorySound", Mix_GetError());

    lossSound = Mix_LoadMUS("resources/audio/lossSound.wav");
    if (lossSound == nullptr) logErrorAndExit("lossSound", Mix_GetError());

    bgMusic = Mix_LoadMUS("resources/audio/bgm.wav");
    if (bgMusic == nullptr) logErrorAndExit("bgm", Mix_GetError());
}
void loadTilesIMG()
{
    if(!boardTiles.loadFromFile("resources/img/tilesIMG.jpg"))
        logErrorAndExit("tiles", SDL_GetError());
    else
    {
        for (int i = 0; i < MAX_TILE_STATE; i++)
        {
            TileStatus[i].x = i * TILE_SIZE;
            TileStatus[i].y = 0;
            TileStatus[i].w = TILE_SIZE;
            TileStatus[i].h = TILE_SIZE;
        }
    }
}
void loadNumsIMG()
{
    if(!Numbers.loadFromFile("resources/img/NumbersIMG.png"))
       logErrorAndExit("numbers", SDL_GetError());
    else{
        for (int i = 0; i < MAX_NUMBER_SPRITE; i++)
        {
            NumberSprite[i].x = i * TILE_SIZE;
            NumberSprite[i].y = 0;
            NumberSprite[i].w = TILE_SIZE;
            NumberSprite[i].h = 2*TILE_SIZE-10;
        }
    }
}
void loadIMG()
{
    loadTilesIMG();
    loadNumsIMG();

    if (!startScene.loadFromFile("resources/img/startScene.jpg"))
        logErrorAndExit("startScene", SDL_GetError());

    if (!instructionScene.loadFromFile("resources/img/instructionScene.png"))
        logErrorAndExit("instructionScene", SDL_GetError());

    if (!modeScene.loadFromFile("resources/img/modeScene.png"))
        logErrorAndExit("modeScene", SDL_GetError());

    if (!easyBoard.loadFromFile("resources/img/easyBoard.png"))
        logErrorAndExit("easyboard", SDL_GetError());

    if (!mediumBoard.loadFromFile("resources/img/mediumBoard.png"))
        logErrorAndExit("mediumboard", SDL_GetError());

    if (!hardBoard.loadFromFile("resources/img/hardBoard.png"))
        logErrorAndExit("hardboard", SDL_GetError());

    if (!backButton.loadFromFile("resources/img/backIcon.jpg"))
        logErrorAndExit("backicon", SDL_GetError());

    if (!MuteSpeaker.loadFromFile("resources/img/muteIcon.png"))
        logErrorAndExit("muteIcon", SDL_GetError());

    if(!UnMuteSpeaker.loadFromFile("resources/img/unmuteIcon.jpg"))
        logErrorAndExit("unmuteicon", SDL_GetError());

    if(!restartButton.loadFromFile("resources/img/restartIcon.png"))
        logErrorAndExit("restarticon", SDL_GetError());

    if (!victoryFaceDisplay.loadFromFile("resources/img/winFace.png"))
        logErrorAndExit("winface", SDL_GetError());

    if(!mainFaceDisplay.loadFromFile("resources/img/mainPlayingFace.png"))
        logErrorAndExit("mainplayingface", SDL_GetError());

    if(!lossFaceDisplay.loadFromFile("resources/img/lossFace.png"))
        logErrorAndExit("lossface", SDL_GetError());
}
void loadText()
{
    SDL_Color whiteCrayon = {255, 255, 255};
    SDL_Color yellowCrayon = {255, 255, 0};

    if (!beginButton.loadFromRenderedText("START", whiteCrayon))
        logErrorAndExit("whiteStart", SDL_GetError());
    if (!beginButtonColored.loadFromRenderedText("START", yellowCrayon))
        logErrorAndExit("yellowStart", SDL_GetError());

    if (!instructionButton.loadFromRenderedText("INSTRUCTS", whiteCrayon))
        logErrorAndExit("whiteInstruct", SDL_GetError());
    if (!instructionButtonColored.loadFromRenderedText("INSTRUCTS", yellowCrayon))
        logErrorAndExit("yellowInstruct", SDL_GetError());

    if (!exitButton.loadFromRenderedText("EXIT", whiteCrayon))
        logErrorAndExit("whiteExit", SDL_GetError());
    if (!exitButtonColored.loadFromRenderedText("EXIT", yellowCrayon))
        logErrorAndExit("yellowExit", SDL_GetError());

    SDL_Color greenCrayon = {0, 255, 0};
    SDL_Color orangeCrayon = {255, 128, 0};
    SDL_Color redCrayon = {255, 0, 0};

    if (!easyModeButton.loadFromRenderedText("EASY MODE", whiteCrayon))
        logErrorAndExit("whiteEz", SDL_GetError());
    if (!easyModeButtonColored.loadFromRenderedText("EASY MODE", greenCrayon))
        logErrorAndExit("greenEz", SDL_GetError());

    if (!mediumModeButton.loadFromRenderedText("MEDIUM MODE", whiteCrayon))
        logErrorAndExit("whiteMed", SDL_GetError());
    if (!mediumModeButtonColored.loadFromRenderedText("MEDIUM MODE", orangeCrayon))
        logErrorAndExit("orangeMed", SDL_GetError());

    if (!hardModeButton.loadFromRenderedText("HARD MODE", whiteCrayon))
        logErrorAndExit("whiteHard", SDL_GetError());
    if (!hardModeButtonColored.loadFromRenderedText("HARD MODE", redCrayon))
        logErrorAndExit("redHard", SDL_GetError());
}
void loadGameAssets()
{
    loadAudio();
    loadIMG();
    loadFont();
    loadText();
}

void setStartScene()
{
    startScene.render(0, 0);
    beginButton.render(START_BUTTON_POSX, START_SCENE_BUTTON_POSY);
    instructionButton.render(INSTRUCTION_BUTTON_POSX, START_SCENE_BUTTON_POSY);
    exitButton.render(EXIT_BUTTON_POSX, START_SCENE_BUTTON_POSY);
    SDL_RenderPresent(gameRenderer);
}
void showStartScene()
{
    Mix_PlayMusic(bgMusic, 5);
    bool startButtonClick = false;
    bool instructionButtonClick = false;
    bool exitButtonClick = false;
    bool isStartingScene = true;

    setStartScene();
    while (isStartingScene)
    {
        while (SDL_PollEvent(&gameEvent) != 0)
        {
            if (gameEvent.type == SDL_QUIT
                || (gameEvent.type == SDL_KEYDOWN && gameEvent.key.keysym.sym == SDLK_ESCAPE))
            {
                        gameStatus.isNotClosed = false;
                        isStartingScene = false;
            }
            if (gameEvent.type == SDL_MOUSEBUTTONDOWN || gameEvent.type == SDL_MOUSEMOTION)
            {
                int mousePosX, mousePosY;
                SDL_GetMouseState(&mousePosX, &mousePosY);
                //std::cout << mousePosX << " " << mousePosY << std::endl;
                if(canClick(mousePosX, mousePosY, START_BUTTON_POSX, START_SCENE_BUTTON_POSY, beginButton.getGrWidth(), beginButton.getGrHeight()))
                    startButtonClick = true;
                else startButtonClick = false;

                if (canClick(mousePosX, mousePosY, INSTRUCTION_BUTTON_POSX, START_SCENE_BUTTON_POSY, instructionButton.getGrWidth(), instructionButton.getGrHeight()))
                    instructionButtonClick = true;
                else instructionButtonClick = false;

                if (canClick(mousePosX, mousePosY, EXIT_BUTTON_POSX, START_SCENE_BUTTON_POSY, exitButton.getGrWidth(), exitButton.getGrHeight()))
                    exitButtonClick = true;
                else exitButtonClick = false;

                if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
                {
                    if(gameEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        if (startButtonClick)
                        {
                            gameStatus.isStarting = true;
                            isStartingScene = false;
                            gameStatus.inInstruction = false;
                        }
                        if (exitButtonClick)
                        {
                            gameStatus.isNotClosed = false;
                            isStartingScene = false;
                        }
                        if (instructionButtonClick)
                        {
                            gameStatus.inInstruction = true;
                            isStartingScene = false;
                            gameStatus.isStarting = false;
                        }
                    }
                }
                if (gameEvent.type == SDL_MOUSEMOTION)
                {
                    if (startButtonClick)
                        beginButtonColored.render(START_BUTTON_POSX, START_SCENE_BUTTON_POSY);
                    else beginButton.render(START_BUTTON_POSX, START_SCENE_BUTTON_POSY);

                    if (instructionButtonClick)
                        instructionButtonColored.render(INSTRUCTION_BUTTON_POSX, START_SCENE_BUTTON_POSY);
                    else instructionButton.render(INSTRUCTION_BUTTON_POSX, START_SCENE_BUTTON_POSY);

                    if (exitButtonClick)
                        exitButtonColored.render(EXIT_BUTTON_POSX, START_SCENE_BUTTON_POSY);
                    else exitButton.render(EXIT_BUTTON_POSX, START_SCENE_BUTTON_POSY);
                }
            }
            SDL_RenderPresent(gameRenderer);
        }
    }

}

void setModeScene()
{
    modeScene.render(0, 0);
    easyModeButton.render(EASYMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
    mediumModeButton.render(MEDIUMMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
    hardModeButton.render(HARDMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
}
void easyModeShowUp()
{
    gameStatus.isNotOver = true;
    gameStatus.isChoosingMode = false;
    timer.start();
    gameMode.easy = true;
    gameMode.medium = false;
    gameMode.hard = false;
    SDL_SetWindowSize(gameWindow, EASYMODE_SCREEN_WIDTH, EASYMODE_SCREEN_HEIGHT);
    setGameMode(EASYMODE_BOARD_SIZE_X, EASYMODE_BOARD_SIZE_Y, EASYMODE_BOMBS_COUNT, DX, DY, NUMX, NUMY, EASYMODE_TIMENUMX);
    generateBoard();
}
void mediumModeShowUp()
{
    gameStatus.isNotOver = true;
    gameStatus.isChoosingMode = false;
    timer.start();
    gameMode.easy = false;
    gameMode.medium = true;
    gameMode.hard = false;
    SDL_SetWindowSize(gameWindow, MEDIUMMODE_SCREEN_WIDTH, MEDIUMMODE_SCREEN_HEIGHT);
    setGameMode(MEDIUMMODE_BOARD_SIZE_X, MEDIUMMODE_BOARD_SIZE_Y, MEDIUMMODE_BOMBS_COUNT, DX, DY, NUMX, NUMY, MEDIUMMODE_TIMENUMX);
    generateBoard();
}
void hardModeShowUp()
{
    gameStatus.isNotOver = true;
    gameStatus.isChoosingMode = false;
    timer.start();
    gameMode.easy = false;
    gameMode.medium = false;
    gameMode.hard = true;
    SDL_SetWindowSize(gameWindow, HARDMODE_SCREEN_WIDTH, HARDMODE_SCREEN_HEIGHT);
    setGameMode(HARDMODE_BOARD_SIZE_X, HARDMODE_BOARD_SIZE_Y, HARDMODE_BOMBS_COUNT, DX, DY, NUMX, NUMY, HARDMODE_TIMENUMX);
    generateBoard();
}
void showModeScene()
{
    bool easyModeButtonClick = false;
    bool mediumModeButtonClick = false;
    bool hardModeButtonClick = false;
    setModeScene();

    while (gameStatus.isChoosingMode)
    {
        while(SDL_PollEvent(&gameEvent) != 0)
        {
            if (gameEvent.type == SDL_QUIT
                || (gameEvent.type == SDL_KEYDOWN && gameEvent.key.keysym.sym == SDLK_ESCAPE))
            {
                gameStatus.isNotClosed = false;
                gameStatus.isChoosingMode = false;
            }
            if (gameEvent.type == SDL_MOUSEMOTION || gameEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                int mousePosX, mousePosY;
                SDL_GetMouseState(&mousePosX, &mousePosY);
                if (canClick(mousePosX, mousePosY, EASYMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY, easyModeButton.getGrWidth(), easyModeButton.getGrHeight()))
                    easyModeButtonClick = true;
                else easyModeButtonClick = false;

                if (canClick(mousePosX, mousePosY, MEDIUMMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY, mediumModeButton.getGrWidth(), mediumModeButton.getGrHeight()))
                    mediumModeButtonClick = true;
                else mediumModeButtonClick = false;

                if (canClick(mousePosX, mousePosY, HARDMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY, hardModeButton.getGrWidth(), hardModeButton.getGrHeight()))
                    hardModeButtonClick = true;
                else hardModeButtonClick = false;

                if (gameEvent.type == SDL_MOUSEMOTION)
                {
                    if (easyModeButtonClick) easyModeButtonColored.render(EASYMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
                    else easyModeButton.render(EASYMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);

                    if (mediumModeButtonClick) mediumModeButtonColored.render(MEDIUMMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
                    else mediumModeButton.render(MEDIUMMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);

                    if (hardModeButtonClick) hardModeButtonColored.render(HARDMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
                    else hardModeButton.render(HARDMODE_BUTTON_POSX, MODE_SCENE_BUTTON_POSY);
                }
                if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (gameEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        if (easyModeButtonClick) easyModeShowUp();
                        if (mediumModeButtonClick) mediumModeShowUp();
                        if (hardModeButtonClick) hardModeShowUp();
                    }
                }
                SDL_RenderPresent(gameRenderer);
            }
        }
    }
}
void showInstructionScene()
{
    instructionScene.render(0, 0);
    while (gameStatus.inInstruction)
    {
        while (SDL_PollEvent(&gameEvent) != 0)
        {
            if (gameEvent.type == SDL_MOUSEBUTTONDOWN && gameEvent.button.button == SDL_BUTTON_RIGHT)
            {
                gameStatus.inInstruction = false;
                gameStatus.isChoosingMode = true;
            }
            SDL_RenderPresent(gameRenderer);
        }
    }
}
void getPlayerInput()
{
    while (SDL_PollEvent(&gameEvent) != 0)
    {
        if (gameEvent.type == SDL_QUIT)
        {
            gameStatus.isNotOver = false;
            gameStatus.isNotClosed = false;
        }
        restartIcon.pressRestartButton();
        backIcon.pressBackButton();
        speaker.pressSpeakerButton();
        for (int i = 0; i < boardSizeX; i++)
        {
            for (int j = 0; j < boardSizeY; j++)
            {
                square[i][j].handleInput();
            }
        }
    }
}
void positingButton()
{
    restartIcon.setButtonPosition(boardSizeX * TILE_SIZE / 2 + BACK_BUTTON_DEF, numY);
    backIcon.setButtonPosition(0, 0);
    speaker.setButtonPosition(timeNumX - 10, 0);
    for (int i = 0; i < boardSizeX; i++)
    {
        for (int j = 0; j < boardSizeY; j++)
        {
            square[i][j].setButtonPosition(i * TILE_SIZE + dx, j * TILE_SIZE + dy);
        }
    }
}
void muteAudio()
{
    clickSound = nullptr;
    victorySound = nullptr;
    lossSound = nullptr;
}
void gamePlaying()
{
    if (isMuting == false)
    {
        SDL_RenderClear(gameRenderer);
        UnMuteSpeaker.render(timeNumX - 10, 0);
        loadAudio();
    }
    else
    {
        SDL_RenderClear(gameRenderer);
        MuteSpeaker.render(timeNumX - 10, 0);
        muteAudio();
    }
    if (gameMode.easy == true || gameMode.medium == true || gameMode.hard == true)
    {
        if (gameMode.easy) easyBoard.render(0, BOARD_ALL_MODE_POSY);
        if (gameMode.medium) mediumBoard.render(0, BOARD_ALL_MODE_POSY);
        if (gameMode.hard) hardBoard.render(0, BOARD_ALL_MODE_POSY);
    }
    mainFaceDisplay.render(boardSizeX * TILE_SIZE / 2 - 20, numY);
    restartButton.render(boardSizeX * TILE_SIZE / 2 + 30, numY);
    renderSquare();
    backButton.render(0, 0);
    mineManager();
    winCondition();
    timeDeal();
    GameResponse();
    SDL_RenderPresent(gameRenderer);
}


void cleanTTF()
{
    TTF_CloseFont(gameFont);
    gameFont = nullptr;
}
void clearSDL()
{
    boardTiles.cleanGraphics();
    Numbers.cleanGraphics();

    cleanTTF();

    SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;

    SDL_DestroyRenderer(gameRenderer);
    gameRenderer = nullptr;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
