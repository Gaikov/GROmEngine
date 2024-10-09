#ifndef __Args_H__
#define __Args_H__

/*******************************************************\
*
* GROm Games (c) 2003-2019
*
\*******************************************************/
#include "StdAfx.h"

class Args
{
public:
	Args(int count, char* argv[]);
	virtual ~Args() = default;

	bool HasOption(const char *option) const;
	const char* GetParam(const int index) const { return _args[index].c_str(); }
    const char* GetByName(const char *name) const;
    int GetIndex(const char *name) const;
	int Length() const { return (int)_args.size(); }

private:
	typedef std::vector<std::string> tArgs;
	tArgs _args;
};

#endif
