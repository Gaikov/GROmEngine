//
// Created by roman on 7/2/25.
//

#include "Net.h"
#include "nsLib/log.h"

bool nsNet::Init() {
    Log::Info("...Network initialising");
    return true;
}

void nsNet::Release() {
    Log::Info("...Network releasing");
}

int nsNet::GetLastError() {
    return errno;
}

const char* nsNet::GetErrorMessage(int code) {
    return strerror(code);
}

void nsNet::CloseSocket(int socket) {
    shutdown(socket, SHUT_RDWR);
    NET_ERROR("Error to shutdown socket",)
    close(socket);
    NET_ERROR("Error to close socket",)
}
