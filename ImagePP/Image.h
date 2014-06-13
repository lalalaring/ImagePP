#pragma  once
#include "HCStream.h"
#include "ImagePPconfig.h"

class  Color32
{
public:
	Color32() { }
	Color32(const Color32 & c) : u(c.u) { }
	Color32(uint8 R, uint8 G, uint8 B) { setRGBA(R, G, B, 0xFF); }
	Color32(uint8 R, uint8 G, uint8 B, uint8 A) { setRGBA(R, G, B, A); }

	explicit Color32(uint32 U) : u(U) { }

	void setRGBA(uint8 R, uint8 G, uint8 B, uint8 A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	}

	void setBGRA(uint8 B, uint8 G, uint8 R, uint8 A = 0xFF)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	}

	operator uint32 () const {
		return u;
	}

	union {
		struct {
#if PP_LITTLE_ENDIAN
			uint8 b, g, r, a;
#else
			uint8 a : 8;
			uint8 r : 8;
			uint8 g : 8;
			uint8 b : 8;
#endif
		};
		uint32 u;
	};
};


class Image
{
public:

	enum Format
	{
		Format_RGB,
		Format_ARGB,
	};

	Image();
	Image(const Image & img);
	~Image();

	const Image & operator=(const Image & img);


	void allocate(uint w, uint h, uint d = 1);
	bool load(const char * name);

	void wrap(void * data, uint w, uint h, uint d = 1);
	void unwrap();

	uint width() const;
	uint height() const;
	uint depth() const;

	const Color32 * scanline(uint h) const;
	Color32 * scanline(uint h);

	const Color32 * pixels() const;
	Color32 * pixels();

	const Color32 & pixel(uint idx) const;
	Color32 & pixel(uint idx);

	const Color32 & pixel(uint x, uint y, uint z = 0) const;
	Color32 & pixel(uint x, uint y, uint z = 0);

	Format format() const;
	void setFormat(Format f);

	void fill(Color32 c);

	void* GetData(){ return m_data; }
	//void* GetData(int mip){ return data; }
	uint GetWidth(){ return m_width; }
	uint GetHeight(){ return m_height; }
	uint GetDepth(){ return m_depth; }

private:
	void free();

private:
	uint m_width;
	uint m_height;
	uint m_depth;
	Format m_format;
	Color32 * m_data;
};
