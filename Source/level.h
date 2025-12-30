#pragma once 
//TODO: unecesary includes
#include <iostream>
#include <string>
#include <fstream>

//TODO: unecesary file?
struct Entity
{
	float x, y;

	void LoadLevelFromAFile(const std::string& filename);
};