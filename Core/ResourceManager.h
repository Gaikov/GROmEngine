// Copyright (c) 2003-2007, Gaikov Roman. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ResourceManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------

#ifndef	_ResourceManager_H_
#define	_ResourceManager_H_

#include "headers.h"
#include "StructUt.h"

template<class Resource, class LoadParams>
class nsResourceManager
{
protected:
	struct resDesc_t
	{
		nsString		fileName;
		Resource		*res;
		int				refCount;
		resDesc_t		*prev, *next;
	};

	nsResourceManager();
	virtual ~nsResourceManager();

	Resource*		LoadRes( const char* fileName, LoadParams *params = 0 );
	void			ReleaseRes( Resource* res );

	Resource*		GetFirst();
	Resource*		GetNext();

	resDesc_t*		FindRes( const char* fileName );
	resDesc_t*		FindRes( Resource *res );

private:
	resDesc_t		*m_resList;
	resDesc_t		*m_fetch;	//для выборки, GetFirst, GetNext

private:
	//вызывается в LoadRes если ресурс не найден и нужно загрузить его
	virtual bool	OnLoadRes( const char *fileName, Resource* res, LoadParams *params ) = 0;

	nsResourceManager::nsResourceManager( const nsResourceManager& );
	nsResourceManager& operator = ( const nsResourceManager& );
};

//---------------------------------------------------------
// nsResourceManager::nsResourceManager:
//---------------------------------------------------------
template<class Resource, class LoadParams>
nsResourceManager<Resource, LoadParams>::nsResourceManager()
{
	m_resList = 0;
	m_fetch = 0;
}

//---------------------------------------------------------
// nsResourceManager::~nsResourceManager:
//---------------------------------------------------------
template<class Resource, class LoadParams>
nsResourceManager<Resource, LoadParams>::~nsResourceManager()
{
	resDesc_t	*resDesc = m_resList;
	while ( resDesc )
	{
		resDesc_t	*del = resDesc;
		resDesc = resDesc->next;
		
		delete del->res;
		delete del;
	}
	
	m_resList = 0;
}

//---------------------------------------------------------
// nsResourceManager::LoadRes:
//---------------------------------------------------------
template<class Resource, class LoadParams>
Resource* nsResourceManager<Resource, LoadParams>::LoadRes( const char* fileName, LoadParams *params )
{
	resDesc_t	*resDesc = FindRes( fileName );
	if ( resDesc )
	{
		resDesc->refCount ++;
		return resDesc->res;
	}

	resDesc = new resDesc_t;
	resDesc->fileName = fileName;
	resDesc->refCount = 1;
	resDesc->res = new Resource;
	if ( !OnLoadRes( fileName, resDesc->res, params ) )
	{
		delete resDesc->res;
		delete resDesc;
		return 0;
	}
	
	AddToList( &m_resList, resDesc );
	return resDesc->res;
}

//---------------------------------------------------------
// nsResourceManager::ReleaseRes:
//---------------------------------------------------------
template<class Resource, class LoadParams>
void nsResourceManager<Resource, LoadParams>::ReleaseRes( Resource* res )
{
	resDesc_t	*resDesc = FindRes( res );
	if ( !resDesc ) return;

	resDesc->refCount --;
	if ( resDesc->refCount <= 0 )
	{
		DelFromList( &m_resList, resDesc );
		delete resDesc->res;
		delete resDesc;
	}
}

//---------------------------------------------------------
// nsResourceManager::GetFirst:
//---------------------------------------------------------
template<class Resource, class LoadParams>
Resource* nsResourceManager<Resource, LoadParams>::GetFirst()
{
	m_fetch = m_resList;
	return m_fetch->res;
}

//---------------------------------------------------------
// nsResourceManager::GetNext:
//---------------------------------------------------------
template<class Resource, class LoadParams>
Resource* nsResourceManager<Resource, LoadParams>::GetNext()
{
	if ( !m_fetch ) return 0;
	m_fetch = m_fetch->next;
	return m_fetch->res;
}

//---------------------------------------------------------
// nsResourceManager::FindRes:
//---------------------------------------------------------
template<class Resource, class LoadParams>
typename nsResourceManager<Resource, LoadParams>::resDesc_t*
nsResourceManager<Resource, LoadParams>::FindRes( const char* fileName )
{
	for ( resDesc_t *resDesc = m_resList; resDesc; resDesc = resDesc->next )
	{
		if ( resDesc->fileName == fileName )
			return resDesc;
	}
	return 0;
}

//---------------------------------------------------------
// nsResourceManager::FindRes:
//---------------------------------------------------------
template<class Resource, class LoadParams>
typename nsResourceManager<Resource, LoadParams>::resDesc_t* 
nsResourceManager<Resource, LoadParams>::FindRes( Resource *res )
{
	for ( resDesc_t *resDesc = m_resList; resDesc; resDesc = resDesc->next )
	{
		if ( resDesc->res == res )
			return resDesc;
	}
	return 0;
}

#endif	//_ResourceManager_H_
