# Chat-Application-using-Socket-Programming

# Introduction
This project is a chat application built using socket programming in C. The application demonstrates key concepts in computer networks, operating systems, and multithreading, while also incorporating custom library functions. Through this project, I have gained hands-on experience with client-server architecture, thread management, and encryption techniques.

# Features
* **_Client-Server Model_ :** Implements a basic client-server architecture where multiple clients can connect to a single server and communicate with each other.
* **_Multithreading_ :** The server uses threads to handle multiple clients simultaneously, ensuring that the server can manage several connections concurrently.
* **_User Authentication_ :** Basic user registration and login functionality using username and password.
* **_Message Encryption_ :** XOR-based encryption/decryption for secure message transmission between the client and server.
* **_Custom Library Functions_ :** Custom utility functions for encryption, message handling and user authentication.

# Concepts Explored
* **Computer Networks**
  * **_Socket Programming_ :** Implemented TCP sockets for reliable communication between the client and server.
  * **_Client-Server Architecture_ :** Understood how clients and servers interact, including connection establishment and data exchange.
  * **_Data Transmission_ :** Explored how data is sent and received over the network using send and receive functions.
  
* **Operating Systems**
  * **_Thread Management_ :** Used POSIX threads (pthreads) to enable concurrent handling of multiple clients on the server side.
  * **_Synchronization_ :** Implemented mutexes to ensure thread-safe access to shared resources, preventing race conditions.
  * **_File Handling_ :** Managed user data (e.g., usernames and passwords) through file operations in C.
  
* **Custom Library**
  * **_XOR Encryption_ :** Created a custom library for XOR-based encryption and decryption to secure communication.
  * **_Server_Functions_:** Created a custom library that stores all the server functions.
  * **_Client_Functions_:** Created a custom library that stores all the client functions including user authentication.
  

# How to Run
* **Prerequisites**
  * C compiler (e.g., GCC)
  * POSIX-compliant system (e.g., Linux, macOS)

* **Steps**
1. Clone the repository:
    ```
   git clone https://github.com/Shriyaseth25/Chat-Application-using-Socket-Programming.git
    ```
2. Navigate to Project Directory:
   ```
   cd Chat-Application-using-Socket-Programming
   ```
3. Open one Terminal:
   ```
   gcc -o server_executable Server/server.c Server/server_header.c Encryption/encryption.c -lpthread
   ./server_executable
   ```

4. Open another Terminal:
   ```
   gcc -o client_executable Client/client.c Client/client_header.c Encryption/encryption.c -lpthread
   ./client_executable
   ```

* **You may open as many client terminals as you want, to chat with multiple clients. For that you need to run only the executable file.**

# Usage

* **_Register_ :** When you start the client, register by providing a username and password.
* **_Login_ :** Log in using your credentials.
* **_Chat_ :** Send and receive encrypted messages with other connected clients.
  
# File Structure
* **_server.c_ :** Server-side implementation handling multiple clients.
* **_client.c_ :** Client-side implementation for connecting to the server and sending/receiving messages.
* **encryption.h:** Custom header file for XOR encryption and decryption.
* **server_header.h:** Custom header file for server-side functions.
* **client_header.h:** Custom header file for client-side functions.

# Acknowledgments
This project helped me solidify my understanding of several core concepts in computer science, including networking, operating systems, and multithreading. Special thanks to the various online communities and documentation that provided invaluable guidance.

# Future Enhancements
* **_Graphical User Interface (GUI)_ :** Implement a user-friendly interface for the chat application.
* **_Database Integration_ :** Store user data in a database instead of a file.

# Snapshot
![Screenshot 2024-09-06 195101](https://github.com/user-attachments/assets/33c65447-5678-4b17-bc10-04d582af1f05)

