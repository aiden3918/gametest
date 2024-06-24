#define OLC_PGE_APPLICATION

#include "headers/extern-lib/olcPixelGameEngine.h"  
#include "headers/h/collision.h"
#include "headers/h/environment.h"
#include "headers/h/gameObject.h"
#include "headers/h/player.h"
#include "headers/h/tile.h"
#include "headers/h/util.h"

class Game : public olc::PixelGameEngine {
public:
    Game()
    {
        sAppName = "game test";
    }

public:

    bool OnUserCreate() override
    {
        std::string playerSpriteRef = "assets/sprites/sprite-temp.png";
        mainPlayer = new Player({ 500, 300 }, { 0, 0 }, { 0, 0 }, playerSpriteRef, true, true);

        worldEnvironment = new Environment();
        Tile floor = Tile({ 0, 600 }, { 1280, 120 });
        Tile ceiling = Tile({ 0, 0 }, { 1280, 50 });
        Tile wall = Tile({ 100, 300 }, { 50, 200 });
        Tile wall2 = Tile({ 500, 300 }, { 50, 200 });
        worldEnvironment->addTile(floor);
        worldEnvironment->addTile(ceiling);
        worldEnvironment->addTile(wall);
        worldEnvironment->addTile(wall2);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::GREY);
        vec2D mouseInfo = { GetMouseX(), GetMouseY() };

        worldEnvironment->drawTiles(this, fElapsedTime);
        mainPlayer->update(this, fElapsedTime, worldEnvironment, mouseInfo);
        worldEnvironment->drawProjectiles(this, fElapsedTime, mouseInfo);

        return true;
    }

    bool OnUserDestroy() override {
        return true;
    }

    vec2D screenSize = { 1280, 720 };

private:
    Player* mainPlayer;
    Environment* worldEnvironment = nullptr;
};

int main(int* argc, char** argv)
{
    Game game;

    if (game.Construct((int)game.screenSize.x, (int)game.screenSize.y, 1, 1)) {
        game.Start();
    }

    return 0;
}
