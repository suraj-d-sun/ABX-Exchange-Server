#include "../include/Client.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <winsock2.h>
#include <../nlohmann/json.hpp>

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;

ABXClient::ABXClient(const string& ip, int port) : serverIP(ip), serverPort(port) {}

void ABXClient::requestStreamAllPackets() 
{
    SOCKET sock;
    sockaddr_in server;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        cerr << "WSAStartup failed." << endl;
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    {
        cerr << "Socket creation failed." << endl;
        WSACleanup();
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);
    server.sin_addr.s_addr = inet_addr(serverIP.c_str());

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) 
    {
        cerr << "Connection failed." << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    char payload[2] = {1, 0}; // callType = 1, resendSeq = 0
    send(sock, payload, sizeof(payload), 0);

    char buffer[1024];
    while (int bytesRead = recv(sock, buffer, sizeof(buffer), 0)) 
    {
        if (bytesRead > 0)
        {
            vector<char> data(buffer, buffer + bytesRead);
            Packet packet = parsePacket(data);
            packets.push_back(packet);
            sequenceTracker[packet.sequenceNumber] = true;
        }
        else 
        {
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
}

void ABXClient::requestResendPacket(int sequenceNumber) 
{
    SOCKET sock;
    sockaddr_in server;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        cerr << "WSAStartup failed." << endl;
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
    {
        cerr << "Socket creation failed." << endl;
        WSACleanup();
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);
    server.sin_addr.s_addr = inet_addr(serverIP.c_str());

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) 
    {
        cerr << "Connection failed." << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    char payload[2] = {2, static_cast<char>(sequenceNumber)};
    send(sock, payload, sizeof(payload), 0);

    char buffer[1024];
    if (int bytesRead = recv(sock, buffer, sizeof(buffer), 0)) 
    {
        vector<char> data(buffer, buffer + bytesRead);
        Packet packet = parsePacket(data);
        packets.push_back(packet);
        sequenceTracker[packet.sequenceNumber] = true;
    }

    closesocket(sock);
    WSACleanup();
}

Packet ABXClient::parsePacket(const vector<char>& buffer) 
{
    Packet packet;

    packet.symbol = string(buffer.begin(), buffer.begin() + 4);
    packet.buySellIndicator = buffer[4];
    packet.quantity = (buffer[5] << 24) | (buffer[6] << 16) | (buffer[7] << 8) | buffer[8];
    packet.price = (buffer[9] << 24) | (buffer[10] << 16) | (buffer[11] << 8) | buffer[12];
    packet.sequenceNumber = (buffer[13] << 24) | (buffer[14] << 16) | (buffer[15] << 8) | buffer[16];

    return packet;
}

void ABXClient::handleMissingSequences() 
{
    cout << "Checking for missing sequences..." << endl;
    int maxSequence = 0;
    for (const auto& packet : packets) 
    {
        maxSequence = max(maxSequence, packet.sequenceNumber);
    }

    for (int seq = 1; seq <= maxSequence; ++seq) 
    {
        if (sequenceTracker.find(seq) == sequenceTracker.end() || !sequenceTracker[seq]) 
        {
            cout << "Missing sequence found: " << seq << ". Requesting resend..." << endl;
            requestResendPacket(seq);
        }
    }
    cout << "Missing sequences have been handled." << endl;
}

void ABXClient::saveToJSON(const string& filePath) 
{
    json output = json::array();
    for (const auto& packet : packets) 
    {
        output.push_back({
            {"symbol", packet.symbol},
            {"buySellIndicator", string(1, packet.buySellIndicator)},
            {"quantity", packet.quantity},
            {"price", packet.price},
            {"sequenceNumber", packet.sequenceNumber}
        });
    }

    ofstream file(filePath);
    if (file.is_open()) 
    {
        file << output.dump(4);
        file.close();
        cout << "Data saved to JSON file at: " << filePath << endl;
    }
    else 
    {
        cerr << "Failed to open JSON file for saving: " << filePath << endl;
    }
}

