
#include "Image.h"
#include "DecoderTga.h"
#include "DecoderPng.h"
#include "HCStdStream.h"


#include <string.h> 
#include <memory>

using namespace std;

Image::Image() : m_width(0), m_height(0), m_format(Format_RGB), m_data(NULL)
{
}

Image::Image(const Image & img) : m_data(NULL)
{
	allocate(img.m_width, img.m_height, img.m_depth);
	m_format = img.m_format;
	memcpy(m_data, img.m_data, sizeof(Color32) * m_width * m_height * m_depth);
}

Image::~Image()
{
	free();
}

const Image & Image::operator=(const Image & img)
{
	allocate(img.m_width, img.m_height, m_depth);
	m_format = img.m_format;
	memcpy(m_data, img.m_data, sizeof(Color32) * m_width * m_height * m_depth);
	return *this;
}


void Image::allocate(uint w, uint h, uint d)
{
	free();
	m_width = w;
	m_height = h;
	m_depth = d;
	m_data = (Color32*)realloc(m_data, sizeof(Color32) *w * h * d);
}

bool Image::load(const char * name)
{
	free();

	shared_ptr<Image> img(DecoderPng::Read(HCStdInputStream(name)));
	if (img.get() == NULL)
	{
	
		img.reset(DecoderTga::Read(HCStdInputStream(name)));
	}


	if (img.get() == NULL)
	{
		return false;
	}

	swap(m_width, img->m_width);
	swap(m_height, img->m_height);
	swap(m_depth, img->m_depth);
	swap(m_format, img->m_format);
	swap(m_data, img->m_data);

	return true;
}

void Image::wrap(void * data, uint w, uint h, uint d)
{
	free();
	m_data = (Color32 *)data;
	m_width = w;
	m_height = h;
	m_depth = d;
}

void Image::unwrap()
{
	m_data = NULL;
	m_width = 0;
	m_height = 0;
	m_depth = 0;
}


void Image::free()
{
	::free(m_data);
	m_data = NULL;
}


uint Image::width() const
{
	return m_width;
}

uint Image::height() const
{
	return m_height;
}

uint Image::depth() const
{
	return m_depth;
}

const Color32 * Image::scanline(uint h) const
{
	return m_data + h * m_width;
}

Color32 * Image::scanline(uint h)
{
	return m_data + h * m_width;
}

const Color32 * Image::pixels() const
{
	return m_data;
}

Color32 * Image::pixels()
{
	return m_data;
}

const Color32 & Image::pixel(uint idx) const
{
	return m_data[idx];
}

Color32 & Image::pixel(uint idx)
{
	return m_data[idx];
}
const Color32 & Image::pixel(uint x, uint y, uint z) const
{
	return m_data[x*m_width+y];
}

Color32 & Image::pixel(uint x, uint y, uint z)
{
	return m_data[x*m_width + y];
}

Image::Format Image::format() const
{
	return m_format;
}

void Image::setFormat(Image::Format f)
{
	m_format = f;
}

void Image::fill(Color32 c)
{
	const uint size = m_width * m_height * m_depth;
	for (uint i = 0; i < size; ++i)
	{
		m_data[i] = c;
	}
}

