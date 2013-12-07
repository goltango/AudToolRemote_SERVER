/******** AUDTOOL REMOTE *********************
 A simple Audacious remote control based on audtool.
 Server side.
 **************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void conexHandler(int _value);
void executeOption(int _option, char *_stdOut);
int splitString(char * _str[], char * _argv[20]);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             conexHandler(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     }
     close(sockfd);
     return 0;
}

/******** conexHandler() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 Tiene como finalidad crear una instancia separada
 de esta funcion por cada conexion. Esta maneja toda la
 comunicacion una vez que la conexion fue establecida.
 *****************************************/
void conexHandler(int sock)
{
    int n,op;
    char buffer[256];
    FILE *lsofFile_p;
    char *line_p;
    char buffer2[256];

    char * arguments[20];
    int i=0;

    bzero(buffer,256);
    n = read(sock,buffer,255);
    if (n < 0) error("ERROR reading from socket");

//    printf ("Splitting string [%s] into tokens:\n",buffer);

    arguments[0] = strtok(buffer, " \n");
	for(i = 1; i < 20; i++){
		arguments[i] = strtok(NULL, " \n");
		if (arguments[i] == NULL)
			break;
	}

    executeOption(atoi(arguments[0]),buffer2);

    n = write(sock,buffer2,strlen(buffer2));
    if (n < 0) error("ERROR writing to socket");
}

/******** executeOption() *********************
 Ejecuta en el comando audtool en la consola del sistema
 segun que opcion se ingrese como parametro. Ademas almacena
 en un buffer, el stdout devuelto por el comando ejecutado.
 *****************************************/
void executeOption(int _option, char *_stdOut){
    FILE *lsofFile_p;
    char *line_p;
    char buffer[256];

    switch(_option){
        /** Playback manipulation. **/
        case 0:
            lsofFile_p = popen("audtool --playback-play", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            break;
        case 1:
            lsofFile_p = popen("audtool --playback-pause", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            break;
        case 2:
            lsofFile_p = popen("audtool --playback-playpause", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            break;
        case 3:
            lsofFile_p = popen("audtool --playback-stop", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            break;
        case 4:
            lsofFile_p = popen("audtool --playback-playing", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
        case 5:
            lsofFile_p = popen("audtool --playback-paused", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 6:
            lsofFile_p = popen("audtool --playback-stopped", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 7:
            lsofFile_p = popen("audtool --playback-status", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 8:
            /**Stuff to do**/
            break;
        case 9:
            /**Stuff to do**/
            break;
        /** Volume control. **/
        case 10:
            lsofFile_p = popen("audtool --get-volume", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 11:
            lsofFile_p = popen("audtool --set-volume <level> ", "r");   //Needs codification
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        /** Now playing information. **/
        case 12:
            lsofFile_p = popen("audtool --current-song", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 13:
            lsofFile_p = popen("audtool --current-song-filename", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 14:
            lsofFile_p = popen("audtool --current-song-length-seconds", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 15:
            lsofFile_p = popen("audtool --current-song-length-frames", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 16:
            lsofFile_p = popen("audtool --current-song-output-length", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 17:
            lsofFile_p = popen("audtool --current-song-output-length-seconds", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 18:
            lsofFile_p = popen("audtool --current-song-output-length-frames", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 19:
            lsofFile_p = popen("audtool --current-song-bitrate", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 20:
            lsofFile_p = popen("audtool --current-song-bitrate-kbps", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 21:
            lsofFile_p = popen("audtool --current-song-frequency", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 22:
            lsofFile_p = popen("audtool --current-song-frequency-khz", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;
        case 23:
            lsofFile_p = popen("audtool --current-song-channels", "r");
            line_p = fgets(buffer, sizeof(buffer), lsofFile_p);
            pclose(lsofFile_p);
            strcpy(_stdOut,buffer);
            break;

        default:
            printf("DEFAULT\n");
            break;
    }
}
