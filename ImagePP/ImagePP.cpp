#include "ImagePP.h"


ImagePP::ImagePP()
{
}


ImagePP::~ImagePP()
{
}

Image* ImagePP::CreateFromFile(std::string file)
{
	Image* im = new Image();
	im->load(file.c_str());
	return im;
}