//
// Created by roman on 7/2/25.
//

struct WinsockError {
    int code;
    const char* name;
    const char* description;
};

WinsockError WinsockErrorCodes[] = {
    { 6, "WSA_INVALID_HANDLE", "Specified event object handle is invalid." },
    { 8, "WSA_NOT_ENOUGH_MEMORY", "Insufficient memory available." },
    { 87, "WSA_INVALID_PARAMETER", "One or more parameters are invalid." },
    { 995, "WSA_OPERATION_ABORTED", "Overlapped operation aborted." },
    { 996, "WSA_IO_INCOMPLETE", "Overlapped I/O event object not in signaled state." },
    { 997, "WSA_IO_PENDING", "Overlapped operations will complete later." },
    { 10004, "WSAEINTR", "Interrupted function call." },
    { 10009, "WSAEBADF", "File handle is not valid." },
    { 10013, "WSAEACCES", "Permission denied." },
    { 10014, "WSAEFAULT", "Bad address." },
    { 10022, "WSAEINVAL", "Invalid argument." },
    { 10024, "WSAEMFILE", "Too many open files." },
    { 10035, "WSAEWOULDBLOCK", "Resource temporarily unavailable." },
    { 10036, "WSAEINPROGRESS", "Operation now in progress." },
    { 10037, "WSAEALREADY", "Operation already in progress." },
    { 10038, "WSAENOTSOCK", "Socket operation on nonsocket." },
    { 10039, "WSAEDESTADDRREQ", "Destination address required." },
    { 10040, "WSAEMSGSIZE", "Message too long." },
    { 10041, "WSAEPROTOTYPE", "Protocol wrong type for socket." },
    { 10042, "WSAENOPROTOOPT", "Bad protocol option." },
    { 10043, "WSAEPROTONOSUPPORT", "Protocol not supported." },
    { 10044, "WSAESOCKTNOSUPPORT", "Socket type not supported." },
    { 10045, "WSAEOPNOTSUPP", "Operation not supported." },
    { 10046, "WSAEPFNOSUPPORT", "Protocol family not supported." },
    { 10047, "WSAEAFNOSUPPORT", "Address family not supported by protocol family." },
    { 10048, "WSAEADDRINUSE", "Address already in use." },
    { 10049, "WSAEADDRNOTAVAIL", "Cannot assign requested address." },
    { 10050, "WSAENETDOWN", "Network is down." },
    { 10051, "WSAENETUNREACH", "Network is unreachable." },
    { 10052, "WSAENETRESET", "Network dropped connection on reset." },
    { 10053, "WSAECONNABORTED", "Software caused connection abort." },
    { 10054, "WSAECONNRESET", "Connection reset by peer." },
    { 10055, "WSAENOBUFS", "No buffer space available." },
    { 10056, "WSAEISCONN", "Socket is already connected." },
    { 10057, "WSAENOTCONN", "Socket is not connected." },
    { 10058, "WSAESHUTDOWN", "Cannot send after socket shutdown." },
    { 10059, "WSAETOOMANYREFS", "Too many references." },
    { 10060, "WSAETIMEDOUT", "Connection timed out." },
    { 10061, "WSAECONNREFUSED", "Connection refused." },
    { 10062, "WSAELOOP", "Cannot translate name." },
    { 10063, "WSAENAMETOOLONG", "Name too long." },
    { 10064, "WSAEHOSTDOWN", "Host is down." },
    { 10065, "WSAEHOSTUNREACH", "No route to host." },
    { 10066, "WSAENOTEMPTY", "Directory not empty." },
    { 10067, "WSAEPROCLIM", "Too many processes." },
    { 10068, "WSAEUSERS", "User quota exceeded." },
    { 10069, "WSAEDQUOT", "Disk quota exceeded." },
    { 10070, "WSAESTALE", "Stale file handle reference." },
    { 10071, "WSAEREMOTE", "Item is remote." },
    { 10091, "WSASYSNOTREADY", "Network subsystem is unavailable." },
    { 10092, "WSAVERNOTSUPPORTED", "Winsock.dll version out of range." },
    { 10093, "WSANOTINITIALISED", "Successful WSAStartup not yet performed." },
    { 10101, "WSAEDISCON", "Graceful shutdown in progress." },
    { 10102, "WSAENOMORE", "No more results." },
    { 10103, "WSAECANCELLED", "Call has been canceled." },
    { 10104, "WSAEINVALIDPROCTABLE", "Procedure call table is invalid." },
    { 10105, "WSAEINVALIDPROVIDER", "Service provider is invalid." },
    { 10106, "WSAEPROVIDERFAILEDINIT", "Service provider failed to initialize." },
    { 10107, "WSASYSCALLFAILURE", "System call failure." },
    { 10108, "WSASERVICE_NOT_FOUND", "Service not found." },
    { 10109, "WSATYPE_NOT_FOUND", "Class type not found." },
    { 10110, "WSA_E_NO_MORE", "No more results." },
    { 10111, "WSA_E_CANCELLED", "Call was canceled." },
    { 10112, "WSAEREFUSED", "Database query was refused." }
};

bool nsNet::Init() {
    Log::Info("...Network initialising");

    WSADATA wsaData = {};
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    NET_ERROR("WSAStartup", false);

    Log::Info("Net desc: %s", wsaData.szDescription);
    Log::Info("Net status: %s", wsaData.szSystemStatus);

    return true;
}

void nsNet::Release() {
    Log::Info("...Network releasing");
    WSACleanup();
}

int nsNet::GetLastError() {
    return WSAGetLastError();
}

const char* nsNet::GetErrorMessage(int code) {
    if (!code) return "No error.";
    for (const auto & error : WinsockErrorCodes) {
        if (error.code == code) {
            return error.description;
        }
    }
    return "Unknown error code.";
}