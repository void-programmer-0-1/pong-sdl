#ifndef __GAME_H__
#define __GAME_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

using std::cout;
using std::endl;
using std::string;

typedef struct{
    float x;
    float y;
}Vector2;

class Game
{
    public:
        Game();
        bool Initialize();
        void RunLoop();
        void ShutDown();
    private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutput();
        
        SDL_Window* window;
        SDL_Renderer* renderer;
        TTF_Font* font;

        bool isRunning;
        uint8_t ball_thickness;
        uint8_t paddle_height;
        Vector2 ball_pos;
        Vector2 ball_velocity;
        Vector2 user_paddle_pos;
        Vector2 enemy_paddle_pos;
        int user_paddle_dir;
        float paddle_speed;
        bool canEnemyMove;
        int player_score;
        int enemy_score;

        //audio
        Mix_Chunk* burpsound;
        Mix_Chunk* bounce;
        Mix_Chunk* death;
};

#endif // __GAME_H__

// g++ main.cpp Game.hpp Game.cpp -lSDL2 -lSDL2_ttf -o game && ./game