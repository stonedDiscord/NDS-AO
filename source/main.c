/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>

void getHttp(char* url);
	
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	consoleDemoInit();  //setup the sub screen for printing

	iprintf("\tAttorney Online Demo\n");
	iprintf("Connecting via WFC data ...\n");

	if(!Wifi_InitDefault(WFC_CONNECT)) {
		iprintf("Failed to connect!");
	} else {

		iprintf("Connected\n\n");

		startAO("104.131.93.82:27017");	
        
    }	
	
	while(1) {
		swiWaitForVBlank();
        int keys = keysDown();
        if(keys & KEY_START) break;
	}

	return 0;
}

//---------------------------------------------------------------------------------
void startAO(char* url) {
//---------------------------------------------------------------------------------
    // seperate port and ip

    char *ipv4, *s_port;
    int port;
    char* seperator = ":";
    ipv4 = strtok(url, seperator);
    s_port = strtok(NULL, ":");
    char *end;
    port = strtol(s_port,&end,10);

    // the request for later
    const char * request_text = 
        "HI#NDSserial#%ID#NDS-AO#0.0.1#%";

    // Find the IP address of the server, with gethostbyname
    struct hostent * myhost = gethostbyname( ipv4 );
    iprintf("Found Server!\n");
 
    // Create a TCP socket
    int my_socket;
    my_socket = socket( AF_INET, SOCK_STREAM, 0 );
    iprintf("Created Socket!\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(port);
    sain.sin_addr.s_addr= *( (unsigned long *)(myhost->h_addr_list[0]) );
    connect( my_socket,(struct sockaddr *)&sain, sizeof(sain) );
    iprintf("Connected to server! %s on port %d\n",ipv4,port);

    // send our request
    send( my_socket, request_text, strlen(request_text), 0 );
    iprintf("Sent our request!\n");

    // Print incoming data
    iprintf("Printing incoming data:\n");

    int recvd_len;
    char incoming_buffer[256];

    char *message_character;
    char *message_text;
    char *music_track;

    while( ( recvd_len = recv( my_socket, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
        if(recvd_len>0) { // data was received!
            incoming_buffer[recvd_len] = 0; // null-terminate
            switch(incoming_buffer[0]){
                // M
                case 77: switch(incoming_buffer[1]){
                            // S
                            case 83:
                                    break;
                            // C
                            case 67:
                                    break;
                        }
                        break;

                default: iprintf("%s\n",incoming_buffer);
                        break;
            }
		}
	}

	iprintf("Other side closed connection!");

	shutdown(my_socket,0); // good practice to shutdown the socket.

	closesocket(my_socket); // remove the socket.
}

