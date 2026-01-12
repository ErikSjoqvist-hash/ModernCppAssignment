#pragma once
#include <vector>
#include "TextureManager.h"

class Resources
{ 
public:
	Resources();


	std::vector<TextureManager> shipTextures;
	TextureManager alienTexture;
	TextureManager barrierTexture;
	TextureManager laserTexture;
};