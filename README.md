# ABX Mock Exchange Client

## **Introduction**
This project implements a C++ client that interacts with the ABX Mock Exchange Server, simulating a stock exchange environment. The server provides data packets containing stock order information, and the client is designed to:

1. Stream all available packets from the server.
2. Handle missing packets by requesting specific sequences.
3. Parse the received packets.
4. Save the parsed data into a structured JSON file.

The project uses:
- **Node.js** for the mock server.
- **C++** for the client implementation.
- **nlohmann/json** library for JSON handling in C++.

---

## **Project Folder Structure**

```plaintext
ABX-Exchange-Server/
├── ABX_Server/           # Contains the mock exchange server script
│   └── main.js          
├── Client_C++/           # Contains the C++ client application
│   ├── include/          # Header files
│   │   └── Client.h
│   │   └── nlohmann
│   ├── src/              # Source files
│   │   └── Client.cpp
│   ├── main.cpp          # Entry point for the C++ client
│   └── logs/             # Folder for output JSON
├── README.md             # Project documentation
```

---

## **Setup Instructions**

### Prerequisites
Ensure the following are installed on your system:
1. **Node.js** (version 16.17.0 or higher)
2. **C++ Compiler** (e.g., GCC, MinGW for Windows)
3. **Git** (to clone the repository)

---

### Clone the Repository

1. Clone the repository to your local system:
   ```bash
   git clone <repo-url>
   cd ABX-Exchange-Server
   ```

2. Navigate to the server and client directories as needed:
   ```bash
   cd ABX_Server
   ```

---

### Setting Up the Node.js Server

1. Navigate to the `ABX_Server` folder:
   ```bash
   cd ABX-Exchange-Server/ABX_Server
   ```

2. Run the server:
   ```bash
   node main.js
   ```

   The server will start and listen on port `3000` for incoming connections.

---

### Setting Up the C++ Client

1. Ensure the JSON library (`json.hpp`) is placed in the `nlohmann/` directory.
   - Path: `ABX-Exchange-Server/Client_C++/include/nlohmann/json.hpp`
   - Download from: [https://github.com/nlohmann/json](https://github.com/nlohmann/json)

2. Compile the C++ client:
   ``` 
    > cd ABX-Exchange-Server/Client_C++/src
    > g++ -c Client.cpp -I ..\include\nlohmann -I ..\include -o Client.o -std=c+ +17 -lws2_32
    > ar rcs libClient.lib *.o
    > move libClient.lib ..\lib
    ```

3. Run the client:
   ```
   > g++ main.cpp -I .\include -I .\include\nlohmann -L .\lib -llibClient -o main.exe -lws2_32
   > main

   ```

---

### Outputs

1. **Console Logs:**
   - Logs for received packets, missing sequences, and JSON output status will be displayed in the console.

2. **JSON File:**
   - The output file will be saved in the `logs` directory:
     ```plaintext
     ABX-Exchange-Server/Client_C++/logs/output.json
     ```
   - Example JSON structure:
     ```json
     [
         {
             "symbol": "MSFT",
             "buySellIndicator": "B",
             "quantity": 50,
             "price": 100,
             "sequenceNumber": 1
         },
         {
             "symbol": "AAPL",
             "buySellIndicator": "S",
             "quantity": 30,
             "price": 55,
             "sequenceNumber": 6
         }
     ]
     ```

---
