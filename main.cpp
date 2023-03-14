#include "Game.hpp"

int main()
{
    Game game;
    bool status = game.Initialize();
    if(status == true)
    {
        game.RunLoop();
    }

    return 0;
}