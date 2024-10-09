// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Name.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Name_H_
#define	_Name_H_

//---------------------------------------------------------
// nsName: 
//---------------------------------------------------------
class nsName
{
public:
	nsName();
	explicit nsName( const char *name );
	
	int			AsInt() const;
	const char*	AsChar() const;
	bool		IsValid() const;

	bool		operator == ( const nsName &name ) const;
	bool		operator != ( const nsName &name ) const;

private:
	int			m_id;
};

//---------------------------------------------------------
// nsNamePool: 
//---------------------------------------------------------
class nsNamePool
{
friend struct nsCore;

public:	
	static int			GetID( const char *name );
	static const char*	GetNameByID( int id );
	static bool			IsValidID( int id );

private:
	static char			**m_names;
	static int			m_count;

private:
	static void			Free();
};

#endif //_Name_H_
