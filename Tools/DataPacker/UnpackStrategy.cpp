#include "StdAfx.h"
#include "UnpackStrategy.h"
#include "Core/PackArch.h"

void DecodeText(unsigned char *data, unsigned int size)
{
	unsigned char	lo, hi;
	for (unsigned int i = 0; i < size; i++, data++)
	{
		lo = (*data) & 0x0F;
		hi = (*data) & 0xF0;
		*data = (lo << 4) | (hi >> 4);
	}
}

UnpackStrategy::UnpackStrategy(const char* packedFile, const char* targetFolder) :
	_packetFile(packedFile), _targetFolder(targetFolder)
{
}

bool UnpackStrategy::Perform()
{
	FILE* sourceFile = nullptr;
	sourceFile = fopen(_packetFile, "rb");
	if (!sourceFile)
	{
		printf("can't open packed file");
		return false;
	}

	packHeader_t header;
	int size = sizeof(header);
	if (fread(&header, sizeof(header), 1, sourceFile) != 1)
	{
		printf("can't read packed file header");
		return false;
	}

	if (!checkPackHeader(header))
	{
		printf("invalid packed file format");
		return false;
	}

	const auto numFiles = header.dir_size / sizeof(packFileDesc_t);
	printf("%u files found...\n", numFiles);
	std::vector<packFileDesc_t> files;
	for (unsigned int i = 0; i < numFiles; i++)
	{
		packFileDesc_t fileDesc;
		const auto descSize = sizeof(fileDesc);
		const auto read = fread(&fileDesc, 1, sizeof(packFileDesc_t), sourceFile);
		if (read != descSize)
		{
			printf("can't read files info\n");
			return false;
		}

		//printf("%s\n", fileDesc.filename);
		files.push_back(fileDesc);
	}

	nsFilePath folder(_targetFolder);
	for (auto& file : files)
	{
		printf("unpacking: %s\n", file.filename);
		if (fseek(sourceFile, file.offset, SEEK_SET))
		{
			printf("can't locate file");
			return false;
		}

		const auto data = malloc(file.size);
		if (fread(data, 1, file.size, sourceFile) != file.size)
		{
			printf("can't read file");
			return false;
		}

		nsFilePath filePath = folder.ResolvePath(file.filename);
		nsFilePath parent = filePath.GetParent();
		if (!parent.CreateFolders())
		{
			printf("can't create folders: %s\n", (const char*)parent);
			return false;
		}

		WriteFile(filePath, data, file.size);

		free(data);
	}

	return true;
}

bool UnpackStrategy::WriteFile(const nsFilePath &filePath, const void *data, unsigned int size)
{
	FILE *hfile = nullptr;
	hfile = fopen(filePath, "w+b");
	if (!hfile)
	{
		printf("can't create file\n");
		return false;
	}

	if (filePath.CheckExtension("txt"))
	{
		printf("...decoding text file\n");
		DecodeText((unsigned char*)(data), size);
	}

	if (fwrite(data, 1, size, hfile) != size)
	{
		printf("can't write data\n");
		return false;
	}
	fclose(hfile);
    return true;
}
