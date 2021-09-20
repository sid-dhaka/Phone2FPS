#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int iResult = 0;

SOCKET setupServer(){
    WSADATA wsaData;

    SOCKET RecvSocket;
    struct sockaddr_in RecvAddr;

    unsigned short Port = 50001;

    //-----------------------------------------------
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup failed with error %d\n", iResult);
        WSACleanup();
        return 1;
    }
    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
    RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (RecvSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error %d\n", WSAGetLastError());
        WSACleanup();   
        return 1;
    }

    // timeval timeout;
    // timeout.tv_sec = 10;
    // timeout.tv_usec = 0;

    // if (setsockopt(RecvSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0){
    //     wprintf(L"setsockopt faield %d\n", WSAGetLastError());
    //     WSACleanup();
    //     return 1;
    // }

    //-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(Port);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    iResult = bind(RecvSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr));
    if (iResult != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    return RecvSocket;
}

bool getData(SOCKET RecvSocket, char* RecvBuf, int BufLen, sockaddr_in* SenderAddr, int* SenderAddrSize){
    //-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.
    iResult = recvfrom(RecvSocket,
                    RecvBuf, BufLen, 0, (SOCKADDR *) SenderAddr, SenderAddrSize);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return false;
    }
    return true;
}

void closeServer(SOCKET RecvSocket){
    //-----------------------------------------------
    // Close the socket when finished receiving datagrams
    wprintf(L"Finished receiving. Closing socket.\n");
    iResult = closesocket(RecvSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
    }
    //-----------------------------------------------
    // Clean up and exit.
    wprintf(L"Exiting.\n");
    WSACleanup();
}