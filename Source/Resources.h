#pragma once
#include <vector>
#include "TextureManager.h"

struct Resources // TODO: struct?
{ 
	Resources();


	std::vector<TextureManager> shipTextures;
	const TextureManager alienTexture;
	const TextureManager barrierTexture;
	const TextureManager laserTexture;
};