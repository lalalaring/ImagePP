#include "DecoderTga.h"



struct Color555 {
	uint16 b : 5;
	uint16 g : 5;
	uint16 r : 5;
};
Image* DecoderTga::Read(HCStream & s)
{
	assert(!s.isError());
	assert(s.isLoading());

	TgaHeader tga;
	s << tga;
	s.seek(TgaHeader::Size + tga.id_length);

	// Get header info.
	bool rle = false;
	bool pal = false;
	bool rgb = false;
	bool grey = false;

	switch (tga.image_type) {
	case TGA_TYPE_RLE_INDEXED:
		rle = true;
		// no break is intended!
	case TGA_TYPE_INDEXED:
		if (tga.colormap_type != 1 || tga.colormap_size != 24 || tga.colormap_length > 256) {
			//nvDebug("*** loadTGA: Error, only 24bit paletted images are supported.\n");
			return NULL;
		}
		pal = true;
		break;

	case TGA_TYPE_RLE_RGB:
		rle = true;
		// no break is intended!
	case TGA_TYPE_RGB:
		rgb = true;
		break;

	case TGA_TYPE_RLE_GREY:
		rle = true;
		// no break is intended!
	case TGA_TYPE_GREY:
		grey = true;
		break;

	default:
		//nvDebug("*** loadTGA: Error, unsupported image type.\n");
		return NULL;
	}

	const uint pixel_size = (tga.pixel_size / 8);
	assert(pixel_size <= 4);

	const uint size = tga.width * tga.height * pixel_size;


	// Read palette
	uint8 palette[768];
	if (pal) {
		assert(tga.colormap_length <= 256);
		s.serialize(palette, 3 * tga.colormap_length);
	}

	// Decode image.
	uint8 * mem = new uint8[size];
	if (rle) {
		// Decompress image in src.
		uint8 * dst = mem;
		int num = size;

		while (num > 0) {
			// Get packet header
			uint8 c;
			s << c;

			uint count = (c & 0x7f) + 1;
			num -= count * pixel_size;

			if (c & 0x80) {
				// RLE pixels.
				uint8 pixel[4];	// uint8 pixel[pixel_size];
				s.serialize(pixel, pixel_size);
				do {
					memcpy(dst, pixel, pixel_size);
					dst += pixel_size;
				} while (--count);
			}
			else {
				// Raw pixels.
				count *= pixel_size;
				//file->Read8(dst, count);
				s.serialize(dst, count);
				dst += count;
			}
		}
	}
	else {
		s.serialize(mem, size);
	}

	// Allocate image.
	auto_ptr<Image> img(new Image());
	img->allocate(tga.width, tga.height);

	int lstep;
	Color32 * dst;
	if (tga.flags & TGA_ORIGIN_UPPER) {
		lstep = tga.width;
		dst = img->pixels();
	}
	else {
		lstep = -tga.width;
		dst = img->pixels() + (tga.height - 1) * tga.width;
	}

	// Write image.
	uint8 * src = mem;
	if (pal) {
		for (int y = 0; y < tga.height; y++) {
			for (int x = 0; x < tga.width; x++) {
				uint8 idx = *src++;
				dst[x].setBGRA(palette[3 * idx + 0], palette[3 * idx + 1], palette[3 * idx + 2], 0xFF);
			}
			dst += lstep;
		}
	}
	else if (grey) {
		img->setFormat(Image::Format_ARGB);

		for (int y = 0; y < tga.height; y++) {
			for (int x = 0; x < tga.width; x++) {
				dst[x].setBGRA(*src, *src, *src, *src);
				src++;
			}
			dst += lstep;
		}
	}
	else {

		if (tga.pixel_size == 16) {
			for (int y = 0; y < tga.height; y++) {
				for (int x = 0; x < tga.width; x++) {
					Color555 c = *reinterpret_cast<Color555 *>(src);
					uint8 b = (c.b << 3) | (c.b >> 2);
					uint8 g = (c.g << 3) | (c.g >> 2);
					uint8 r = (c.r << 3) | (c.r >> 2);
					dst[x].setBGRA(b, g, r, 0xFF);
					src += 2;
				}
				dst += lstep;
			}
		}
		else if (tga.pixel_size == 24) {
			for (int y = 0; y < tga.height; y++) {
				for (int x = 0; x < tga.width; x++) {
					dst[x].setBGRA(src[0], src[1], src[2], 0xFF);
					src += 3;
				}
				dst += lstep;
			}
		}
		else if (tga.pixel_size == 32) {
			img->setFormat(Image::Format_ARGB);

			for (int y = 0; y < tga.height; y++) {
				for (int x = 0; x < tga.width; x++) {
					dst[x].setBGRA(src[0], src[1], src[2], src[3]);
					src += 4;
				}
				dst += lstep;
			}
		}
	}

	// free uncompressed data.
	delete[] mem;

	return img.release();
}
