#ifndef FLAPPYBIRD_GAME_H
#define FLAPPYBIRD_GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

class Game {
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
    enum GameState {
        WAITING,
        PLAYING,
        GAME_OVER
    };

    struct Pipe {
        SDL_Rect rect;
        bool scored;
    };

    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int BIRD_WIDTH = 40;
    static const int BIRD_HEIGHT = 40;
    static constexpr float INITIAL_GRAVITY = 0.12f;
    static constexpr float INITIAL_JUMP_FORCE = -4.0f;
    static const int FONT_SIZE_LARGE = 36;
    static const int FONT_SIZE_MEDIUM = 28;
    static const int FONT_SIZE_SMALL = 24;

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
    Uint32 startTime;
    float gameSpeed;
    float gravity;
    float jumpForce;
    float accelerationFactor;

    SDL_Texture* loadTexture(const std::string& path);
    void jump();
    void createPipe();
    void updatePipes();
    bool checkCollision();
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void updateDifficulty();
    void resetGame();
    void renderRoundedRect(int x, int y, int w, int h, int radius);
};

#endif // FLAPPYBIRD_GAME_H