#ifndef LEOPARDUTIL_H
#define LEOPARDUTIL_H

typedef unsigned char BYTE;
typedef unsigned short WORD;

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

static void initGammaTable(double gamma, int bpp);
static void gammaCorrection(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, double gamma);
int raw_to_bmp(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp, int pixel_order,
	bool GammaEna, double gamma,
	int rr, int rg, int rb, int gr, int gg, int gb, int br, int bg, int bb,
	int r_offset, int g_offset, int b_offset);
int raw_to_bmp_mono(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
	bool GammaEna, double gamma);
static void rgb2rgb(BYTE* in_bytes, BYTE* out_bytes, int width, int height, int bpp,
	int rr, int rg, int rb, int gr, int gg, int gb, int br, int bg, int bb,
	int r_offset, int g_offset, int b_offset);
int convert_yuv_to_rgb_pixel(int y, int u, int v);

////////////// GAMMA_CORRECTION ///////////////////////
static unsigned short linear_to_gamma[65536];
static double gammaValue = -1;
static int gBPP = 0;


#endif
