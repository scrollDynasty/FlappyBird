#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

Game::Game() : window(nullptr), renderer(nullptr),
    birdTexture(nullptr), backgroundTexture(nullptr),
    pipeTexture(nullptr), groundTexture(nullptr),
    font(nullptr),
    isRunning(false), birdVelocity(0), birdAngle(0),
    score(0), scrollOffset(0), gameState(WAITING),
    gameTime(0), gameSpeed(0.6f), startTime(0), accelerationFactor(0.0f) {

    bird.x = SCREEN_WIDTH / 4;
    bird.y = SCREEN_HEIGHT / 2;
    bird.w = BIRD_WIDTH;
    bird.h = BIRD_HEIGHT;

    gravity = INITIAL_GRAVITY;
    jumpForce = INITIAL_JUMP_FORCE;

    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {
    clean();
}

SDL_Texture* Game::loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cout << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
    return texture;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() < 0) {
        std::cout << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("assets/font.ttf", FONT_SIZE_MEDIUM);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }

    birdTexture = loadTexture("assets/bird.png");
    backgroundTexture = loadTexture("assets/background.png");
    pipeTexture = loadTexture("assets/pipe.png");
    groundTexture = loadTexture("assets/ground.png");

    if (!birdTexture || !backgroundTexture || !pipeTexture || !groundTexture) {
        return false;
    }

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                if (gameState == WAITING) {
                    gameState = PLAYING;
                    startTime = SDL_GetTicks();
                    gameTime = 0;
                    birdVelocity = 0;
                    jump();
                }
                else if (gameState == PLAYING) {
                    jump();
                }
                else if (gameState == GAME_OVER) {
                    resetGame();
                }
            }
        }
    }
}

void Game::jump() {
    birdVelocity = jumpForce;
    birdAngle = -25.0f;
}

void Game::update() {
    if (gameState == WAITING) {
        bird.y = SCREEN_HEIGHT / 2 + sin(SDL_GetTicks() / 500.0) * 30;
        scrollOffset -= 0.5;
        if (scrollOffset <= -SCREEN_WIDTH) {
            scrollOffset = 0;
        }
        return;
    }

    if (gameState != PLAYING) {
        return;
    }

    gameTime = SDL_GetTicks() - startTime;
    updateDifficulty();

    birdVelocity += gravity;
    if (birdVelocity > 4.5f) {
        birdVelocity = 4.5f;
    }
    bird.y += static_cast<int>(birdVelocity);

    if (birdVelocity < 0) {
        birdAngle = -25.0f;
    } else {
        birdAngle += 0.8f;
        if (birdAngle > 70.0f) {
            birdAngle = 70.0f;
        }
    }

    if (bird.y < 0) {
        bird.y = 0;
        birdVelocity = 0;
    }
    if (bird.y + bird.h > SCREEN_HEIGHT - 100) {
        bird.y = SCREEN_HEIGHT - 100 - bird.h;
        gameState = GAME_OVER;
    }

    scrollOffset -= 1.5f * gameSpeed;
    if (scrollOffset <= -SCREEN_WIDTH) {
        scrollOffset = 0;
    }

    updatePipes();
    if (checkCollision()) {
        gameState = GAME_OVER;
    }
}

void Game::createPipe() {
    Pipe topPipe, bottomPipe;

    int gap = std::max(200 - static_cast<int>((gameSpeed - 0.6f) * 30), 140);
    int minHeight = 50;
    int maxHeight = SCREEN_HEIGHT - gap - minHeight - 100;
    int height = rand() % (maxHeight - minHeight) + minHeight;

    int pipeSpacing = std::max(300 - static_cast<int>((gameSpeed - 0.6f) * 50), 180);

    topPipe.rect = {
        SCREEN_WIDTH + (pipes.empty() ? 0 : pipeSpacing),
        0,
        60,
        height
    };
    topPipe.scored = false;

    bottomPipe.rect = {
        SCREEN_WIDTH + (pipes.empty() ? 0 : pipeSpacing),
        height + gap,
        60,
        SCREEN_HEIGHT - (height + gap) - 100
    };
    bottomPipe.scored = false;

    pipes.push_back(topPipe);
    pipes.push_back(bottomPipe);
}

void Game::updatePipes() {
    static int frameCount = 0;
    frameCount++;

    int pipeCreationInterval = static_cast<int>(70 / gameSpeed);
    if (frameCount % pipeCreationInterval == 0) {
        createPipe();
    }

    for (size_t i = 0; i < pipes.size(); i += 2) {
        int pipeSpeed = static_cast<int>(4 * gameSpeed);
        pipes[i].rect.x -= pipeSpeed;
        pipes[i + 1].rect.x = pipes[i].rect.x;

        if (!pipes[i].scored && bird.x > pipes[i].rect.x + pipes[i].rect.w) {
            score++;
            pipes[i].scored = true;
            pipes[i + 1].scored = true;
        }
    }

    while (!pipes.empty() && pipes[0].rect.x + pipes[0].rect.w < 0) {
        pipes.erase(pipes.begin(), pipes.begin() + 2);
    }
}

bool Game::checkCollision() {
    for (const auto& pipe : pipes) {
        if (SDL_HasIntersection(&bird, &pipe.rect)) {
            return true;
        }
    }
    return false;
}

void Game::updateDifficulty() {
    float baseSpeed = 0.6f;
    float maxSpeed = 2.0f;
    float accelerationFactor = gameTime / 60000.0f;
    float speedIncrease = std::cbrt(accelerationFactor) * 0.3f;

    gameSpeed = baseSpeed + speedIncrease;

    if (gameSpeed > maxSpeed) {
        gameSpeed = maxSpeed;
    }

    gravity = INITIAL_GRAVITY * (1.0f + (gameSpeed - baseSpeed) * 0.05f);
    jumpForce = INITIAL_JUMP_FORCE * (1.0f + (gameSpeed - baseSpeed) * 0.05f);
}

void Game::resetGame() {
    pipes.clear();
    bird.y = SCREEN_HEIGHT / 2;
    birdVelocity = 0;
    birdAngle = 0;
    score = 0;
    gameState = WAITING;
    gameTime = 0;
    gameSpeed = 0.6f;
    gravity = INITIAL_GRAVITY;
    jumpForce = INITIAL_JUMP_FORCE;
    startTime = SDL_GetTicks();
    accelerationFactor = 0.0f;
}

void Game::renderRoundedRect(int x, int y, int w, int h, int radius) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);

    SDL_Rect rect = { x + radius, y + radius, w - 2 * radius, h - 2 * radius };
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect topRect = { x + radius, y, w - 2 * radius, radius };
    SDL_Rect bottomRect = { x + radius, y + h - radius, w - 2 * radius, radius };
    SDL_RenderFillRect(renderer, &topRect);
    SDL_RenderFillRect(renderer, &bottomRect);

    SDL_Rect leftRect = { x, y + radius, radius, h - 2 * radius };
    SDL_Rect rightRect = { x + w - radius, y + radius, radius, h - 2 * radius };
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_RenderFillRect(renderer, &rightRect);

    for(int i = 0; i <= radius; i++) {
        for(int j = 0; j <= radius; j++) {
            float distance = sqrt((float)(i * i + j * j));
            if(distance <= radius) {
                SDL_RenderDrawPoint(renderer, x + radius - i, y + radius - j);
                SDL_RenderDrawPoint(renderer, x + w - radius + i - 1, y + radius - j);
                SDL_RenderDrawPoint(renderer, x + radius - i, y + h - radius + j - 1);
                SDL_RenderDrawPoint(renderer, x + w - radius + i - 1, y + h - radius + j - 1);
            }
        }
    }
}

void Game::renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_Rect rect = { x, y, textWidth, textHeight };

    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::render() {
    SDL_RenderClear(renderer);

    SDL_Rect bgRect = {static_cast<int>(scrollOffset), 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
    SDL_Rect bgRect2 = {static_cast<int>(scrollOffset) + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect2);

    for (const auto& pipe : pipes) {
        SDL_RenderCopy(renderer, pipeTexture, nullptr, &pipe.rect);
    }

    SDL_RenderCopyEx(renderer, birdTexture, nullptr, &bird, birdAngle, nullptr, SDL_FLIP_NONE);

    SDL_Rect groundRect = {static_cast<int>(scrollOffset), SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
    SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect);
    SDL_Rect groundRect2 = {static_cast<int>(scrollOffset) + SCREEN_WIDTH, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
    SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect2);

    SDL_Color titleColor = { 255, 255, 255, 255 };
    SDL_Color scoreColor = { 255, 223, 0, 255 };
    SDL_Color menuColor = { 173, 216, 230, 255 };

    if (gameState == WAITING) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        int boxWidth = 600;
        int boxHeight = 160;
        int boxX = SCREEN_WIDTH / 2 - boxWidth / 2;
        int boxY = SCREEN_HEIGHT / 2 - boxHeight / 2;

        renderRoundedRect(boxX, boxY, boxWidth, boxHeight, 20);
        renderText("Press SPACE to Start", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 15, menuColor);
    }
    else if (gameState == GAME_OVER) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        int boxWidth = 600;
        int boxHeight = 260;
        int boxX = SCREEN_WIDTH / 2 - boxWidth / 2;
        int boxY = SCREEN_HEIGHT / 2 - boxHeight / 2;

        renderRoundedRect(boxX, boxY, boxWidth, boxHeight, 20);
        renderText("Game Over!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 70, titleColor);
        renderText("Final Score: " + std::to_string(score), SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 10, scoreColor);
        renderText("Press SPACE to Try Again", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 50, menuColor);
    }

    if (gameState == PLAYING) {
        renderText("Score: " + std::to_string(score), 20, 20, scoreColor);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TTF_CloseFont(font);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}