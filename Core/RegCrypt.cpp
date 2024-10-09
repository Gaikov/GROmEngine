#include "RegCrypt.h"
#include <string.h>

typedef unsigned char	uchar;

//---------------------------------------------------------
// Reg_CryptInfo: 
//---------------------------------------------------------
int Reg_CryptInfo( const char* name, const char* code, char outData[MAX_CRYPT_LEN*2] )
{
	int		nameLen, codeLen;
	int		commLen;
	int		commSize;
	uchar	checkSumm = 0;
	int		i;

	if ( !name || !code ) return 0;
	
	nameLen = strlen( name );
	codeLen = strlen( code );
	commLen = nameLen + codeLen;
	commSize = commLen + 1 + 1 + 1; //nameLen + codeLen + checkSummm
	if ( !nameLen || !codeLen || commSize >= MAX_CRYPT_LEN )
		return 0;

	outData[0] = nameLen;
	outData[1] = codeLen;

	strcpy( &outData[2], name );
	strcpy( &outData[nameLen + 2], code );

	outData[commSize - 1] = 0;

	for ( i = 0; i < commSize - 1; ++i )
		checkSumm += outData[i];

	outData[commSize - 1] = checkSumm;
	outData[commSize] = 0;

	return Reg_CryptStr( outData, outData );
}

//---------------------------------------------------------
// Reg_DecryptInfo: 
//---------------------------------------------------------
int Reg_DecryptInfo( const char *data, char* outName, int nameSize, char* outCode, int codeSize )
{
	int		dataLen;
	char	checkSumm = 0;
	uchar	nameLen;
	uchar	codeLen;
	int		i;
	char	outData[MAX_CRYPT_LEN];

	if ( !data || !outName || !outCode ) return 0;

	Reg_DecryptStr( data, outData );

	dataLen = strlen( outData );
	if ( !dataLen ) return 0;

	for ( i = 0; i < dataLen - 1; i++ )
		checkSumm += outData[i];

	if ( checkSumm != outData[dataLen - 1] ) return 0; 

	nameLen = (uchar)outData[0];
	codeLen = (uchar)outData[1];

	if ( nameLen >= nameSize || codeLen >= codeSize )
		return 0;

	strncpy( outName, &outData[2], nameLen );
	outName[nameLen] = 0;
	
	strncpy( outCode, &outData[2 + nameLen], codeLen );
	outCode[codeLen] = 0;
	
	return 1;
}

//---------------------------------------------------------
// Reg_CryptStr: 
//---------------------------------------------------------
int Reg_CryptStr( const char* str, char outData[MAX_CRYPT_LEN*2] )
{
	int	i;
	int	len = strlen( str );

	if ( len > MAX_CRYPT_LEN ) return 0;

	for ( i = len - 1; i >= 0; i-- )
	{
		char	ch = str[i];
		outData[(i*2) + 1] = ((ch & 15) + (i & 7) + 'b');
		outData[(i*2)] = ((ch >> 4 ) + (i & 7) + 'c');
	}

	outData[len*2] = 0;

	return 1;
}

//---------------------------------------------------------
// Reg_DecryptStr: 
//---------------------------------------------------------
void Reg_DecryptStr( const char* data, char *outStr )
{
	int	i;
	int	len = strlen( data ) / 2;
	
	for ( i = 0; i < len; ++i )
	{
		char ch = (data[(i*2)] - (i & 7) - 'c') * 16 +
			(data[(i*2) + 1] - (i & 7) - 'b');
		outStr[i] = ch;
	}
	outStr[i] = 0;
}

