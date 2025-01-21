#include "./include/Client.h"
int main() 
{
    ABXClient client("127.0.0.1", 3000);
    client.requestStreamAllPackets();          
    client.handleMissingSequences();    
    client.saveToJSON("..\\logs\\output.json");
    return 0;
}
