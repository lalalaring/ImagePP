#pragma once

#include <string>
#include <memory>
#include "Image.h"
#include "TgaHeader.h"
class DecoderTga
{
public:
	static Image* Read(HCStream & s);

private:
	
};

