#include "server.h"
#include <pthread.h>

/** Flag to enable debugging */
#define DEBUG_SERVER 1

//Max request backlog
#define BACKLOG 100

/** Array with games */
tGame games[MAX_GAMES];

int control = 0;

void initServerStructures (){

	int i;

	if (DEBUG_SERVER)
		printf ("Initializing...\n");

	// Init seed
	srand (time(NULL));

	// Init each game
	for (i=0; i<MAX_GAMES; i++){

		// Allocate and init board
		games[i].board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		initBoard (games[i].board);

		// Calculate the first player to play
		if ((rand()%2)==0)
			games[i].currentPlayer = player1;
		else
			games[i].currentPlayer = player2;

		// Allocate and init player names
		games[i].player1Name = (xsd__string) malloc (STRING_LENGTH);
		games[i].player2Name = (xsd__string) malloc (STRING_LENGTH);
		memset (games[i].player1Name, 0, STRING_LENGTH);
		memset (games[i].player2Name, 0, STRING_LENGTH);

		// Game status
		games[i].endOfGame = FALSE;
		games[i].status = gameEmpty;
	}
}

conecta4ns__tPlayer switchPlayer (conecta4ns__tPlayer currentPlayer){

	conecta4ns__tPlayer nextPlayer;

		if (currentPlayer == player1)
			nextPlayer = player2;
		else
			nextPlayer = player1;

	return nextPlayer;
}

int searchEmptyGame (){

	int i, found, result;

		// Init...
		found = FALSE;
		i = 0;
		result = -1;

		// Search for a non-active game
		while ((!found) && (i<MAX_GAMES)){

			if ((games[i].status == gameEmpty) ||
				(games[i].status == gameWaitingPlayer)){
				found = TRUE;
			}
			else
				i++;
		}

		if (found){
			result = i;
		}
		else{
			result = ERROR_SERVER_FULL;
		}

	return result;
}

int locateGameForPlayer (xsd__string name){
	int idx = 0, encontrado = 0;
	int result = ERROR_PLAYER_NOT_FOUND;
	
	//go over games and search for an empty or waitingplayer game.
	while(idx < MAX_GAMES && encontrado == 0){
		if((strcmp(name, games[idx].player1Name) == 0) || (strcmp(name, games[idx].player2Name) == 0)){
			result = idx;
			encontrado = 1; 
		}		
		idx++;		
	}
	return result;
}

void freeGameByIndex (int index){
	
	//First select randomly who starts the move
	if ((rand() % 2) == 0){
		games[index].currentPlayer = player1;
	}
	else{
		games[index].currentPlayer = player2;
	}
	
	//Allocate memory & init the board .
	games[index].board = (xsd__string) malloc(BOARD_WIDTH*BOARD_HEIGHT);
	initBoard(games[index].board);
	
	//Allocate memory & init the player1 name.
	games[index].player1Name = (xsd__string) malloc(STRING_LENGTH);
	memset(games[index].player1Name, 0, STRING_LENGTH);
	//Allocate memory & init the player2 name.
	games[index].player2Name = (xsd__string) malloc(STRING_LENGTH);
	memset(games[index].player2Name, 0, STRING_LENGTH);
	
	//init endOfGame
	games[index].endOfGame = FALSE;
	
	//init game status
	games[index].status = gameEmpty;
}


int conecta4ns__register (struct soap *soap, conecta4ns__tMessage playerName, int *code){

	int gameIndex;
	
	//Init...
	gameIndex = -1;
	
	// Set \0 at the end of the string
	playerName.msg[playerName.__size] = 0;
	
	//Search for an empty
	//check if the server is full
	if((gameIndex = searchEmptyGame()) == ERROR_SERVER_FULL){
		*code = ERROR_SERVER_FULL;
	}
	//If server is not full
	else{

			//Check if playername given is already registered
			if(locateGameForPlayer(playerName.msg) == ERROR_PLAYER_NOT_FOUND){
				
				//The game in the gameIndex position is waitingPlayer
				if(games[gameIndex].status == gameWaitingPlayer){
					strcpy(games[gameIndex].player2Name, playerName.msg);
					games[gameIndex].status = gameReady;
					*code = OK_NAME_REGISTERED;
				}
				//The game in the gameIndex position is empty.
				else{
					strcpy(games[gameIndex].player1Name, playerName.msg);
					games[gameIndex].status = gameWaitingPlayer;
					*code = OK_NAME_REGISTERED;

				}
			}
			//Name registered
			else{
				*code = ERROR_NAME_REPEATED;
			}
	}
	
	return SOAP_OK;
}

int conecta4ns__getStatus (struct soap *soap, conecta4ns__tMessage playerName, conecta4ns__tBlock* status){

	int gameIndex;
	char messageToPlayer [STRING_LENGTH];
	
	while(control == 1){
		printf("%d \n", control);
	}
		printf("%d \n", control);
		// Set \0 at the end of the string
		playerName.msg[playerName.__size] = 0;

		// Allocate memory for the status structure
		(status->msgStruct).msg = (xsd__string) malloc (STRING_LENGTH);
		memset ((status->msgStruct).msg, 0, STRING_LENGTH);
		status->board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		memset (status->board, 0, BOARD_WIDTH*BOARD_HEIGHT);
		status->__size = BOARD_WIDTH*BOARD_HEIGHT;


		if (DEBUG_SERVER)
			printf ("Receiving getStatus() request from -> %s [%d]\n", playerName.msg, playerName.__size);
			
		//Locate the game 
		gameIndex = locateGameForPlayer(playerName.msg);

		//Game Encontrado
		if(gameIndex != ERROR_PLAYER_NOT_FOUND){
			
			//Board is not full
			if(!isBoardFull(games[gameIndex].board)){
				
				//There is a winner
				if(checkWinner(games[gameIndex].board, games[gameIndex].currentPlayer)){
					
					//Check if currentPlayer is player1
					if(games[gameIndex].currentPlayer == player1){
						
						//Winner is player1Name (player name given as parameter is equal to player1Name)
						if(strcmp(playerName.msg, games[gameIndex].player1Name) == 0){
							status->code = GAMEOVER_WIN;
							sprintf(status->msgStruct.msg, "FINISH!! YOU WIN THE MATCH!!");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
						//player2Name Lose
						else{
							status->code = GAMEOVER_LOSE;
							sprintf(status->msgStruct.msg, "FINISH!! YOU LOSE THE MATCH!!");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
					}
					//Current player is player2
					else{
						
						//player1Name Lose (player name given as parameter is equal to player1Name)
						if(strcmp(playerName.msg, games[gameIndex].player1Name) == 0){
							status->code = GAMEOVER_LOSE;
							sprintf(status->msgStruct.msg, "FINISH!! YOU LOSE THE MATCH!!");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
						//player2Name win
						else{
							status->code = GAMEOVER_WIN;
							sprintf(status->msgStruct.msg, "FINISH!! YOU WIN THE MATCH!!");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
					}
					
					//The match ended with a winner, initialize the game in gameIndex position again.
					freeGameByIndex(gameIndex);
				}
				//There is not a winner, the game status is waitingPlayer.
				else if(games[gameIndex].status == gameWaitingPlayer){
					status->code = TURN_WAIT;
					sprintf(status->msgStruct.msg, "Waiting for rival...");
					status->msgStruct.__size = strlen(status->msgStruct.msg);
				}
				//There is not a winner, either game state is not waitingPlayer
				else{
					
					//Check if the currentPlayer is player1
					if(games[gameIndex].currentPlayer == player1){
						
						//Turn_move is player1Name (player name given as parameter is equal to player1Name)
						if(strcmp(playerName.msg, games[gameIndex].player1Name) == 0){
							status->code = TURN_MOVE;
							sprintf(status->msgStruct.msg, "Its your turn. You play with: o");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
						//Turn wait is player2Name
						else{
							status->code = TURN_WAIT;
							sprintf(status->msgStruct.msg, "Your rival is thinking... please. Wait! You play with: x");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
							control = 1;
						}
					}
					//CurrentPlayer is player2
					else{
					
						//Turn_wait is player1Name (player name given as parameter is equal to player1Name)
						if(strcmp(playerName.msg, games[gameIndex].player1Name) == 0){
							status->code = TURN_WAIT;
							sprintf(status->msgStruct.msg, "Your rival is thinking... please. Wait! you play with: o");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
							control = 1;
						}
						//Turn_move is player2Name
						else{
							status->code = TURN_MOVE;
							sprintf(status->msgStruct.msg, "Its your turn. You play with: x");
							status->msgStruct.__size = strlen(status->msgStruct.msg);
						}
					}
				}
			}
			//The board is full
			else{
				status->code = GAMEOVER_DRAW;
				sprintf(status->msgStruct.msg, "FINISH THE GAME! THE BOARD IS FULL!! DRAW!!");
				status->msgStruct.__size = strlen(status->msgStruct.msg);
			}
		}
		//No encontrado
		else{
			status->code = ERROR_PLAYER_NOT_FOUND;	
			sprintf(status->msgStruct.msg, "Player is not registered, impossible obtain match state");
			status->msgStruct.__size = strlen(status->msgStruct.msg);
		}

		//Copy the board from games to status->board
		strcpy(status->board, games[gameIndex].board);
		//Copy the board size
		status->__size = BOARD_HEIGHT * BOARD_WIDTH;
		
	return SOAP_OK;
}

int conecta4ns__insertChip (struct soap *soap, conecta4ns__tMessage playerName, int playerMove, conecta4ns__tBlock* status){

	int gameIndex;
	conecta4ns__tMove moveResult;

		// Set \0 at the end of the string
		playerName.msg[playerName.__size] = 0;

		// Allocate memory for the status structure
		(status->msgStruct).msg = (xsd__string) malloc (STRING_LENGTH);
		memset ((status->msgStruct).msg, 0, STRING_LENGTH);
		status->board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		memset (status->board, 0, BOARD_WIDTH*BOARD_HEIGHT);

		// Locate the game
		gameIndex = locateGameForPlayer (playerName.msg);

		//Move is possible
		if ((moveResult = checkMove(games[gameIndex].board, playerMove)) == OK_move) {
			insertChip(games[gameIndex].board, games[gameIndex].currentPlayer, playerMove);
		}
		
		//Check if board isn't full
		if (!isBoardFull(games[gameIndex].board)) {

			//Check if currentPlayer win the match
			if (checkWinner(games[gameIndex].board, games[gameIndex].currentPlayer)) {
				status->code = GAMEOVER_WIN;
				strcpy(status->msgStruct.msg, "FINISH!! YOU WIN THE MATCH!!");
				status->msgStruct.__size = strlen(status->msgStruct.msg);
			}
			//There isn't a winner
			else {

				//Change the currentPlayer
				games[gameIndex].currentPlayer = switchPlayer(games[gameIndex].currentPlayer);

				//Check if the currentPlayer is player1
				if (games[gameIndex].currentPlayer == player1) {

					//Turn_move is player1Name (player name given as parameter is equal to player1Name)
					if (strcmp(playerName.msg, games[gameIndex].player1Name) == 0) {
						status->code = TURN_MOVE;
						strcpy(status->msgStruct.msg, "Its your turn. You play with: o");
						status->msgStruct.__size = strlen(status->msgStruct.msg);
					}
					//Turn_wait is player2Name
					else {
						status->code = TURN_WAIT;
						strcpy(status->msgStruct.msg, "Your rival is thinking... please. Wait! You play with: x");
						status->msgStruct.__size = strlen(status->msgStruct.msg);
					}
				}
				//Current player is player2
				else {
					
					//Turn_wait is player1Name (player name given as parameter is equal to player1Name)
					if (strcmp(playerName.msg, games[gameIndex].player1Name) == 0) {
						status->code = TURN_WAIT;
						strcpy(status->msgStruct.msg, "Your rival is thinking... please. Wait! You play with: o");
						status->msgStruct.__size = strlen(status->msgStruct.msg);
					}
					//Turn_move is player2Name
					else {
						status->code = TURN_MOVE;
						strcpy(status->msgStruct.msg, "Its your turn. You play with: x");
						status->msgStruct.__size = strlen(status->msgStruct.msg);
					}
				}
			}
		}
		//Board is full game ended with DRAW.
		else {
			status->code = GAMEOVER_DRAW;
			strcpy(status->msgStruct.msg, "FINISH THE GAME! THE BOARD IS FULL! DRAW!");
			status->msgStruct.__size = strlen(status->msgStruct.msg);
		}

		//Copy the board from games to status->board
		strcpy(status->board, games[gameIndex].board);
		//Copy the board size
		status->__size = BOARD_HEIGHT * BOARD_WIDTH;
		
		control = 0;

	return SOAP_OK;
}

void *process_request(void *soap){
	pthread_detach(pthread_self());
	soap_serve((struct soap*)soap);
	soap_destroy((struct soap*)soap);
	soap_end((struct soap*)soap);
	soap_done((struct soap*)soap);
	free(soap);
	return NULL;
}

int main(int argc, char **argv){ 

  int primarySocket, secondarySocket;
  struct soap soap;

		if (argc < 2) {
			printf("Usage: %s <port>\n",argv[0]);
			exit(-1);
		}

		// Init structures
		initServerStructures();

		// Init environment
		soap_init(&soap);
		
		//max socket delay send
		soap.send_timeout = 10;
		//max socket delay receive
		soap.recv_timeout = 10;
		//max keep-alive sequence
		soap.accept_timeout = 3600;
		
		soap.max_keep_alive = 100;
		void *process_request(void*);
		
		//each thread needs a runtime context
		struct soap *tsoap;
		pthread_t tid;
		
		// Bind to the specified port
		if((primarySocket = soap_bind(&soap,NULL,atoi(argv[1]),100)) < 0){
			//tratar error
			soap_print_fault(&soap, stderr); 
			exit(-1);
		}

		printf ("Server is ON! waiting for requests...\n");
		
		

		// Listen to next connection
		while (TRUE) {
			/* Aceptar conexiones entrantes */
			 if ((secondarySocket=soap_accept(&soap))<0){
				// Tratar el error
				soap_print_fault(&soap, stderr);
				exit(-1);
			 }
			 
			 tsoap = soap_copy(&soap);
			 
			 if(!tsoap)
				break;
				
			pthread_create(&tid, NULL, (void*(*))process_request, (void*)tsoap);
			 	 
			 /* Invocar el servicio correspondiente */
			 /*if (soap_serve(&soap) != SOAP_OK){
				 //Tratar el error
				soap_print_fault(&soap, stderr);
				exit(-1);
			 } */
			 	 
			 /* Borrar datos y cerrar socket */
			// soap_end(&soap);
			
			
		}
		
		soap_done(&soap);

  return 0;
}
