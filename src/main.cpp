#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init()) {
        return 1;
    }

    while (game.isGameRunning()) {
        game.handleEvents();
        game.update();
        game.render();
    }

    game.clean();
    return 0;
}