#ifndef	_RegCrypt_H_
#define	_RegCrypt_H_

#define MAX_CRYPT_LEN	1024

#ifdef __cplusplus
extern "C"
{
#endif

int		Reg_CryptInfo( const char* name, const char* code, char outData[MAX_CRYPT_LEN*2] );
int		Reg_DecryptInfo( const char *data, char* outName, int nameSize, char* outCode, int codeSize );

int		Reg_CryptStr( const char* str, char outData[MAX_CRYPT_LEN*2] );
void	Reg_DecryptStr( const char* data, char *outStr );

#ifdef __cplusplus
}
#endif

#endif	//_RegCrypt_H_