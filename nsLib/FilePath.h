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
    static bool IsAbsolute(const char *path);

public:
	nsFilePath(const char *path);
	nsString GetExtension() const;
	nsString GetPathWithoutExt() const;
    nsString GetNameOnly() const;
	nsString GetName() const;

	bool IsEmpty() const { return _path.IsEmpty(); }
	bool Listing(tList &result) const;
    bool ListingRecursive(tList &result) const;
	nsFilePath ResolvePath(const char *relative) const;
	nsFilePath WithSuffix(const char *suffix) const;
	nsString GetRelativePath(const nsFilePath &path) const;
	bool CheckExtension(const char *ext) const;
	bool IsFolder() const;
	bool IsExists() const;
	bool Remove() const;
	bool RemoveIfExists() const;
	bool RenameTo(const nsFilePath &destination) const;
	bool GetCanonical(nsFilePath &result) const;
	bool IsWithin(const nsFilePath &root, bool caseInsensitive) const;

	nsFilePath GetParent() const;
	nsFilePath ToAbsolute() const;
	bool CreateFolders() const;
	bool CreateParentFolders() const;

	const nsString &GetPath() const
	{ return _path; }

    const char* AsChar() const {
        return _path;
    }

	operator const char *() const
	{ return _path; };

	bool operator==(const nsFilePath &other) const {
        return _path == other._path;
    }

private:
	nsString _path;
};
