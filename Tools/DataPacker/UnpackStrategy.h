#ifndef __UnpackStrategy_H__
#define __UnpackStrategy_H__

/*******************************************************\
*
* GROm Games (c) 2003-2019
*
\*******************************************************/

class UnpackStrategy
{
public:
	UnpackStrategy(const char* packedFile, const char* targetFolder);
	virtual ~UnpackStrategy() = default;

	bool Perform();

private:
	const char *_packetFile;
	const char *_targetFolder;
	bool WriteFile(const nsFilePath &filePath, const void *data, unsigned int size);
};

#endif
