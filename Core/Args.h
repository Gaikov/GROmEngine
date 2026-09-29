// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Args.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsArgs {
public:
	nsArgs();
	explicit nsArgs( const char *line );
	virtual ~nsArgs();

	[[nodiscard]] int ArgCount() const;
	[[nodiscard]] const char *Arg( int index ) const;
	[[nodiscard]] const char **GetArgs() const;
	[[nodiscard]] bool HasArg( const char *value ) const;
	[[nodiscard]] const char *GetValue( const char *name ) const;

	void FromArgs( int count, const char *args[] );
	void FromLine( const char *line );
	void Clear();

private:
	void AddFromLine( const char *line );
	void Free();

	int _argCount = 0;
	char **_args = nullptr;
};
