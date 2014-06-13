#pragma once
#include <string>
#include "Image.h"
enum IMAGEPIEXLFORMAT
{
	FORMAT_R8G8B8,
	FORMAT_B8G8R8,
	FORMAT_R8G8B8A,
	FORMAT_R8G8B8A8,
	FORMAT_B8G8R8A,
	FORMAT_B8G8R8A8,
};

class ImagePP
{
public:
	ImagePP();
	~ImagePP();

	Image* CreateFromFile(std::string file);
};

