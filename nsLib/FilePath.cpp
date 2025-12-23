//
// Created by capricorn on 24.08.19.
//

#include "FilePath.h"
#include <sys/stat.h>
#include <filesystem>
#include <dirent.h>

namespace fs = std::filesystem;

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
#if !(defined(ANDROID) || defined(WEB_ASM))
		return rmdir(_path) == 0;
#endif
	}

	return remove(_path) == 0;
}

bool nsFilePath::MakeFolder(const char *name)
{
	return std::filesystem::create_directories(name);
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
	if (_path.IsEmpty()) {
		return {relative};
	}

	nsString path = _path;
	if (path[path.Length() - 1] != '/')
	{
		path += "/";
	}
	path += relative;
	return {path};
}

nsString nsFilePath::GetRelativePath(const nsFilePath &path) const {
	const fs::path base = static_cast<const char *>(_path);
	const fs::path target = static_cast<const char *>(path._path);
	nsString res = fs::relative( target, base).string().c_str();
	Normalize(res);
	return res;
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
	const auto     slash  = strrchr(parent, '/');
	if (!slash)
	{
		return "";
	}

	*slash = 0;
	return parent.AsChar();
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
			if (!MakeFolder(str))
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
	if (const auto lastSlash = strrchr(_path, '/')) {
        nsString name = lastSlash + 1;
        if (const auto dot = strchr(name.AsChar(), '.')) {
            *dot = 0;
        }
        return name;
    }
    return "";
}

nsString nsFilePath::GetName() const {
	if (const auto lastSlash = strrchr(_path, '/')) {
		return lastSlash + 1;
	}
	return "";
}


