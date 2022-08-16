#include "clientGame.h"

void sendMessageToServer (int socketServer, tString message){	//FUNCION REVISADA
	unsigned int msglength;
	unsigned int tam = strlen(message); 
	
	// Send player's message length to the server
	msglength = send(socketServer, &tam, sizeof(unsigned int), 0);
	
	//check send bytes
	if (msglength < 0)
		showError("ERROR while writting TAM to the socket");
		
	// Send player's message to the server
	msglength = send(socketServer, message, tam, 0);
	
	//check send bytes
	if(msglength < 0)
		showError("ERROR while writting MESSAGE to the socket");
}

void receiveMessageFromServer (int socketServer, tString message){ //FUNCION REVISADA
	unsigned int msglength, tam;

	memset(message, 0, STRING_LENGTH);
	//Receive the message length
	msglength = recv(socketServer, &tam, sizeof(unsigned int), 0);	//Recive the message length
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading TAM  from socket");
	
	//Receive the text message	
	msglength = recv(socketServer, message, tam, 0);	
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading MESSAGE from socket");
}

void receiveBoard (int socketServer, tBoard board){ //FUNCION REVISADA
	unsigned int msglength, tam;
		
	//Receive the board
	memset(board, 0, 42);	
	msglength = recv(socketServer, board, 42, 0);
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading BOARD from socket");

}

unsigned int receiveCode (int socketServer){ //FUNCION REVISADA
	unsigned int code, msglength;
	
	//Receive the code
	msglength = recv(socketServer, &code, sizeof(unsigned int), 0);
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading CODE from socket");
	
	return code;
}

unsigned int readMove (){

	tString enteredMove;
	unsigned int move;
	unsigned int isRightMove;


		// Init...
		isRightMove = FALSE;
		move = STRING_LENGTH;

		while (!isRightMove){

			printf ("Enter a move [0-6]:");

			// Read move
			fgets (enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Length of entered move is not correct
			if (strlen(enteredMove) != 1){
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
			}

			// Check if entered move is a number
			else if (isdigit(enteredMove[0])){

				// Convert move to an int
				move =  enteredMove[0] - '0';

				if (move > 6)
					printf ("Entered move is not correct. It must be a number between [0-6]\n");
				else
					isRightMove = TRUE;
			}

			// Entered move is not a number
			else
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
		}

	return move;
}

void sendMoveToServer (int socketServer, unsigned int move){ //FUNCION REVISADA
	unsigned int msglength;
	
	// Send player's move to the server
	msglength = send(socketServer, &move, sizeof(unsigned int), 0);
	
	//check send bytes
	if (msglength < 0)
		showError("ERROR while writting MOVE to the socket");
	
}



int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;			/** Server address structure */
	char* serverIP;						/** Server IP */

	tBoard board;						/** Board to be displayed */
	tString playerName;					/** Name of the player */
	tString rivalName;					/** Name of the rival */
	tString message;					/** Message received from server */
	unsigned int column;					/** Selected column */
	unsigned int code;					/** Code sent/receive to/from server */
	unsigned int endOfGame;					/** Flag to control the end of the game */
	unsigned int msglength;					/** Message lentgth */


		// Check arguments!
		if (argc != 3){
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
			exit(0);
		}

		// Get the server address
		serverIP = argv[1];

		// Get the port
		port = atoi(argv[2]);
	

		// Create socket
		socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// Check if the socket has been successfully created
		if (socketfd < 0)
			showError("ERROR while creating the socket");

		// Fill server address structure
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(serverIP);
		server_address.sin_port = htons(port);
		
		// Connect with server
		if(connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address))<0) //¿?
			showError("ERROR while stablishing connection");
			
		printf ("Connection established with server!\n");

		// Init player's name
		do{
			memset(playerName, 0, STRING_LENGTH);
			printf ("Enter player name:");
			fgets(playerName, STRING_LENGTH-1, stdin);

			// Remove '\n'
			playerName[strlen(playerName)-1] = 0;

		}while (strlen(playerName) <= 2);

		//Send player name to the server.
		sendMessageToServer(socketfd, playerName);

		// Receive rival's name
		receiveMessageFromServer (socketfd,&rivalName);
		
		if(msglength < 0)
			showError("ERROR while writting to the socket");

		printf ("You are playing against %s\n", rivalName);

		// Init
		endOfGame = FALSE;

		// Game starts
		printf ("Game starts!\n\n");
		
		// While game continues...
		while(!endOfGame){
			
			//Receive code
			code = receiveCode (socketfd);
	
			//Receive information message.
			receiveMessageFromServer (socketfd, message);

			//Receibe and print the board and info message
			receiveBoard (socketfd, board);
			printBoard(board, message);	
			
			//If client have the turn
			if(code == TURN_MOVE){	
				printf("Enter a move [0-6]:\n");
				scanf("%d", &column);
				//Send the client move to the server.
				sendMoveToServer (socketfd, column);	
			}
			//If client the match finish
			else if(code == GAMEOVER_WIN || code == GAMEOVER_LOSE || code == GAMEOVER_DRAW){
				endOfGame = TRUE;
			}		
		}
	
	// Close socket
	close(socketfd);
}
