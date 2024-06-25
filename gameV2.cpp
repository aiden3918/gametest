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
        Tile wall2 = Tile({ 700, 400 }, { 200, 50 });
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

//License(OLC - 3)
//Copyright 2018 - 2024 OneLoneCoder.com
//
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met :
//
//Redistributions or derivations of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
//Redistributions or derivative works in binary form must reproduce the above copyright notice.This list of conditions and the following disclaimer must be reproduced in the documentation and /or other materials provided with the distribution.
//
//Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN 
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
