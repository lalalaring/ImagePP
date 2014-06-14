#pragma once
#include <string>
#include <memory>
#include "Image.h"
class DecoderTiff
{
public:
	static Image* Read(HCStream & s);
};

