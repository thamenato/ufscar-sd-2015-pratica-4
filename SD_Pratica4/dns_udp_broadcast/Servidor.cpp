#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "AppObjects.h"
#include "../my_socket.h"
#pragma comment (lib, "Ws2_32.lib")

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

DWORD WINAPI thread_Servidor(LPVOID lpParameter);

DWORD WINAPI thread_Servidor(LPVOID lpParameter)
{
	// Create a WSADATA object called wsaData.
	WSADATA wsaData;
	int iResult;
	SOCKET Server_Socket = INVALID_SOCKET;
	int count_socket=0;
	// Declare an addrinfo object that contains a sockaddr structure
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int recvbuflen = DEFAULT_BUFLEN;
	
	struct sockaddr_storage their_addr;
	size_t addr_len;
	char s[INET6_ADDRSTRLEN];
	
	int flag_on = 1;              /* socket option flag */
	
	message msg1;
	
	
	printf("Sistemas Distribuidos: Servidor de Nomes inicializando ...\n\n");
	
	SocketParams *params;
	params = (SocketParams *)lpParameter;
	
	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	         {
	          printf("WSAStartup failed with error: %d\n", iResult);
	          return 1;
		        }
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = params->family;
	hints.ai_socktype = params->socktype;
	hints.ai_protocol = params->protocol;
	hints.ai_flags = params->flags;
	
	/* Resolve the server address and port */
	iResult = getaddrinfo(NULL, params->port, &hints, &result);
	
	if ( iResult != 0 ) {
		           printf("getaddrinfo failed with error: %d\n", iResult);
		           WSACleanup();
		           return 1;
	                     	}
	/* Create a SOCKET 
	   After initialization, a SOCKET object must be instantiated for use by the server */
	Server_Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Server_Socket == INVALID_SOCKET) {
		            printf("socket failed with error: %ld\n", WSAGetLastError());
		            freeaddrinfo(result);
		            WSACleanup();
		            return 1;
		                          }
	
	/* set reuse port to on to allow multiple binds per host */
	if ((setsockopt(Server_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag_on,
	       sizeof(flag_on))) < 0) {
	    perror("setsockopt() failed");
		exit(1);
	  }
	/* Setup the UDP listening socket */
	iResult = bind(Server_Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		            printf("bind failed with error: %d\n", WSAGetLastError());
		            freeaddrinfo(result);
	            	closesocket(Server_Socket);
		            WSACleanup();
		            return 1;
	                        	}
	
	while(true)
	{
		printf("listener: waiting to recvfrom...\n");
		addr_len = sizeof their_addr;
		
		iResult = recvfrom(Server_Socket, (char *)&msg1, (int)sizeof(msg1), 0, (struct sockaddr *)&their_addr, (int *) &addr_len);
		// Check for error 
		if (iResult==-1) { printf("Erro-Close\n\n"); 
		                   break;
		}
		
		switch (msg1.opcode)
		{
			case 0:		// registro
				printf("Listener received-nome:  %s\n", msg1.name);
				printf("Listener received-message: %s\n", msg1.buf);
				printf("Listener: got packet from %s\n", inet_ntop(their_addr.ss_family,
					get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

				break;
			case 1:		// de-registro
				printf("Listener received-nome:  %s\n", msg1.name);
				printf("Listener received-message: %s\n", msg1.buf);
				printf("Listener: got packet from %s\n", inet_ntop(their_addr.ss_family,
					get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

				break;
			case 2:		// consulta
				printf("Listener received-nome:  %s\n", msg1.name);
				printf("Listener received-message: %s\n", msg1.buf);
				printf("Listener: got packet from %s\n", inet_ntop(their_addr.ss_family,
					get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

				break;
		}
		
		Sleep(10);
	}
	closesocket(Server_Socket);
	return 0;
}