// Visualizer.cpp

#include "Visualizer.h"


namespace Platformer
{


struct Visualizer::Impl
{
    Impl()
    {
    }
};



Visualizer::Visualizer()
    : _pimpl(new Impl())
{
}

Visualizer::Visualizer(Visualizer&& /*other*/) = default;
Visualizer& Visualizer::operator=(Visualizer&& /*other*/) = default;
Visualizer::~Visualizer() = default;


void Visualizer::refresh()
{
}


}  // namespace Platformer
