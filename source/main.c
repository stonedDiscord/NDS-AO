/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>

void startAO(char* url);
	
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	consoleDemoInit();  //setup the sub screen for printing

	iprintf("\tAttorney Online Demo\n");
	iprintf("Connecting via WFC data ...\n");

	if(!Wifi_InitDefault(WFC_CONNECT)) {
		iprintf("Failed to connect!");
	} else {

		iprintf("\x1b[32mConnected to WiFi\x1b[39m\n\n");

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
    iprintf("Connecting...\n");

    // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
    struct sockaddr_in sain;
    sain.sin_family = AF_INET;
    sain.sin_port = htons(port);
    sain.sin_addr.s_addr= *( (unsigned long *)(myhost->h_addr_list[0]) );
    connect( my_socket,(struct sockaddr *)&sain, sizeof(sain) );
    iprintf("Connected to server %s on port %d\n",ipv4,port);

    // send our request
    send( my_socket, request_text, strlen(request_text), 0 );
    iprintf("Sent HI!\n");

    int recvd_len;
    char incoming_buffer[256];
    char *message_color = "\x1b[39m";
    char *message_character;
    char *message_text;
    char *music_track;

    while( ( recvd_len = recv( my_socket, incoming_buffer, 255, 0 ) ) != 0 ) { // if recv returns 0, the socket has been closed.
        if(recvd_len>0) { // data was received!
            incoming_buffer[recvd_len] = 0; // null-terminate

            int i = 0;
            char *p = strtok (incoming_buffer, "#");
            char *parameter_array[20];

            while (p != NULL)
            {
                parameter_array[i++] = p;
                p = strtok (NULL, "#");
            }

            switch(parameter_array[0][0]){
                // M
                case 77: switch(parameter_array[0][1]){
                            // S
                            case 83:
                                message_character = parameter_array[3];
                                message_text = parameter_array[5];
                                switch(strtol(parameter_array[15][0],&end,10)){
                                    case 1:
                                        message_color = "\x1b[32m";
                                        break;
                                    case 2:
                                        message_color = "\x1b[31m";
                                        break;
                                    case 3:
                                        message_color = "\x1b[33m";
                                        break;
                                    case 4:
                                        message_color = "\x1b[34m";
                                        break;
                                    case 5:
                                        message_color = "\x1b[33m";
                                        break;
                                    case 0:
                                    default:
                                        message_color = "\x1b[39m";
                                        break;
                                }
                                iprintf("\x1b[39m<%s> %s%s\n",message_character,message_color,message_text);
                                break;
                            // C
                            case 67:
                            music_track = parameter_array[1];
                                iprintf("Played a track: %s\n",music_track);
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

