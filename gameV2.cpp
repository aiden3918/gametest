#define OLC_PGE_APPLICATION
#include "headers/extern-lib/olcPixelGameEngine.h"  

#define OLC_SOUNDWAVE
#include "headers/extern-lib/olcSoundWaveEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "headers/extern-lib/olcPGEX_MiniAudio.h"

#include "headers/h/collision.h"
#include "headers/h/environment.h"
#include "headers/h/gameObject.h"
#include "headers/h/player.h"
#include "headers/h/tile.h"
#include "headers/h/util.h"
#include "headers/h/entity.h"
#include "headers/h/background.h"
#include <random>

class Game : public olc::PixelGameEngine {
public:
    Game()
    {
        sAppName = "game test";
    }

public:

    bool OnUserCreate() override
    {
        srand(time(NULL));

        const std::string worldDataRef = "data/worlddata.txt";
        _worldEnvironment = new Environment(worldDataRef);

        const std::string playerSpriteRef = "assets/sprites/player.png";
        _mainPlayer = new Player(_worldEnvironment->getSpawnPoint(), { 0, 0 }, { 0, 0 }, playerSpriteRef, screenSize, true, true);

        const std::string testBgs[3] = { "assets/backgrounds/backbgtest.png",
            "assets/backgrounds/midbgtest.png", "assets/backgrounds/frontbgtest.png", };
        _bgHandler = new Background();
        _bgHandler->setBackground("test", testBgs[0], testBgs[1], testBgs[2]);

        _ma = new olc::MiniAudio;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        vec2D mouseInfo = { GetMouseX(), GetMouseY() };
        vec2D playerPos = _mainPlayer->pos;
        vec2D playerCenter = _mainPlayer->getCenter();

        vec2D displayOffset = _mainPlayer->getDisplayOffset();

        float globalFreezeCtr = _mainPlayer->freezeCtr;

        Clear(olc::GREY);
        SetPixelMode(olc::Pixel::MASK); // do not draw any transparent pixels
        _bgHandler->update(this, playerPos);

        _mainPlayer->update(this, _ma, fElapsedTime, _worldEnvironment, mouseInfo);
        _worldEnvironment->update(this, _ma, fElapsedTime, displayOffset, mouseInfo, 
            playerCenter, globalFreezeCtr);

        /*worldEnvironment->drawTiles(this, fElapsedTime, displayOffset);
        worldEnvironment->drawProjectiles(this, fElapsedTime, mouseInfo, displayOffset);
        worldEnvironment->drawEntities(this, fElapsedTime, mouseInfo, displayOffset, playerCenter);*/

        return true;
    }

    bool OnUserDestroy() override {
        delete _mainPlayer;
        delete _worldEnvironment;
        delete _bgHandler;
        delete _soundHandler;
        delete _ma;

        return true;
    }

    const vec2D screenSize = { 1280, 720 };

private:
    Player* _mainPlayer = nullptr;
    Environment* _worldEnvironment = nullptr;
    Background* _bgHandler = nullptr;
    SoundHandler* _soundHandler = nullptr;
    olc::MiniAudio* _ma;
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
