#pragma once
#include <vector>
#include "TextureManager.h"

struct Resources 
{ 
	Resources();


	std::vector<TextureManager> shipTextures;
	TextureManager alienTexture;
	TextureManager barrierTexture;
	TextureManager laserTexture;
};