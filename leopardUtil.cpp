#include "leopardUtil.h"
#include "raw2bmp.c"

// create gamma table
static void initGammaTable(double gamma, int bpp)
{
	int result;
	double dMax;
	int iMax;
	unsigned short addr, value;

	if (bpp > 12)
		return;

	dMax = pow(2, (double)bpp);
	iMax = (int)dMax;

	for (int i = 0; i < iMax; i++) {
		result = (int)(pow((double)i / dMax, 1.0 / gamma)*dMax);

		linear_to_gamma[i] = result;
	}

	gammaValue = gamma;
	gBPP = bpp;

}


static void gammaCorrection(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, double gamma)
{
	int i;
	WORD *srcShort;
	WORD *dstShort;

	if (gamma != gammaValue || gBPP != bpp)
		initGammaTable(gamma, bpp);

	if (bpp > 8)
	{
		srcShort = (WORD *)(in_bytes);
		dstShort = (WORD *)(out_bytes);
		for (i = 0; i<width*height; i++)
			*dstShort++ = linear_to_gamma[*srcShort++];
	}
	else
	{
		for (i = 0; i<width*height; i++)
			*out_bytes++ = linear_to_gamma[*in_bytes++];
	}

}

int raw_to_bmp(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, int pixel_order,
	bool GammaEna, double gamma,
	int rr, int rg, int rb, int gr, int gg, int gb, int br, int bg, int bb,
	int r_offset, int g_offset, int b_offset)
{
	int i, j;
	int shift = bpp - 8;
	unsigned short tmp;
	BYTE* ptr = in_bytes;
	BYTE* src = in_bytes;
	WORD* srcWord = (WORD *)in_bytes;

	if (GammaEna)
	{
		src = in_bytes;
		gammaCorrection(src, src, width, height, bpp, gamma);
	}

	if (bpp > 8)
	{
		srcWord = (WORD *)in_bytes;
		// convert 16bit bayer to 8bit bayer
		for (i = 0; i<height; i++)
			for (j = 0; j<width; j++)
			{
				tmp = (*srcWord++) >> shift;
				*ptr++ = (BYTE)tmp;
			}
	}

	src = in_bytes;
	bayer_to_rgb24(src, out_bytes, width, height, pixel_order);

	src = out_bytes;
	rgb2rgb(src, src, width, height, bpp, rr, rg, rb, gr, gg, gb, br, bg, bb, r_offset, g_offset, b_offset);
	return 0;
}

int raw_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
	bool GammaEna, double gamma)
{
	int i, j;
	int shift = bpp - 8;
	unsigned short tmp;
	BYTE* dst = out_bytes;
	BYTE* src = in_bytes;
	WORD* srcWord = (WORD *)in_bytes;

	if (GammaEna)
	{
		src = in_bytes;
		gammaCorrection(src, src, width, height, bpp, gamma);
	}

	if (bpp > 8)
	{
		srcWord = (WORD *)in_bytes;
		// convert 16bit bayer to 8bit bayer
		for (i = 0; i<height; i++)
			for (j = 0; j<width; j++)
			{
				tmp = (*srcWord++) >> shift;
				*dst++ = (BYTE)tmp;
				*dst++ = (BYTE)tmp;
				*dst++ = (BYTE)tmp;
			}
	}
	else
	{
		for (i = 0; i<height; i++)
			for (j = 0; j<width; j++)
			{
				tmp = (*src++);
				*dst++ = (BYTE)tmp;
				*dst++ = (BYTE)tmp;
				*dst++ = (BYTE)tmp;
			}

	}


	return 0;
}

static void rgb2rgb(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
	int rr, int rg, int rb, int gr, int gg, int gb, int br, int bg, int bb,
	int r_offset, int g_offset, int b_offset)
{
	int i, j;
	int r_in, g_in, b_in, r_out, g_out, b_out;

	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
		{
			r_in = *in_bytes++;
			g_in = *in_bytes++;
			b_in = *in_bytes++;

			r_out = (rr * r_in + rg * g_in + rb * b_in) / 256 + r_offset;
			g_out = (gr * r_in + gg * g_in + gb * b_in) / 256 + g_offset;
			b_out = (br * r_in + bg * g_in + bb * b_in) / 256 + b_offset;

			*out_bytes++ = (r_out > 255) ? 255 : ((r_out < 0) ? 0 : r_out);
			*out_bytes++ = (g_out > 255) ? 255 : ((g_out < 0) ? 0 : g_out);
			*out_bytes++ = (b_out > 255) ? 255 : ((b_out < 0) ? 0 : b_out);
		}
}

int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
	int pixel32 = 0;
	BYTE *pixel = (BYTE *)&pixel32;
	int r, g, b;
	b = y + (1.370705 * (v - 128));
	g = y - (0.698001 * (v - 128)) - (0.337633 * (u - 128));
	r = y + (1.732446 * (u - 128));
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;
	return pixel32;
}
