#include "DecoderPng.h"
#include "png.h"
#include <memory>
static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	assert(png_ptr != NULL);

	HCStream * s = (HCStream *)png_get_io_ptr(png_ptr);
	s->serialize(data, (int)length);

	if (s->isError()) {
		png_error(png_ptr, "Read Error");
	}
}
Image* DecoderPng::Read(HCStream & s)
{
assert(!s.isError());

// Set up a read buffer and check the library version
png_structp png_ptr;
png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
if (png_ptr == NULL) {
	//	nvDebug( "*** LoadPNG: Error allocating read buffer in file '%s'.\n", name );
	return false;
}

// Allocate/initialize a memory block for the image information
png_infop info_ptr = png_create_info_struct(png_ptr);
if (info_ptr == NULL) {
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	//	nvDebug( "*** LoadPNG: Error allocating image information for '%s'.\n", name );
	return false;
}

// Set up the error handling
if (setjmp(png_jmpbuf(png_ptr))) {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	//	nvDebug( "*** LoadPNG: Error reading png file '%s'.\n", name );
	return false;
}

// Set up the I/O functions.
png_set_read_fn(png_ptr, (void*)&s, user_read_data);


// Retrieve the image header information
png_uint_32 width, height;
int bit_depth, color_type, interlace_type;
png_read_info(png_ptr, info_ptr);
png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);


if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) {
	// Convert indexed images to RGB.
	png_set_expand(png_ptr);
}
else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
	// Convert grayscale to RGB.
	png_set_expand(png_ptr);
}
else if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	// Expand images with transparency to full alpha channels
	// so the data will be available as RGBA quartets.
	png_set_expand(png_ptr);
}
else if (bit_depth < 8) {
	// If we have < 8 scale it up to 8.
	//png_set_expand(png_ptr);
	png_set_packing(png_ptr);
}

// Reduce bit depth.
if (bit_depth == 16) {
	png_set_strip_16(png_ptr);
}

// Represent gray as RGB
if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
	png_set_gray_to_rgb(png_ptr);
}

// Convert to RGBA filling alpha with 0xFF.
if (!(color_type & PNG_COLOR_MASK_ALPHA)) {
	png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
}

// @todo Choose gamma according to the platform?
double screen_gamma = 2.2;
int intent;
if (png_get_sRGB(png_ptr, info_ptr, &intent)) {
	png_set_gamma(png_ptr, screen_gamma, 0.45455);
}
else {
	double image_gamma;
	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma)) {
		png_set_gamma(png_ptr, screen_gamma, image_gamma);
	}
	else {
		png_set_gamma(png_ptr, screen_gamma, 0.45455);
	}
}

// Perform the selected transforms.
png_read_update_info(png_ptr, info_ptr);

png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

unique_ptr<Image> img(new Image());
img->allocate(width, height);

// Set internal format flags.
if (color_type & PNG_COLOR_MASK_COLOR) {
	//img->flags |= PI_IF_HAS_COLOR;
}
if (color_type & PNG_COLOR_MASK_ALPHA) {
	//img->flags |= PI_IF_HAS_ALPHA;
	img->setFormat(Image::Format_ARGB);
}

// Read the image
uint8 * pixels = (uint8 *)img->pixels();
png_bytep * row_data = new png_bytep[sizeof(png_byte) * height];
for (uint i = 0; i < height; i++) {
	row_data[i] = &(pixels[width * 4 * i]);
}

png_read_image(png_ptr, row_data);
delete[] row_data;

// Finish things up
png_read_end(png_ptr, info_ptr);
png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

// RGBA to BGRA.
uint num = width * height;
for (uint i = 0; i < num; i++)
{
	Color32 c = img->pixel(i);
	img->pixel(i) = Color32(c.b, c.g, c.r, c.a);
}

// Compute alpha channel if needed.
/*if( img->flags & PI_IU_BUMPMAP || img->flags & PI_IU_ALPHAMAP ) {
if( img->flags & PI_IF_HAS_COLOR && !(img->flags & PI_IF_HAS_ALPHA)) {
img->ComputeAlphaFromColor();
}
}*/

return img.release();
};

