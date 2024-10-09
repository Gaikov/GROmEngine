//
// Created by Roman on 5/8/2024.
//

#ifndef _GROM_CRYPT_H
#define _GROM_CRYPT_H

class nsCrypt {
public:
   static void XorEncode(void *data, unsigned int length, const char* key);
};

#endif //_GROM_CRYPT_H
