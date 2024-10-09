#ifndef _OGG_READER_H_
#define _OGG_READER_H_

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "nsLib/comm_types.h"

enum OggResult
{
	OGG_OK = 0,
	OGG_ERROR,
	OGG_EOF
};

class nsFile;

//---------------------------------------------------------
// nsOggReader: 
//---------------------------------------------------------
class nsOggReader
{
public:
	nsOggReader();
	bool			StartReading( const char* fileName, bool looped );
	vorbis_info*	GetVorbisInfo();
	void			Seek( long pos );
	long			Tell();
	OggResult		Read( void *buff, long &size );
	void*			ReadWhole( long &size );
	void			Free();
	

private:
	typedef struct 
	{
		int		code;
		const char	*desc;
	}
	oggerr_t;

	OggVorbis_File			m_oggFile;
	nsFile					*m_file;
    uint                    m_offset;
	int						m_oggSection;
	bool					m_looped;

	static ov_callbacks		m_oggCallbacks;
	static const oggerr_t	m_oggerr[];
	static const int		m_errCount;

private:
	static size_t	ov_read_f( void *ptr, size_t size, size_t nmemb, void *datasource );
	static int		ov_seek_f( void *datasource, ogg_int64_t offset, int whence );
	static int		ov_close_f( void *datasource );
	static long		ov_tell_f( void *datasource );

	const char*		GetErrorDesc( int code );
};

#endif