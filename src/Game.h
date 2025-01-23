#ifndef FLAPPYBIRD_GAME_H
#define FLAPPYBIRD_GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>

class Game {
private:
    // Константы
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int BIRD_WIDTH = 40;
    static const int BIRD_HEIGHT = 40;
    const float INITIAL_GRAVITY = 0.15f;        // Уменьшенная гравитация
    const float INITIAL_JUMP_FORCE = -4.5f;     // Уменьшенная сила прыжка

    // Состояния игры
    enum GameState {
        WAITING,
        PLAYING,
        GAME_OVER
    };

    // Структура для труб
    struct Pipe {
        SDL_Rect rect;
        bool scored;
    };

    // Структура для аудио
    struct AudioSystem {
        Mix_Music* backgroundMusic;
        std::map<std::string, Mix_Chunk*> soundEffects;
        bool musicEnabled;
        bool soundEnabled;
        int musicVolume;
        int soundVolume;
    };

    // SDL компоненты
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* birdTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* pipeTexture;
    SDL_Texture* groundTexture;
    TTF_Font* font;

    // Игровые объекты и состояния
    SDL_Rect bird;
    std::vector<Pipe> pipes;
    AudioSystem audio;
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

public:
    Game();
    ~Game();

    bool init();
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running() const { return isRunning; }

private:
    SDL_Texture* loadTexture(const std::string& path);
    void createPipe();
    void updatePipes();
    bool checkCollision();
    void jump();
    void updateDifficulty();
    void renderRoundedRect(int x, int y, int w, int h, int radius);
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void resetGame();

    // Аудио функции
    bool initAudio();
    void loadAudio();
    void playSound(const std::string& name);
    void playMusic();
    void stopMusic();
    void toggleMusic();
    void toggleSound();
    void setMusicVolume(int volume);
    void setSoundVolume(int volume);
};

#endif // FLAPPYBIRD_GAME_H