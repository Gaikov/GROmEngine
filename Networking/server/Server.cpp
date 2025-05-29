//
// Created by Roman on 5/29/2025.
//

#include "Server.h"
#include "nsLib/log.h"
#include "Networking/Net.h"

nsServer::nsServer(int port) : _port(port) {
}

nsServer::~nsServer() {
    Stop();
    Log::Info("Server stopped!");
}

bool nsServer::Start() {
    if (!_socket.Listen(_port)) {
        return false;
    }

    _clientsThread = std::thread([this]() {
        auto client = _socket.Accept();
    });
    Log::Info("Server started");
    _isRunning = true;
    return true;
}

void nsServer::Stop() {
    if (_isRunning) {
        Log::Info("Stopping server");

        _isRunning = false;
        _socket.Close();
        _clientsThread.join();
    }
}

