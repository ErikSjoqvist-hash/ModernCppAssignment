#pragma once
#include <vector>
#include "TextureManager.h"

class Resources // TODO: struct?
{ 
public:
	Resources();


	std::vector<TextureManager> shipTextures;
	const TextureManager alienTexture;
	const TextureManager barrierTexture;
	const TextureManager laserTexture;
};