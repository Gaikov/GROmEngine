// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file PackArch.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_PackArch_H_
#define	_PackArch_H_

#include <cstring>

#define	PACK_VERSION	0x0102
#define PACK_ID "PACK"
#define PACK_MAX_PATH   128

#pragma pack( push, 1 )

typedef struct
{
	char	id[4];				//PACK
	unsigned short	version;	//0x0100
	unsigned int	dir_size;
}
packHeader_t;

typedef struct
{
	char			filename[PACK_MAX_PATH];
	unsigned int	offset;
	unsigned int	size;
}
packFileDesc_t;

#pragma pack( pop )

bool checkPackHeader(const packHeader_t &header);

#endif //_PackArch_H_