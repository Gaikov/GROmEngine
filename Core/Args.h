// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Args.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#ifndef	_Args_H_
#define	_Args_H_

class nsArgs
{
public:
	nsArgs();
	explicit nsArgs( const char* line );
	virtual ~nsArgs();

	int					ArgCount() const;
	const char*			Arg( int idx ) const;
	const char**		GetArgs() const;
	void				FromArgs( int argc, const char* argv[] );
	void				FromLine( const char *line );
	bool                HasArg(const char *value) const;

private:
	int					m_argCount;
	char**				m_args;

private:
	void				AddFromLine( const char* line );
	void				Free();
};

#endif	//_Args_H_