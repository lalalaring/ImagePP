#include "DecoderTiff.h"


Image* DecoderTiff::Read(HCStream & s)
{

		//nvCheck(!s.isError());

		//TIFF * tif = TIFFOpen(fileName, "r");
		////TIFF * tif = TIFFClientOpen(fileName, "r", &s, tiffReadWriteProc, tiffReadWriteProc, tiffSeekProc, tiffCloseProc, tiffSizeProc, tiffMapFileProc, tiffUnmapFileProc);

		//if (!tif)
		//{
		//	nvDebug("Can't open '%s' for reading\n", fileName);
		//	return NULL;
		//}

		//::uint16 spp, bpp, format;
		//::uint32 width, height;
		//TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		//TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		//TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
		//TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
		//TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &format);

		//if (bpp != 8 && bpp != 16 && bpp != 32) {
		//	nvDebug("Can't load '%s', only 1 sample per pixel supported\n", fileName);
		//	TIFFClose(tif);
		//	return NULL;
		//}

		//AutoPtr<FloatImage> fimage(new FloatImage());
		//fimage->allocate(spp, width, height);

		//int linesize = TIFFScanlineSize(tif);
		//tdata_t buf = malloc<uint8>(linesize);

		//for (uint y = 0; y < height; y++)
		//{
		//	TIFFReadScanline(tif, buf, y, 0);

		//	for (uint c = 0; c<spp; c++)
		//	{
		//		float * dst = fimage->scanline(y, c);

		//		for (uint x = 0; x < width; x++)
		//		{
		//			if (bpp == 8)
		//			{
		//				dst[x] = float(((::uint8 *)buf)[x*spp + c]) / float(0xFF);
		//			}
		//			else if (bpp == 16)
		//			{
		//				dst[x] = float(((::uint16 *)buf)[x*spp + c]) / float(0xFFFF);
		//			}
		//			else if (bpp == 32)
		//			{
		//				if (format == SAMPLEFORMAT_IEEEFP)
		//				{
		//					dst[x] = float(((float *)buf)[x*spp + c]);
		//				}
		//				else
		//				{
		//					dst[x] = float(((::uint32 *)buf)[x*spp + c] >> 8) / float(0xFFFFFF);
		//				}
		//			}
		//		}
		//	}
		//}

		//free(buf);

		//TIFFClose(tif);

		//return fimage.release();

}