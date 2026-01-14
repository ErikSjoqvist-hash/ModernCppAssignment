#pragma once
#include <vector>
#include "TextureManager.h"

struct Resources 
{ 
	Resources();


	std::vector<TextureManager> shipTextures;
	const TextureManager alienTexture;
	const TextureManager barrierTexture;
	const TextureManager laserTexture;
};