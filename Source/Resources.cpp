#include "Resources.h"

Resources::Resources()
    : alienTexture("Assets/alien.png")
    , barrierTexture("Assets/barrier.png")
    , laserTexture("Assets/laser.png")
{
    shipTextures.reserve(3);
    shipTextures.emplace_back("Assets/ship1.png");
    shipTextures.emplace_back("Assets/ship2.png");
    shipTextures.emplace_back("Assets/ship3.png");
}
	
