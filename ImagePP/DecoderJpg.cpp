#include "DecoderJpg.h"
#include "jpeglib.h"
#include <memory>
#include <vector>
#include <array>
static void init_source(j_decompress_ptr /*cinfo*/){
}

static boolean fill_input_buffer(j_decompress_ptr cinfo) {
	struct jpeg_source_mgr * src = cinfo->src;
	static JOCTET FakeEOI[] = { 0xFF, JPEG_EOI };



	// Insert a fake EOI marker
	src->next_input_byte = FakeEOI;
	src->bytes_in_buffer = 2;

	return TRUE;
}

static void skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
	struct jpeg_source_mgr * src = cinfo->src;

	if (num_bytes >= (long)src->bytes_in_buffer) {
		fill_input_buffer(cinfo);
		return;
	}

	src->bytes_in_buffer -= num_bytes;
	src->next_input_byte += num_bytes;
}

static void term_source(j_decompress_ptr /*cinfo*/){
	// no work necessary here
}


Image* DecoderJpg::Read(HCStream & s)
{
	char buf[2] = { 0 };
	s.serialize(buf, 2);
	if (buf[0] != '\xFF'||buf[1] != '\xD8')
		return nullptr;
	s.seek(0);
	unsigned char * buftemp = new unsigned char[s.size()];
	s.serialize(buftemp, s.size());

	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	cinfo.src = (struct jpeg_source_mgr *) (*cinfo.mem->alloc_small)
		((j_common_ptr)&cinfo, JPOOL_PERMANENT, sizeof(struct jpeg_source_mgr));
	cinfo.src->init_source = init_source;
	cinfo.src->fill_input_buffer = fill_input_buffer;
	cinfo.src->skip_input_data = skip_input_data;
	cinfo.src->resync_to_restart = jpeg_resync_to_restart;	// use default method
	cinfo.src->term_source = term_source;
	cinfo.src->bytes_in_buffer = s.size();
	cinfo.src->next_input_byte = buftemp;// (const JOCTET *)&arr[0];// byte_array.buffer();

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	/*
	cinfo.do_fancy_upsampling = FALSE;	// fast decompression
	cinfo.dct_method = JDCT_FLOAT;			// Choose floating point DCT method.
	*/

	uint8 * tmp_buffer = new uint8[cinfo.output_width * cinfo.output_height * cinfo.num_components];
	uint8 * scanline = tmp_buffer;

	while (cinfo.output_scanline < cinfo.output_height)
	{
		int num_scanlines = jpeg_read_scanlines(&cinfo, &scanline, 1);
		scanline += num_scanlines * cinfo.output_width * cinfo.num_components;
	}

	jpeg_finish_decompress(&cinfo);

	unique_ptr<Image>  img(new Image());
	img->allocate(cinfo.output_width, cinfo.output_height);

	Color32 * dst = img->pixels();
	const int size = img->height() * img->width();
	const uint8 * src = tmp_buffer;

	if (cinfo.num_components == 3)
	{
		img->setFormat(Image::Format_RGB);
		for (int i = 0; i < size; i++) {
			*dst++ = Color32(src[0], src[1], src[2]);
			src += 3;
		}
	}
	else
	{
		img->setFormat(Image::Format_ARGB);
		for (int i = 0; i < size; i++)
		{
			*dst++ = Color32(*src, *src, *src, *src);
			src++;
		}
	}
	delete[] tmp_buffer;
	delete[] buftemp;
	jpeg_destroy_decompress(&cinfo);

	return img.release();
}