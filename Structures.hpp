#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

struct GameMode
{
    bool easy = false;
    bool medium = false;
    bool hard = false;
};

struct GameState
{
    bool isNotOver = false;
    bool inInstruction = false;
    bool showFirstScene = false;
    bool isNotClosed = true;
    bool isChoosingMode = true;
    bool isWinning = false;
    bool isLosing = false;
    bool isStarting = false;
    bool v_playAgain = false;
};

class Graphics
{
public:
    Graphics();
    ~Graphics();

    void cleanGraphics();

    int getGrWidth();
    int getGrHeight();

    bool loadFromFile(std::string filepath);
    bool loadFromRenderedText(std::string grText, SDL_Color crayon);

    void render(int renderPosX, int renderPosY, SDL_Rect* clip = nullptr);
private:
    SDL_Texture* grTexture;
    int grWidth, grHeight;
};
class TimeControl
{
public:
    TimeControl();
    ~TimeControl();

    void start();
    void pause();
    void stop();

    int getTicks();
    bool getStarted();
    bool getPaused();
private:
    int startTicks;
    int pauseTicks;
    bool isPaused;
    bool isStarted;
};
TimeControl::TimeControl()
{
    startTicks = 0;
    pauseTicks = 0;
    isPaused = false;
    isStarted = false;
}
TimeControl::~TimeControl()
{

}
void TimeControl::start()
{
    isStarted = true;
    isPaused = false;
    int startTicks = SDL_GetTicks();
}
void TimeControl::stop()
{
    isStarted = false;
    isPaused = false;

    startTicks = 0;
    pauseTicks = 0;
}
void TimeControl::pause()
{
    if(isStarted == true)
    {
        if (isPaused == false)
        {
            isPaused = true;
            pauseTicks = SDL_GetTicks() - startTicks;
            startTicks = 0;
        }
    }
}
int TimeControl::getTicks()
{
    if (isStarted == true)
    {
        if (isPaused == true)
        {
            return pauseTicks;
        }
        else return SDL_GetTicks() - startTicks;
    }
    return 0;
}
bool TimeControl::getStarted()
{
    return isStarted;
}
bool TimeControl::getPaused()
{
    return (isStarted && isPaused);
}

#endif // STRUCTURES_HPP
