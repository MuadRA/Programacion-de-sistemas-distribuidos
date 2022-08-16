#include "serverGame.h"
#include <pthread.h>

struct ThreadArgs{
	int socketP1;
	int socketP2;
};

void sendMessageToPlayer (int socketClient, tString message){ //FUNCION REVISADA
	unsigned int msglength;
	unsigned int tam = strlen(message);
	
	// Send server's message length to the client
	msglength = send(socketClient, &tam, sizeof(int), 0);
		
	if (msglength < 0)
		showError("ERROR while writting TAM to the socket");
		
	// Send server's message to the client
	msglength = send(socketClient, message, tam, 0);
	
	if(msglength < 0)
		showError("ERROR while writting MESSAGE to the socket");
}

void receiveMessageFromPlayer (int socketClient, tString message){ //FUNCION REVISADA
	unsigned int msglength, tam;
	
	//Receive the message length
	memset(message, 0, STRING_LENGTH);
	msglength = recv(socketClient, &tam, sizeof(unsigned int), 0);
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading TAM from socket");
	
	//Receive the text message
	memset(message, 0, tam);	
	msglength = recv(socketClient, message, tam, 0);
	
	//Check read bytes
	if(msglength < 0)
		showError("ERROR while reading MESSAGE from socket");
}

void sendCodeToClient (int socketClient, unsigned int code){ //FUNCION REVISADA
	unsigned int msglength;
	
	//Send code to client
	msglength = send(socketClient, &code, sizeof(unsigned int), 0);

	if(msglength < 0)
		showError("ERROR while send CODE to the socket");
}

void sendBoardToClient (int socketClient, tBoard board){ //FUNCION REVISADA
	unsigned int msglength;
	
	// Send server's board to the client
	msglength = send(socketClient, board, 42, 0);
	
	if(msglength < 0)
		showError("ERROR while send BOARD to the socket");
}

unsigned int receiveMoveFromPlayer (int socketClient){ //FUNCION REVISADA
	unsigned int msglength;
	unsigned int move;
	
	//Receive the move from client.
	msglength = recv(socketClient, &move, sizeof(unsigned int), 0);
	
	if (msglength < 0)
		showError("ERROR while reading MOVE from socket");
	
	return move;
}

int getSocketPlayer (tPlayer player, int player1socket, int player2socket){

	int socket;

		if (player == player1)
			socket = player1socket;
		else
			socket = player2socket;

	return socket;
}

tPlayer switchPlayer (tPlayer currentPlayer){

	tPlayer nextPlayer;

		if (currentPlayer == player1)
			nextPlayer = player2;
		else
			nextPlayer = player1;

	return nextPlayer;
}

char switchChipPlayer (char Current_chip){
		char nextChip;
		
		if(Current_chip == 'x'){
			nextChip = 'o';
		}
		else{
			nextChip = 'x';
		}
		
	return nextChip;
}

void *threadProcessing(void *threadArgs){
	tBoard board;					/** Board of the game */
	tPlayer currentPlayer;				/** Current player */
	tMove moveResult;				/** Result of player's move */
	tString player1Name;				/** Name of player 1 */
	tString player2Name;				/** Name of player 2 */
	int endOfGame;					/** Flag to control the end of the game*/
	unsigned int column;				/** Selected column to insert the chip */
	tString message;				/** Message sent to the players */
	unsigned int msglength;				/** Message length */
	char currentP_chip;				/** Current player chip */
	int socketPlayer1, socketPlayer2;		/** Socket descriptor for each player */
		
		//Detach resources.
		pthread_detach(pthread_self());
		
		//Get client socket form thread param
		socketPlayer1 = ((struct ThreadArgs *) threadArgs)->socketP1;
		socketPlayer2 = ((struct ThreadArgs *) threadArgs)->socketP2;
		
		
		//Info message of ID thread
		printf("Thread ID: %lu \n", (unsigned int) pthread_self());
		//Recive player1 name
		receiveMessageFromPlayer (socketPlayer1, &player1Name);
		printf ("Name of player 1 received: %s\n", player1Name);

		// Receive player2 name
		receiveMessageFromPlayer (socketPlayer2, &player2Name);
		printf ("Name of player 2 received: %s\n\n", player2Name);

		// Send rival name to player 1
		sendMessageToPlayer (socketPlayer1, player2Name);

		// Send rival name to player 2
		sendMessageToPlayer (socketPlayer2, player1Name);

		// Init board
		initBoard (board);
		endOfGame = FALSE;
		
		// Calculates who is moving first and send the corresponding code
	    if ((rand() % 2) == 0){
	    	currentPlayer = player1;
	    	currentP_chip = PLAYER_1_CHIP;
	    }
	    else{
	    	currentPlayer = player2;
	    	currentP_chip = PLAYER_2_CHIP;
		}
	
	//while game continues...
	  	while(!endOfGame){	
			//send code, msg and tboard to clients
			//Code p1
			sendCodeToClient(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), TURN_MOVE);	
			//Code p2			
			sendCodeToClient(getSocketPlayer(switchPlayer( currentPlayer), socketPlayer1, socketPlayer2), TURN_WAIT); 
			
			//Information message p1
			sprintf(message, "Its your turn. You play with: %c", currentP_chip); //llevar control decambiarficha		
			sendMessageToPlayer(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), message);
			//Information message p2			
			sprintf(message, "Your rival is thinking... please. Wait! You play with:%c", switchChipPlayer (currentP_chip)); //controlficha
			sendMessageToPlayer(getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), message);
			
			//Board game p1
			sendBoardToClient (getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), board); 
			//Board game p2			
			sendBoardToClient( getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), board);   
			
			//Receive the column chosen by currentPlayer
			column = receiveMoveFromPlayer(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2));	
			
			if((moveResult = checkMove (board, column)) == OK_move){	//if move of currentPlayer is possible.
				//Insert Chip
				insertChip(board, currentPlayer, column);
				
				//If the currentPlayer win
				if(checkWinner (board, currentPlayer)){
					//send code, msg and tboard to clients
					//Code p1
					sendCodeToClient(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), GAMEOVER_WIN);	
					//Code p2			
					sendCodeToClient(getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), GAMEOVER_LOSE); 
			
					//Information message p1
					sprintf(message, "FINISH! YOU WIN THE MATCH!!!");  		
					sendMessageToPlayer(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), message);
					//Information message p2			
					sprintf(message, "FINISH! YOU LOSE THE MATCH!!!");
					sendMessageToPlayer(getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), message);
			
					//Board game p1
					sendBoardToClient (getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), board); 
					//Board game p2			
					sendBoardToClient( getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), board);
					endOfGame = TRUE;
				}
				//If board is full
				else if(isBoardFull (board)){
					//send code, msg and tboard to clients
					//Code p1
					sendCodeToClient(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), GAMEOVER_DRAW);	
					//Code p2			
					sendCodeToClient(getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), GAMEOVER_DRAW); 
			
					//Information message p1
					sprintf(message, "FINISH THE GAME! THE BOARD IS FULL! DRAW!!");  		
					sendMessageToPlayer(getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), message);
					//Information message p2			
					sprintf(message, "FINISH THE GAME! THE BOARD IS FULL! DRAW!!"); 
					sendMessageToPlayer(getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), message);
			
					//Board game p1
					sendBoardToClient (getSocketPlayer(currentPlayer, socketPlayer1, socketPlayer2), board); 
					//Board game p2			
					sendBoardToClient( getSocketPlayer(switchPlayer ( currentPlayer), socketPlayer1, socketPlayer2), board);
					endOfGame = TRUE;
				}
				//If the currentPlayer move is possible
				else{
					//We change the turn
					tPlayer aux = currentPlayer;
					currentPlayer = switchPlayer (currentPlayer);
					currentP_chip = switchChipPlayer (currentP_chip);
				}		 
			}
	  	}
	  	
	close(socketPlayer1);
	close(socketPlayer2);
}



int main(int argc, char *argv[]){

	int socketfd;					/** Socket descriptor */
	struct sockaddr_in serverAddress;		/** Server address structure */
	unsigned int port;				/** Listening port */
	struct sockaddr_in player1Address;		/** Client address structure for player 1 */
	struct sockaddr_in player2Address;		/** Client address structure for player 2 */
	int socketPlayer1, socketPlayer2;		/** Socket descriptor for each player */
	unsigned int clientLength;			/** Length of client structure */
	
	pthread_t threadID;
	struct ThreadArgs *threadArgs;


		// Check arguments
		if (argc != 2) {
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
			exit(1);
		}

		// Init seed
		srand(time(NULL));
		
		
		// Create the socket
		if((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
			showError("Error while creating socket");
			
		// Check
		if (socketfd < 0)
			showError("ERROR while opening socket");

		// Init server structure
		memset(&serverAddress, 0, sizeof(serverAddress));

		
		// Get listening port
		port = atoi(argv[1]);

		// Fill server structure
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);

		// Bind
		if(bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
			showError("ERROR while binding");

		// Listen
		listen(socketfd, 10);

		while(1){
			
			// Get length of client structure
			clientLength = sizeof(player1Address);

			// Accept player 1 connection!!!
			socketPlayer1 = accept(socketfd, (struct sockaddr *) &player1Address, &clientLength);

			// Check accept result
			if (socketPlayer1 < 0)
				showError("ERROR while accepting connection for player 1");

			printf ("Player 1 is connected!\n");

			// Accept player 2 connection!!!
			socketPlayer2 = accept(socketfd, (struct sockaddr *) &player2Address, &clientLength);

			// Check accept result
			if (socketPlayer2 < 0)
				showError("ERROR while accepting connection for player 1");

			printf ("Player 2 is connected!\n");
			
			//Allocate memory
			if((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
				showError("ERROR while allocating memory");
				
			//Set the clients sockets to the thread`s parameter structure.
			threadArgs->socketP1 = socketPlayer1;
			threadArgs->socketP2 = socketPlayer2;
			
			//Create a new thread
			if(pthread_create(&threadID, NULL, threadProcessing, (void *) threadArgs) != 0)
				showError("Error while creating new thread");
		}
	  	
	  	
	close(socketfd);
}
