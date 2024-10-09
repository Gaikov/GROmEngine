#include "StdAfx.h"
#include "Args.h"

Args::Args(int count, char* argv[])
{
	_args.reserve(count);
	for (int i = 0; i < count; i++)
	{
		_args.emplace_back(argv[i]);
	}
}

bool Args::HasOption(const char* option) const
{
    return GetIndex(option) >= 0;
}

const char *Args::GetByName(const char *name) const {
    auto index = GetIndex(name);
    if (index >= 0 && index < _args.size() - 1) {
        return _args[index +1].c_str();
    }
    return nullptr;
}

int Args::GetIndex(const char *name) const {
    for (int i = 0; i < _args.size(); i++) {
        if (_args[i] == name){
            return i;
        }
    }
    return -1;
}

