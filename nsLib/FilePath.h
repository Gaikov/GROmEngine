//
// Created by capricorn on 24.08.19.
//

#pragma once
#include "StrTools.h"

class nsFilePath
{
public:
	typedef std::vector<nsFilePath> tList;

public:
	static bool MakeFolder(const char *name);
	static bool FolderListing(const char *folderPath, std::vector<nsString> &result);
	static void Normalize(nsString &path);
	static bool Exists(const char *path);

public:
	nsFilePath(const char *path);
	nsString GetExtension() const;
	nsString GetPathWithoutExt() const;
    nsString GetNameOnly() const;
	nsString GetName() const;

	bool Listing(tList &result) const;
    bool ListingRecursive(tList &result) const;
	nsFilePath ResolvePath(const char *relative) const;
	nsString GetRelativePath(const nsFilePath &path) const;
	bool CheckExtension(const char *ext) const;
	bool IsFolder() const;
	bool IsExists() const;
	bool Remove() const;

	nsFilePath GetParent() const;
	bool CreateFolders() const;

	const nsString &GetPath() const
	{ return _path; }

    const char* AsChar() const {
        return _path;
    }

	operator const char *() const
	{ return _path; };

private:
	nsString _path;
};

