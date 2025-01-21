#pragma once
#include <string>
#include <vector>
#include <map>
using namespace std;

struct Packet 
{
    string symbol;
    char buySellIndicator;
    int quantity;
    int price;
    int sequenceNumber;
};

class ABXClient 
{
private:
    string serverIP;
    int serverPort;
    vector<Packet> packets;
    map<int, bool> sequenceTracker;

    void requestResendPacket(int sequenceNumber);
    Packet parsePacket(const vector<char>& buffer);

public:
    ABXClient(const string& ip, int port);
    void requestStreamAllPackets();
    void handleMissingSequences();
    void saveToJSON(const string& filePath);
};

