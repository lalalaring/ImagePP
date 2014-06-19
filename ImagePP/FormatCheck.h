#pragma once
enum class FileFormatType
{
	FILEFORMAT_JPG,
	FILEFORMAT_PNG,
	FILEFORMAT_TIFF,
	FILEFORMAT_TGA,
	FILEFORMAT_BMP,

};

class FormatCheck
{
public:
	FormatCheck();
	~FormatCheck();

	FileFormatType CheckFormat();
};

