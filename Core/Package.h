// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Package.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Package_H_
#define	_Package_H_

#include "PackArch.h"
#include "nsLib/StrTools.h"
#include "Blob.h"
#include "nsLib/events/EventDispatcher.h"

//---------------------------------------------------------
// CPackage: 
//---------------------------------------------------------
class nsPackage : public nsEventDispatcher
{
public:
	enum {FILE_LOADED_EVENT = 0};

	//initialize
	bool			Init();
	void			Release();
	bool			AddPack( const char *fileName );

	nsFile*			LoadFile( const char *fileName );
	void			ReleaseFile( nsFile *file );
	
	//file manipulation
	bool			IsExists( const char *fileName );
	//bool			FileWrite( const char *fileName, const void *data, int size, bool overwrite = false );
	
	int				EnumDirFiles( const char *dir, const char *type, nsString **list, bool withPacks = false );

    void            SetPassKey(const char *key);
    void            SetLoadPackedOnly(bool packed);

private:
	typedef struct packDesc_s
	{
		nsString		packName = nullptr;
		packFileDesc_t	*files = nullptr;
		unsigned int	count = 0;
	}
	packDesc_t;

    std::vector<packDesc_t> m_packs;

    nsString        _decryptionKey;
    bool            _loadPacked = false;

private:
	bool			InitPack( const char *fileName );
	packFileDesc_t*	FindPackFile( const char *fileName, int pack );
	nsFile*			LoadPackFile( const char *fileName );
	void			DecodeFile( nsFile *file );
};

extern	nsPackage	g_pack;

#endif //_Package_H_
