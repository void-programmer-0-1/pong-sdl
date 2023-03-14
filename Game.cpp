#include "Game.hpp"

Game::Game()
{
    this->isRunning = true;
    this->ball_thickness = 30;
    this->paddle_height = 200;
    this->user_paddle_dir = 0;
    this->paddle_speed = 6;
    this->canEnemyMove = false;
    this->player_score = 0;
    this->enemy_score = 0;
}

bool Game::Initialize()
{
    int video_status = SDL_Init(SDL_INIT_VIDEO);
    int audio_status = SDL_Init(SDL_INIT_AUDIO);

    if(video_status != 0)
    {
        SDL_Log("Unable to initialize video: %s",SDL_GetError());
        return false;
    }

    if(audio_status != 0){
        SDL_Log("Unable to initialize audio: %s",SDL_GetError());
        return false;
    }
    
    this->window = SDL_CreateWindow("Pong",100,100,1024,768,0);
    if(!this->window)
    {
        SDL_Log("Unable to initialize game window: %s",SDL_GetError());
        return false;
    }
    
    this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!this->renderer)
    {
        SDL_Log("Unable to initialize the renderer: %s",SDL_GetError());
        return false;
    }

    if(TTF_Init() == -1)
    {
        SDL_Log("Unable to initialize the TTF module: %s",SDL_GetError());
        return false;
    }

    this->font = TTF_OpenFont("./assets/fonts/ka1.ttf",10);
    if(this->font == nullptr)
    {
        SDL_Log("Unable to initialize the Font: %s",SDL_GetError());
        return false;
    }

    if(Mix_OpenAudio(44100,AUDIO_S16,1,4096) < 0){
        cout << Mix_GetError() << endl;
    }

    Mix_Volume(-1,MIX_MAX_VOLUME);
   
    burpsound = Mix_LoadWAV("./assets/music/jumper.wav"); //jump sound
    bounce = Mix_LoadWAV("./assets/music/bounce.wav");    //bounce sound
    death = Mix_LoadWAV("./assets/music/death.wav");      //death sound

   
    if(burpsound == nullptr){
        std::cout << "Error in loading the music :: " << Mix_GetError() << std::endl; 
    }

    if(bounce == nullptr){
        std::cout << "Error in loading the music :: " << Mix_GetError() << std::endl; 
    }

     if(death == nullptr){
        std::cout << "Error in loading the music :: " << Mix_GetError() << std::endl; 
    }

    this->ball_pos.x = 1024 / 2;
    this->ball_pos.y = 768 / 2;

    this->ball_velocity.x = -200.f;
    this->ball_velocity.y = 235.f;

    this->user_paddle_pos.x = 0;
    this->user_paddle_pos.y = 768 / 2;

    this->enemy_paddle_pos.x = 1024 - this->ball_thickness;
    this->enemy_paddle_pos.y = 768 / 2;
    
    return true;
}

void Game::ShutDown()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    TTF_CloseFont(font);

    Mix_FreeChunk(burpsound);
    Mix_FreeChunk(bounce);
    Mix_FreeChunk(death);
    death = nullptr;
    bounce = nullptr;
    burpsound = nullptr;
    Mix_CloseAudio();

    SDL_Quit();
}

void Game::RunLoop()
{
    while(this->isRunning)
    {
        this->ProcessInput();
        this->UpdateGame();
        this->GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event events;
    while(SDL_PollEvent(&events))
    {
        switch(events.type)
        {
            case SDL_QUIT:
                this->isRunning = false;
                break;
        }
    }
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_ESCAPE])
    {
        this->isRunning = false;
    }
    if(state[SDL_SCANCODE_UP])
    {
        this->user_paddle_dir = -1;
    }
    if(state[SDL_SCANCODE_DOWN])
    {
        this->user_paddle_dir = 1; 
    }
}

void Game::UpdateGame()
{
    this->ball_pos.x += this->ball_velocity.x * 0.03;
    this->ball_pos.y += this->ball_velocity.y * 0.03;
    
    // ball wall collision
    if(this->ball_pos.y >= 768 - this->ball_thickness)
    {
        this->ball_velocity.y *= -1;
        Mix_PlayChannel(-1,bounce,0);
    }
    if(this->ball_pos.y <= this->ball_thickness * 2)
    {
        this->ball_velocity.y *= -1;
        Mix_PlayChannel(-1,bounce,0);
    }

    // player paddle collision with ball
    if((this->ball_pos.x - this->ball_thickness <= this->ball_thickness) && ((this->ball_pos.y - this->ball_thickness <= this->user_paddle_pos.y + this->paddle_height / 2) && (this->ball_pos.y - this->ball_thickness >= this->user_paddle_pos.y - this->paddle_height / 2)))
    {
        this->ball_velocity.x *= -1;
        this->canEnemyMove = true;
            Mix_PlayChannel(-1,burpsound,0);
    }

    // if player lost the ball
    if(this->ball_pos.x < this->ball_thickness)
    {
        // player lost and reset the ball
        this->ball_pos.x = 1024 / 2;
        this->ball_pos.y = 768 / 2;

        this->ball_velocity.x = -200.f;
        this->ball_velocity.y = 235.f;
        
        this->enemy_score += 1;
        Mix_PlayChannel(-1,death,0);
    }

    // enemy paddle collision with ball
    if((this->ball_pos.x >= 1024 - this->ball_thickness) && ((this->ball_pos.y - this->ball_thickness <= this->enemy_paddle_pos.y + this->paddle_height / 2) && (this->ball_pos.y - this->ball_thickness >= this->enemy_paddle_pos.y - this->paddle_height / 2)))
    {
        this->ball_velocity.x *= -1;
        this->canEnemyMove = false;
        Mix_PlayChannel(-1,burpsound,0);
    }

    // if enemy lost the ball
    if(this->ball_pos.x > 1024)
    {
        // enemy lost and reset the ball
        this->ball_pos.x = 1024 / 2;
        this->ball_pos.y = 768 / 2;

        this->ball_velocity.x = -200.f;
        this->ball_velocity.y = 235.f;

        this->player_score += 1;
        Mix_PlayChannel(-1,death,0);
    }

    // user paddle movement code
    this->user_paddle_pos.y += this->user_paddle_dir * this->paddle_speed;
    this->user_paddle_dir = 0;
    
    // user paddle collision with wall
    if(this->user_paddle_pos.y >=768 - this->ball_thickness - this->paddle_height / 2)
    {
        this->user_paddle_pos.y = 768 - this->ball_thickness - this->paddle_height / 2;
    }
    if(this->user_paddle_pos.y <= this->paddle_height / 2 + this->ball_thickness)
    {
        this->user_paddle_pos.y = this->paddle_height / 2 + this->ball_thickness;
    }

    // enemy paddle movement code
    if(this->canEnemyMove)
    {
        float ball_y = this->ball_pos.y;
        if(ball_y > this->enemy_paddle_pos.y)
        {
            this->enemy_paddle_pos.y += 1 * this->paddle_speed + 1;
        }
        if(ball_y < this->enemy_paddle_pos.y)
        {
            this->enemy_paddle_pos.y += -1 * this->paddle_speed - 1;
        }
    }

    // enemy paddle collision with wall code
    if(this->enemy_paddle_pos.y >=768 - this->ball_thickness - this->paddle_height / 2)
    {
        this->enemy_paddle_pos.y = 768 - this->ball_thickness - this->paddle_height / 2;
    }
    if(this->enemy_paddle_pos.y <= this->paddle_height / 2 + this->ball_thickness)
    {
        this->enemy_paddle_pos.y = this->paddle_height / 2 + this->ball_thickness;
    }
}

void Game::GenerateOutput()
{
    // background code
    SDL_SetRenderDrawColor(this->renderer,33,33,32,225); 
    SDL_RenderClear(this->renderer);  

    // score code
    string score = std::to_string(this->player_score) + " " + std::to_string(this->enemy_score);
    SDL_Surface* text_surface = TTF_RenderText_Solid(this->font,score.c_str(),{225,225,225});
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(this->renderer,text_surface);
    SDL_FreeSurface(text_surface);
    SDL_SetRenderDrawColor(this->renderer,255,255,255,255);
    SDL_Rect coordinates;
    coordinates.x = 1024 / 2 - this->ball_thickness;
    coordinates.y = 100;
    coordinates.w = 100;
    coordinates.h = 100;
    SDL_RenderCopy(this->renderer,text_texture,NULL,&coordinates);
    

    // wall code
    SDL_SetRenderDrawColor(this->renderer,255,255,255,255);
    SDL_Rect top_wall{0,0,1024,this->ball_thickness};
    SDL_RenderFillRect(this->renderer,&top_wall);
    SDL_Rect bottom_wall{0,768 - this->ball_thickness,1024,this->ball_thickness};
    SDL_RenderFillRect(this->renderer,&bottom_wall);

    // ball code
    SDL_SetRenderDrawColor(this->renderer,112,127,207,255);
    SDL_Rect ball{
        static_cast<int>(this->ball_pos.x - this->ball_thickness),
        static_cast<int>(this->ball_pos.y - this->ball_thickness),
        this->ball_thickness,
        this->ball_thickness
    };
    SDL_RenderFillRect(this->renderer,&ball);

    // user paddle code
    SDL_SetRenderDrawColor(this->renderer,125,176,123,255);
    SDL_Rect user_paddle{
        static_cast<int>(this->user_paddle_pos.x),
        static_cast<int>(this->user_paddle_pos.y - this->paddle_height / 2),
        this->ball_thickness,
        this->paddle_height
    };
    SDL_RenderFillRect(this->renderer,&user_paddle);

    // enemy paddle code
    SDL_Rect enemy_paddle{
        static_cast<int>(this->enemy_paddle_pos.x),
        static_cast<int>(this->enemy_paddle_pos.y - this->paddle_height / 2),
        this->ball_thickness,
        this->paddle_height
    };
    SDL_RenderFillRect(this->renderer,&enemy_paddle);
    
    // render 
    SDL_RenderPresent(this->renderer); 
}