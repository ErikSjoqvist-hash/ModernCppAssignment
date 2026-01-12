#pragma once
#include <vector>
#include "TextureManager.h"

class Resources
{ 
public:
	Resources();


	std::vector<TextureManager> shipTextures;
	const TextureManager alienTexture;
	const TextureManager barrierTexture;
	const TextureManager laserTexture;
};