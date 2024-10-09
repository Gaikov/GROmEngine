// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ResourceMap.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_ResourceMap_H_
#define	_ResourceMap_H_

#include "nsLib/StrTools.h"
#include "Core/Parser.h"
#include "RenManager.h"
#include "SndManager.h"

class nsResourceMap
{
public:
	nsResourceMap();

	bool		Init();
	void		Release();

	const char*	TextureFile( const char *fileNameOrID );

private:
	struct texMap_t
	{
		nsString	id;
		nsString	fileName;
	};

	struct soundMap_t
	{
		nsString	id;
		nsString	fileName;
	};

private:
	texMap_t		*m_texMap;
	int				m_texCount;

	soundMap_t		*m_sndMap;
	int				m_sndCount;

private:
	template <class T>
	bool		ParseRes( T **map, int &count, script_state_t *ss, const char *blockName );

	template <class T>
	T*			FindRes( T *map, int count, const char *id );
};

extern nsResourceMap g_resMap;

#endif //_ResourceMap_H_