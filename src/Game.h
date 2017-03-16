// Game.h

#ifndef GAME_H
#define GAME_H

#include <memory>


namespace Platformer
{


class Game
{
public:
    Game();
    Game(Game&& other);
    virtual Game& operator=(Game&& other);
    virtual ~Game();

private:
    struct Impl;
    std::unique_ptr<Impl> _pimpl;
};


}  // namespace Platformer

#endif  // GAME_H
