#include "OggReader.h"
#include "nsLib/log.h"
#include "Core/Package.h"
#include "Core/Memory.h"

ov_callbacks nsOggReader::m_oggCallbacks = 
{
	nsOggReader::ov_read_f,
	nsOggReader::ov_seek_f,
	nsOggReader::ov_close_f,
	nsOggReader::ov_tell_f
};

const nsOggReader::oggerr_t nsOggReader::m_oggerr[] = 
{
	{ OV_FALSE, "OV_FALSE" },
	{ OV_HOLE, "OV_HOLE" },
	{ OV_EREAD, "OV_EREAD" },
	{ OV_EFAULT, "OV_EFAULT" },
	{ OV_EIMPL, "OV_EIMPL" },
	{ OV_EINVAL, "OV_EINVAL" },
	{ OV_ENOTVORBIS, "OV_ENOTVORBIS" },
	{ OV_EBADHEADER, "OV_EBADHEADER" },
	{ OV_EVERSION, "OV_EVERSION" },
	{ OV_EBADLINK, "OV_EBADLINK" },
	{ OV_ENOSEEK, "OV_ENOSEEK" }
};

const int nsOggReader::m_errCount = sizeof(nsOggReader::m_oggerr) / sizeof(nsOggReader::oggerr_t);

//---------------------------------------------------------
// nsOggReader::nsOggReader: 
//---------------------------------------------------------
nsOggReader::nsOggReader() :
	m_file( 0 ), m_offset(0)
{

}

//---------------------------------------------------------
// nsOggReader::StartLoading: 
//---------------------------------------------------------
bool nsOggReader::StartReading( const char* fileName, bool looped )
{
	Free();
	
	if ( !(m_file = g_pack.LoadFile( fileName )) )
	{
		LogPrintf( PRN_ALL, "WARNING: ogg file not found '%s'\n", fileName );
		return false;
	}

	if ( ov_open_callbacks( this, &m_oggFile, 0, 0, m_oggCallbacks ) )
	{
		LogPrintf( PRN_ALL, "WARNING: error read ogg '%s'\n", fileName );
		g_pack.ReleaseFile( m_file );
        m_file = nullptr;
		return false;
	}

	m_oggSection = 0;
	m_looped = looped;
	return true;
}

//---------------------------------------------------------
// nsOggReader::GetVorbisInfo: 
//---------------------------------------------------------
vorbis_info* nsOggReader::GetVorbisInfo()
{
	if ( !m_file ) return 0;
	return ov_info( &m_oggFile, -1 );
}

//---------------------------------------------------------
// nsOggReader::Seek: 
//---------------------------------------------------------
void nsOggReader::Seek( long pos )
{
	if ( !m_file ) return;
	ov_raw_seek( &m_oggFile, pos );
}

//---------------------------------------------------------
// nsOggReader::Tell: 
//---------------------------------------------------------
long nsOggReader::Tell()
{
	if ( !m_file ) return 0;
	return (long)ov_raw_tell( &m_oggFile );
}

//---------------------------------------------------------
// nsOggReader::Read: 
//---------------------------------------------------------
OggResult nsOggReader::Read( void *buff, long &size )
{
	long ret = ov_read( &m_oggFile, (char*)buff, size, 0, 2, 1, &m_oggSection );
	if ( !ret )
	{
		if ( m_looped )
		{
			ov_raw_seek( &m_oggFile, 0 );
			ret = ov_read( &m_oggFile, (char*)buff, size, 0, 2, 1, &m_oggSection );
			if ( !ret || ret == OV_HOLE || ret == OV_EBADLINK )
			{
				LogPrintf( PRN_ALL, "WARNING: ogg rewing error!\n" );
				return OGG_ERROR;
			}
			LogPrintf( PRN_ALL, "ogg rewind\n" );
			size = ret;
		}
		else
			size = 0;
		
		return OGG_EOF;
	}
	else if ( ret == OV_HOLE || ret == OV_EBADLINK )
	{
		LogPrintf( PRN_ALL, "WARNING: ogg read error!\n" );
		return OGG_ERROR;
	}

	size = ret;
	return OGG_OK;
}

#define READ_BUFF_SIZE	65536

//---------------------------------------------------------
// nsOggReader::ReadWhole: 
//---------------------------------------------------------
void* nsOggReader::ReadWhole( long &resSize )
{
	long	size = READ_BUFF_SIZE;
	byte	*buff = (byte*)my_malloc( READ_BUFF_SIZE );
	byte	*resBuff = 0;
	resSize = 0;

	while ( Read( buff, size ) == OGG_OK )
	{
		if ( !resBuff )
			resBuff = (byte*)my_malloc( size );
		else
			resBuff = (byte*)my_realloc( resBuff, resSize + size );

		memcpy( &resBuff[resSize], buff, size );

		resSize += size;
		size = READ_BUFF_SIZE;
	}

	my_free( buff );
	return resBuff;
}

//---------------------------------------------------------
// nsOggReader::Free: 
//---------------------------------------------------------
void nsOggReader::Free()
{
	if ( m_file )
	{
		ov_clear( &m_oggFile );
		g_pack.ReleaseFile( m_file );
		m_file = nullptr;
        m_offset = 0;
	}
}

//---------------------------------------------------------
// nsOggReader::ov_read_f: 
//---------------------------------------------------------
size_t nsOggReader::ov_read_f( void *ptr, size_t size, size_t nmemb, void *datasource )
{
    auto reader = (nsOggReader*)datasource;
	auto file = reader->m_file;
	uint	read_size = size * nmemb;
	uint	rem_size = file->GetSize() - reader->m_offset;
	if ( !rem_size ) return 0;
	
	if ( read_size > rem_size )
		read_size = rem_size;

	memcpy( ptr, file->GetData() + reader->m_offset, read_size );
    reader->m_offset += read_size;

	return read_size / size;
}

//---------------------------------------------------------
// nsOggReader::ov_seek_f: 
//---------------------------------------------------------
int nsOggReader::ov_seek_f( void *datasource, ogg_int64_t offset, int whence )
{
    auto reader = (nsOggReader*)datasource;
	auto file = reader->m_file;
	switch ( whence )
	{
	case SEEK_SET:
		if ( offset > file->GetSize() || offset < 0 ) return 0;
		reader->m_offset = (uint)offset;
		break;
	case SEEK_CUR:
		if ( offset + reader->m_offset > file->GetSize() ) return 0;
		else if ( offset + reader->m_offset < 0 ) return 0;
            reader->m_offset += (uint)offset;
		break;
	case SEEK_END:
		if ( offset > 0 || labs( (long)offset ) > (long)file->GetSize() ) return 0;
            reader->m_offset = uint(file->GetSize() + offset);
		break;
	}
	return 0;
}
  
//---------------------------------------------------------
// nsOggReader::ov_close_f: 
//---------------------------------------------------------
int nsOggReader::ov_close_f( void *datasource )
{
	return 0;
}

//---------------------------------------------------------
// nsOggReader::ov_tell_f: 
//---------------------------------------------------------
long nsOggReader::ov_tell_f( void *datasource )
{
	auto reader = (nsOggReader*)datasource;
	return (long)reader->m_offset;
}

//---------------------------------------------------------
// nsOggReader::GetErrorDesc: 
//---------------------------------------------------------
const char*	nsOggReader::GetErrorDesc( int code )
{
	for ( int i = 0; i < m_errCount; ++i )
		if ( m_oggerr[i].code == code )
			return m_oggerr[i].desc;
	return "No error desc!";
}


