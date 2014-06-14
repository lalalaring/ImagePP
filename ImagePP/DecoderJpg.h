#pragma once
#include "Image.h"
class DecoderJpg
{
public:
	static Image* Read(HCStream & s);
};

