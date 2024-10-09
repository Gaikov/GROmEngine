//
// Created by capricorn on 24.08.19.
//

#include "FilePath.h"
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

nsFilePath::nsFilePath(const char *path) :
		_path(path)
{
	Normalize(_path);
}

nsString nsFilePath::GetExtension() const
{
	nsString ext;
	auto     dot = strrchr(_path, '.');
	if (dot)
	{
		dot++;
		ext = dot;
	}

	return ext;
}

nsString nsFilePath::GetPathWithoutExt() const
{
	nsString name = _path;
	auto     dot  = strrchr(name.AsChar(), '.');
	if (dot)
	{
		*dot = 0;
	}
	return name;
}

bool nsFilePath::CheckExtension(const char *ext) const
{
	auto     currentExt = GetExtension();
	nsString otherExt   = ext;

	currentExt.ToLower();
	otherExt.ToLower();

	return currentExt == otherExt;
}

bool nsFilePath::IsFolder() const
{
	struct stat s = {};
	memset(&s, 0, sizeof(s));
	if (stat(_path, &s))
	{
		return false;
	}
	return s.st_mode & S_IFDIR;
}

bool nsFilePath::IsExists() const
{
	return Exists(_path);
}

bool nsFilePath::Remove() const
{
	if (IsFolder())
	{
		tList files;
		if (!Listing(files))
		{
			return false;
		}

		for (auto &file : files)
		{
			if (!file.Remove())
			{
				return false;
			}
		}
#ifndef ANDROID
		return rmdir(_path) == 0;
#endif
	}

	return remove(_path) == 0;
}

bool nsFilePath::MakeFolder(const char *name)
{
	int result;
#ifdef _WIN32_WINNT
	result = mkdir(name);
#else
	result = mkdir(name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//result = mkdir(name, 0x755);
#endif
	return result == 0;
}

bool nsFilePath::Listing(nsFilePath::tList &result) const
{
	std::vector<nsString> list;
	if (!FolderListing(_path, list))
	{
		return false;
	}

	for (auto &name : list)
	{
		result.push_back(ResolvePath(name));
	}

	return true;
}

nsFilePath nsFilePath::ResolvePath(const char *relative) const
{
	nsString path = _path;
	if (path[path.Length() - 1] != '/')
	{
		path += "/";
	}
	path += relative;
	return nsFilePath(path);
}

bool nsFilePath::FolderListing(const char *folderPath, std::vector<nsString> &result)
{
	auto   handler = opendir(folderPath);
	dirent *entry;

	if (handler)
	{
		while ((entry = readdir(handler)) != nullptr)
		{
			nsString name = entry->d_name;
			if (name != "." && name != "..")
			{
				result.push_back(name);
			}
		}
		closedir(handler);
		return true;
	}

	return false;
}

nsFilePath nsFilePath::GetParent() const
{
	nsString parent = _path;
	auto     slash  = (char *) strrchr(parent, '/');
	if (!slash)
	{
		return nsFilePath("");
	}

	*slash = 0;
	return nsFilePath(parent);
}

bool nsFilePath::CreateFolders() const
{
	if (_path.IsEmpty())
	{
		return false;
	}

	nsString str    = _path;
	char     *slash = str.AsChar();
	do
	{
		slash = strchr(slash, '/');
		if (slash) *slash = 0;
		if (!Exists(str))
		{
			if (!nsFilePath::MakeFolder(str))
			{
				return false;
			}
		}

		if (slash)
		{
			*slash = '/';
			slash++;
		}
	} while (slash);

	return true;
}

void nsFilePath::Normalize(nsString &path)
{
	auto str      = path.AsChar();
	char prevChar = 0;
	while (*str)
	{
		if (*str == '\\')
		{
			*str = '/';
		}

		if (*str == '/' && prevChar == '/')
		{
			memmove(str, str + 1, strlen(str));
		} else
		{
			prevChar = *str;
			str++;
		}
	}
}

bool nsFilePath::Exists(const char *path)
{
	struct stat s;
	memset(&s, 0, sizeof(s));
	return stat(path, &s) == 0;
}

bool nsFilePath::ListingRecursive(nsFilePath::tList &result) const {
    if (!IsFolder()) {
        return false;
    }

    nsFilePath::tList list;
    Listing(list);

    for (auto &file : list) {
        if (file.IsFolder()) {
            file.ListingRecursive(result);
        } else {
            result.push_back(file);
        }
    }

    return true;
}

nsString nsFilePath::GetNameOnly() const {
    auto lastSlash = strrchr(_path, '/');
    if (lastSlash) {
        nsString name = lastSlash + 1;
        auto dot = strchr(name.AsChar(), '.');
        if (dot) {
            *dot = 0;
        }
        return name;
    }
    return "";
}


