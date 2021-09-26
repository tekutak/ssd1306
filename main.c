/* -----------------------------------------------------------------------------
 Include
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jpeglib.h>
#include "common.h"
#include "I2cCtl.h"
#include "Ssd1306.h"

#define FONT_BYTES_SIZE (8)
#define INPUT_IMG_PATH ("./test.jpg")

/* -----------------------------------------------------------------------------
 Typedef
------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 Global
------------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------
 Prototype
------------------------------------------------------------------------------*/
void DrawTest();
void DisplayImg();
void ConvRGB2Gray(U08 *src, U08 *dst, const U32 len);
void Binarization(U08 *src, U08 *dst, U32 len);
U08 GetMostVotedVal(U08 *src, U32 len);
void PackDisplayBitFromImg(U08 *src, U08 *dst, U32 width, U32 height);

/* -----------------------------------------------------------------------------
 Function   : main
 Memo       : 
 Date       : 2021.09.25
------------------------------------------------------------------------------*/
int main()
{
	BOOL ret;

	/* 初期化 */
	ret = I2cCtl_Init();
	if (ret == NG)
	{
		printf("I2cCtl_Init Error.\n");
		return -1;
	}
	ret = Ssd1306_Init();
	if (ret == NG)
	{
		printf("Ssd1306_Init Error.\n");
		return -1;
	}

	/* 描画 */
	// DrawTest();
	DisplayImg();

	return 0;
}

/* -----------------------------------------------------------------------------
 Function   : Draw
 Memo       : 
 Date       : 2021.09.25
------------------------------------------------------------------------------*/
void DrawTest()
{
	U08 *canvas = Ssd1306_Get_Draw_Canvas();
	canvas[0] = 0x0F;
	canvas[1] = 0x0F;
	canvas[2] = 0xF0;
	canvas[3] = 0xF0;
	Ssd1306_Update_Frame();
}

void DisplayImg()
{
	struct jpeg_decompress_struct cinfo;
	jpeg_create_decompress(&cinfo);

	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr); // 既定値

	FILE *fp = fopen(INPUT_IMG_PATH, "rb");
	if (fp == NULL)
	{
		printf("Error: failed to open\n");
		return;
	}

	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, TRUE);

	U32 width = cinfo.image_width;	 // 幅
	U32 height = cinfo.image_height; // 高さ
	U32 ch = cinfo.num_components;	 // チャネル数

	jpeg_start_decompress(&cinfo);

	U32 stride = width * ch;
	U08 *img_rgb = (U08 *)malloc(stride * height);
	U08 *p = img_rgb;
	for (U32 j = 0; j < height; j++)
	{
		jpeg_read_scanlines(&cinfo, &p, 1);
		p += stride;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);

	U08 *img_gray = (U08 *)malloc(width * height);
	U08 *img_binary = (U08 *)malloc(width * height);
	ConvRGB2Gray(img_rgb, img_gray, width * height);

	Binarization(img_gray, img_binary, width * height);

	U08 *canvas = Ssd1306_Get_Draw_Canvas();
	PackDisplayBitFromImg(img_binary, canvas, width, height);
	Ssd1306_Update_Frame();

	struct jpeg_compress_struct cinfo_out;
	jpeg_create_compress(&cinfo_out);
	cinfo_out.err = jpeg_std_error(&jerr);

	fp = fopen("out.jpg", "wb");
	if (fp == NULL)
	{
		printf("Error: failed to open\n");
		return;
	}

	jpeg_stdio_dest(&cinfo_out, fp);

	cinfo_out.image_width = width;
	cinfo_out.image_height = height;
	cinfo_out.input_components = 1;
	cinfo_out.in_color_space = JCS_GRAYSCALE;
	jpeg_set_defaults(&cinfo_out);

	jpeg_start_compress(&cinfo_out, TRUE);

	stride = width;
	p = img_binary;
	for (U32 j = 0; j < height; j++)
	{
		jpeg_write_scanlines(&cinfo_out, &p, 1);
		p += stride;
	}

	jpeg_finish_compress(&cinfo_out);
	jpeg_destroy_compress(&cinfo_out);
	fclose(fp);
}

void ConvRGB2Gray(U08 *src, U08 *dst, const U32 len)
{
	for (U32 i = 0; i < len; i++)
	{
		U08 r = *(src++);
		U08 g = *(src++);
		U08 b = *(src++);
		U08 val = (U08)((F32)r * 0.3 + (F32)g * 0.59 + (F32)b * 0.11);
		*(dst++) = val;
	}
}

void Binarization(U08 *src, U08 *dst, U32 len)
{
	U08 th = GetMostVotedVal(src, len);
	U08 LUT[256] = {0};
	for (U32 i = th; i < 256; i++)
	{
		LUT[i] = 1;
	}

	for (U32 i = 0; i < len; i++)
	{
		*(dst++) = LUT[*(src++)];
	}
}

U08 GetMostVotedVal(U08 *src, U32 len)
{
	U08 hist[256] = {0};
	for (U32 i = 0; i < len; i++)
	{
		hist[*(src++)] += 1;
	}

	U32 count = 0;
	for (U32 i = 0; i < 256; i++)
	{
		count += hist[i];

		if (count >= (U32)(len * 0.55))
		{
			return i;
		}
	}

	return 0;
}

void PackDisplayBitFromImg(U08 *src, U08 *dst, U32 width, U32 height)
{
	for (U32 j = 0; j < height / 8; j++)
	{
		for (U32 i = 0; i < width; i++)
		{
			U08 row0 = *(src + width * (j * 8 + 0) + i);
			U08 row1 = *(src + width * (j * 8 + 1) + i);
			U08 row2 = *(src + width * (j * 8 + 2) + i);
			U08 row3 = *(src + width * (j * 8 + 3) + i);
			U08 row4 = *(src + width * (j * 8 + 4) + i);
			U08 row5 = *(src + width * (j * 8 + 5) + i);
			U08 row6 = *(src + width * (j * 8 + 6) + i);
			U08 row7 = *(src + width * (j * 8 + 7) + i);
			*(dst + width * j + i) =
				(row7 << 7) | (row6 << 6) | (row5 << 5) | (row4 << 4) | (row3 << 3) | (row2 << 2) | (row1 << 1) | (row0 << 0);
		}
	}
}