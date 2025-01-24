#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>

class Game {
public:
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int BIRD_WIDTH = 40;
    static const int BIRD_HEIGHT = 30;
    static constexpr float INITIAL_GRAVITY = 0.15f;
    static constexpr float INITIAL_JUMP_FORCE = -4.0f;

    enum GameState {
        WAITING,
        PLAYING,
        GAME_OVER
    };

    struct Pipe {
        SDL_Rect rect;
        bool scored;
    };

    struct AudioSystem {
        Mix_Music* backgroundMusic;
        std::map<std::string, Mix_Chunk*> soundEffects;
        bool musicEnabled;
        bool soundEnabled;
        int musicVolume;
        int soundVolume;
    };

    Game();
    ~Game();

    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    bool isGameRunning() const;  // Добавлено объявление функции с const

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* birdTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* pipeTexture;
    SDL_Texture* groundTexture;
    TTF_Font* font;

    SDL_Rect bird;
    std::vector<Pipe> pipes;

    bool isRunning;
    float birdVelocity;
    float birdAngle;
    int score;
    float scrollOffset;
    GameState gameState;
    Uint32 gameTime;
    float gameSpeed;
    Uint32 startTime;
    float accelerationFactor;
    float gravity;
    float jumpForce;

    AudioSystem audio;

    SDL_Texture* loadTexture(const std::string& path);
    void createPipe();
    void updatePipes();
    bool checkCollision();
    void renderRoundedRect(int x, int y, int w, int h, int radius);
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void jump();
    void updateDifficulty();
    bool initAudio();
    void loadAudio();
    void playSound(const std::string& name);
    void playMusic();
    void stopMusic();
    void toggleMusic();
    void toggleSound();
    void setMusicVolume(int volume);
    void setSoundVolume(int volume);
    void resetGame();
};

#endif // GAME_H