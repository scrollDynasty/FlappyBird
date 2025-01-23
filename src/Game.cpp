#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <algorithm>

Game::Game() :
    window(nullptr),
    renderer(nullptr),
    birdTexture(nullptr),
    backgroundTexture(nullptr),
    pipeTexture(nullptr),
    groundTexture(nullptr),
    font(nullptr),
    isRunning(false),
    birdVelocity(0),
    birdAngle(0),
    score(0),
    scrollOffset(0),
    gameState(WAITING),
    gameTime(0),
    gameSpeed(0.2f),    // Уменьшенная начальная скорость
    startTime(0),
    accelerationFactor(0.0f),
    gravity(INITIAL_GRAVITY),
    jumpForce(INITIAL_JUMP_FORCE)
{
    bird.x = SCREEN_WIDTH / 4;
    bird.y = SCREEN_HEIGHT / 2;
    bird.w = BIRD_WIDTH;
    bird.h = BIRD_HEIGHT;

    srand(static_cast<unsigned>(time(nullptr)));

    audio.backgroundMusic = nullptr;
    audio.musicEnabled = true;
    audio.soundEnabled = true;
    audio.musicVolume = 0;  // Выключаем фоновую музыку
    audio.soundVolume = MIX_MAX_VOLUME / 4;  // 25% громкости для звуковых эффектов
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    if (!initAudio()) {
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

    birdTexture = loadTexture("assets/bird.png");
    backgroundTexture = loadTexture("assets/background.png");
    pipeTexture = loadTexture("assets/pipe.png");
    groundTexture = loadTexture("assets/ground.png");

    font = TTF_OpenFont("assets/font.ttf", 28);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!birdTexture || !backgroundTexture || !pipeTexture || !groundTexture) {
        return false;
    }

    isRunning = true;
    createPipe(); // Создаем первую трубу при инициализации
    playMusic();
    return true;
}

SDL_Texture* Game::loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "Failed to load image " << path << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cout << "Failed to create texture from " << path << ": " << SDL_GetError() << std::endl;
    }
    return texture;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_SPACE:
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
                    break;
                case SDLK_m:
                    toggleMusic();
                    break;
                case SDLK_s:
                    toggleSound();
                    break;
                case SDLK_UP:
                    setMusicVolume(std::min(audio.musicVolume + 8, MIX_MAX_VOLUME));
                    break;
                case SDLK_DOWN:
                    setMusicVolume(std::max(audio.musicVolume - 8, 0));
                    break;
            }
        }
    }
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

    // Обновление физики птицы
    birdVelocity += gravity;
    birdVelocity = std::min(birdVelocity, 4.0f);
    bird.y += static_cast<int>(birdVelocity);

    if (birdVelocity < 0) {
        birdAngle = -25.0f;
    } else {
        birdAngle = std::min(birdAngle + 2.0f, 70.0f);
    }

    // Проверка столкновений со стенами
    if (bird.y < 0) {
        bird.y = 0;
        birdVelocity = 0;
    }

    // Проверка столкновения с землей
    if (bird.y + bird.h > SCREEN_HEIGHT - 100) {
        bird.y = SCREEN_HEIGHT - 100 - bird.h;
        if (gameState == PLAYING) {
            Mix_HaltChannel(-1);  // Останавливаем все текущие звуки
            playSound("hit");     // Звук удара
            SDL_Delay(100);       // Небольшая задержка
            playSound("die");     // Звук смерти
            gameState = GAME_OVER;
        }
        return;
    }

    // Обновление фона
    scrollOffset -= 1.0f;
    if (scrollOffset <= -SCREEN_WIDTH) {
        scrollOffset = 0;
    }

    // Создание первой трубы, если их нет
    if (pipes.empty() && gameState == PLAYING) {
        createPipe();
    }

    updatePipes();

    // Проверка столкновений с трубами
    if (checkCollision() && gameState == PLAYING) {
        Mix_HaltChannel(-1);  // Останавливаем все текущие звуки
        playSound("hit");     // Немедленно проигрываем звук удара
        gameState = GAME_OVER;
    }
}

void Game::createPipe() {
    Pipe topPipe, bottomPipe;

    const int pipeWidth = 60;
    const int gap = 220;
    const int minHeight = 100;
    const int maxHeight = SCREEN_HEIGHT - gap - minHeight - 100;

    int height = minHeight + (rand() % (maxHeight - minHeight));

    topPipe.rect.x = SCREEN_WIDTH;
    topPipe.rect.y = 0;
    topPipe.rect.w = pipeWidth;
    topPipe.rect.h = height;
    topPipe.scored = false;

    bottomPipe.rect.x = SCREEN_WIDTH;
    bottomPipe.rect.y = height + gap;
    bottomPipe.rect.w = pipeWidth;
    bottomPipe.rect.h = SCREEN_HEIGHT - (height + gap) - 100;
    bottomPipe.scored = false;

    pipes.push_back(topPipe);
    pipes.push_back(bottomPipe);
}

void Game::updatePipes() {
    static int framesSinceLastPipe = 0;
    framesSinceLastPipe++;

    if (framesSinceLastPipe >= 180) {
        createPipe();
        framesSinceLastPipe = 0;
    }

    for (size_t i = 0; i < pipes.size(); i += 2) {
        const int pipeSpeed = 2;
        pipes[i].rect.x -= pipeSpeed;
        pipes[i + 1].rect.x = pipes[i].rect.x;

        if (!pipes[i].scored && bird.x > pipes[i].rect.x + pipes[i].rect.w) {
            score++;
            pipes[i].scored = true;
            pipes[i + 1].scored = true;
            playSound("score");
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

void Game::render() {
    SDL_RenderClear(renderer);

    // Рендеринг фона
    SDL_Rect bgRect = {static_cast<int>(scrollOffset), 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
    SDL_Rect bgRect2 = {static_cast<int>(scrollOffset) + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect2);

    // Рендеринг труб
    for (const auto& pipe : pipes) {
        SDL_RenderCopy(renderer, pipeTexture, nullptr, &pipe.rect);
    }

    // Рендеринг птицы
    SDL_RenderCopyEx(renderer, birdTexture, nullptr, &bird, birdAngle, nullptr, SDL_FLIP_NONE);

    // Рендеринг земли
    SDL_Rect groundRect = {static_cast<int>(scrollOffset), SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
    SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect);
    SDL_Rect groundRect2 = {static_cast<int>(scrollOffset) + SCREEN_WIDTH, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
    SDL_RenderCopy(renderer, groundTexture, nullptr, &groundRect2);

    // Определение цветов для текста
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color scoreColor = {255, 223, 0, 255};
    SDL_Color menuColor = {173, 216, 230, 255};

    // Рендеринг UI
    if (gameState == WAITING) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        renderRoundedRect(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 80, 600, 160, 20);
        renderText("Press SPACE to Start", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 15, menuColor);
    }
    else if (gameState == GAME_OVER) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        renderRoundedRect(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 130, 600, 260, 20);
        renderText("Game Over!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 70, titleColor);
        renderText("Final Score: " + std::to_string(score), SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 10, scoreColor);
        renderText("Press SPACE to Try Again", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 50, menuColor);
    }

    if (gameState == PLAYING) {
        renderText("Score: " + std::to_string(score), 20, 20, scoreColor);
    }

    SDL_RenderPresent(renderer);
}

void Game::renderRoundedRect(int x, int y, int w, int h, int radius) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect rect = {x + radius, y + radius, w - 2 * radius, h - 2 * radius};
    SDL_RenderFillRect(renderer, &rect);

    SDL_Rect topRect = {x + radius, y, w - 2 * radius, radius};
    SDL_Rect bottomRect = {x + radius, y + h - radius, w - 2 * radius, radius};
    SDL_RenderFillRect(renderer, &topRect);
    SDL_RenderFillRect(renderer, &bottomRect);

    SDL_Rect leftRect = {x, y + radius, radius, h - 2 * radius};
    SDL_Rect rightRect = {x + w - radius, y + radius, radius, h - 2 * radius};
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

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::jump() {
    if (gameState == PLAYING) {
        birdVelocity = jumpForce;
        birdAngle = -25.0f;
        playSound("jump");
    }
}

bool Game::initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        return false;
    }

    Mix_AllocateChannels(8);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);  // Громкость эффектов 25%
    Mix_VolumeMusic(0);                  // Выключаем фоновую музыку

    loadAudio();
    return true;
}

void Game::loadAudio() {
    audio.backgroundMusic = Mix_LoadMUS("assets/audio/background.mp3");
    if (!audio.backgroundMusic) {
        std::cout << "Warning: Failed to load background music: " << Mix_GetError() << std::endl;
    }

    const std::vector<std::pair<std::string, std::string>> soundFiles = {
        {"jump", "assets/audio/jump.wav"},
        {"score", "assets/audio/score.wav"},
        {"hit", "assets/audio/hit.wav"},
        {"die", "assets/audio/die.wav"}
    };

    for (const auto& [name, path] : soundFiles) {
        Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
        if (!sound) {
            std::cout << "Warning: Failed to load sound effect " << name << ": " << Mix_GetError() << std::endl;
            continue;
        }
        audio.soundEffects[name] = sound;
    }

    setMusicVolume(audio.musicVolume);
    setSoundVolume(audio.soundVolume);
}

void Game::playSound(const std::string& name) {
    if (!audio.soundEnabled) return;

    auto it = audio.soundEffects.find(name);
    if (it != audio.soundEffects.end() && it->second) {
        // Используем конкретный канал для каждого типа звука
        int channel = -1;
        if (name == "hit") channel = 0;
        else if (name == "die") channel = 1;
        else if (name == "score") channel = 2;
        else if (name == "jump") channel = 3;

        Mix_PlayChannel(channel, it->second, 0);
    }
}

void Game::playMusic() {
    if (!audio.musicEnabled || !audio.backgroundMusic) return;

    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(audio.backgroundMusic, -1);
    }
}

void Game::stopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void Game::toggleMusic() {
    audio.musicEnabled = !audio.musicEnabled;
    if (audio.musicEnabled) {
        playMusic();
    } else {
        stopMusic();
    }
}

void Game::toggleSound() {
    audio.soundEnabled = !audio.soundEnabled;
}

void Game::setMusicVolume(int volume) {
    audio.musicVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    Mix_VolumeMusic(audio.musicVolume);
}

void Game::setSoundVolume(int volume) {
    audio.soundVolume = std::clamp(volume, 0, MIX_MAX_VOLUME);
    for (auto& [name, sound] : audio.soundEffects) {
        if (sound) {
            Mix_VolumeChunk(sound, audio.soundVolume);
        }
    }
}

void Game::resetGame() {
    pipes.clear();
    bird.y = SCREEN_HEIGHT / 2;
    birdVelocity = 0;
    birdAngle = 0;
    score = 0;
    gameState = WAITING;
    gameTime = 0;
    gameSpeed = 0.2f;
    gravity = INITIAL_GRAVITY;
    jumpForce = INITIAL_JUMP_FORCE;
    startTime = SDL_GetTicks();
    accelerationFactor = 0.0f;
}

void Game::clean() {
    for (auto& [name, sound] : audio.soundEffects) {
        if (sound) {
            Mix_FreeChunk(sound);
            sound = nullptr;
        }
    }
    audio.soundEffects.clear();

    if (audio.backgroundMusic) {
        Mix_FreeMusic(audio.backgroundMusic);
        audio.backgroundMusic = nullptr;
    }

    if (birdTexture) {
        SDL_DestroyTexture(birdTexture);
        birdTexture = nullptr;
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (pipeTexture) {
        SDL_DestroyTexture(pipeTexture);
        pipeTexture = nullptr;
    }
    if (groundTexture) {
        SDL_DestroyTexture(groundTexture);
        groundTexture = nullptr;
    }
    if (groundTexture) {
        SDL_DestroyTexture(groundTexture);
        groundTexture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::updateDifficulty() {
    float baseSpeed = 0.2f;             // Уменьшенная базовая скорость
    float maxSpeed = 0.6f;              // Уменьшенная максимальная скорость
    float accelerationFactor = gameTime / 180000.0f;  // Замедленное ускорение
    float speedIncrease = std::cbrt(accelerationFactor) * 0.1f;

    gameSpeed = std::min(baseSpeed + speedIncrease, maxSpeed);
    gravity = INITIAL_GRAVITY * (1.0f + (gameSpeed - baseSpeed) * 0.03f);
    jumpForce = INITIAL_JUMP_FORCE * (1.0f + (gameSpeed - baseSpeed) * 0.03f);
}

bool Game::isGameRunning() const {
    return isRunning;
}