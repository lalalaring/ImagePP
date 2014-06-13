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
//class Image
//{
//public:
//	void* GetData(){ return data; }
//	int GetWidth(){ return width; }
//	int GetHeight(){ return height; }
//	int GetDepth(){ return depth; }
//
//
//protected:
//private:
//
//	int width;
//	int height;
//	int depth;
//	void* data;
//};
class ImagePP
{
public:
	ImagePP();
	~ImagePP();

	Image* CreateFromFile(std::string file);
};

