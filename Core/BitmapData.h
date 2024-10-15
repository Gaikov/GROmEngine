//
// Created by Roman on 28/07/2019.
//

#ifndef _GROM_BITMAPDATA_H
#define _GROM_BITMAPDATA_H

#include "Core/headers.h"

struct nsPixel
{
	unsigned char r, g, b, a;
};

class nsBitmapData
{
public:
	typedef std::shared_ptr<nsBitmapData> tSP;
	virtual ~nsBitmapData();

	static nsBitmapData::tSP Create(int width, int height);
	int GetWidth() const;
	int GetHeight() const;
	const nsPixel *GetData() const;
    nsPixel *GetWriteData() { return _data; }
	void SetData(const void *data);
	void SetScanLine(int y, const void *data);
	nsPixel* GetScanLine(int y);
    void PremultiplyAlpha();

protected:
	nsBitmapData(int w, int h);

protected:
	int     _width;
	int     _height;
	nsPixel *_data;
};


#endif //_GROM_BITMAPDATA_H
