// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Anim.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Anim_H_
#define	_Anim_H_

#include "Core/Name.h"
#include "Core/Parser.h"

//---------------------------------------------------------
// nsTexAnimFrames: 
//---------------------------------------------------------
struct nsAnimFrames
{
friend class nsAnim;

private:
	nsName	id;
	float	fps;
	int		range[2];
	bool	loop;
	nsName	next;
};

class nsAnim;

//---------------------------------------------------------
// nsTexAnimState: 
//---------------------------------------------------------
class nsAnimState
{
friend class nsAnim;

public:
	nsAnimState();
	inline float GetFrame() { return m_frame; }

private:
	float			m_frame;
	nsAnimFrames	*m_currSet;
	nsAnim			*m_currAnim;
};

//---------------------------------------------------------
// nsTexAnim: 
//---------------------------------------------------------
class nsAnim
{
public:
	virtual ~nsAnim();
	static nsAnim*	Load( const char *fileName );
	static void		anim_make_f( int argc, const char* argv[] );
	
	void			AnimPlay( const nsName &name, nsAnimState &state );
	void			AnimMove( nsAnimState &state );
	bool			AnimIsPlaying( const nsName &name, nsAnimState &state );

private:
	nsAnimFrames	*m_frame;
	int				m_frameCount;

private:
	nsAnim();
	bool			Parse( script_state_t *ss );
	nsAnimFrames*	FindFrameset( const nsName &name );
};

#endif //_Anim_H_