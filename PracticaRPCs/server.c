#include "server.h"

/** Flag to enable debugging */
#define DEBUG_SERVER 1


/** Array with games */
tGame games[MAX_GAMES];


void initServerStructures (){

	int i;

		if (DEBUG_SERVER)
			printf ("\nServer is on!!!Initializing...\n");

		// Init seed
		srand (time(NULL));

		// Init each game
		for (i=0; i<MAX_GAMES; i++){

			initBoard (games[i].board);

			if ((rand()%2)==0)
				games[i].currentPlayer = player1;
			else
				games[i].currentPlayer = player2;

			memset (games[i].player1Name, 0, STRING_LENGTH);
			memset (games[i].player2Name, 0, STRING_LENGTH);
			games[i].endOfGame = FALSE;
			games[i].status = gameEmpty;
		}
}


tPlayer switchPlayer (tPlayer currentPlayer){

	tPlayer nextPlayer;

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


int locateGameForPlayer (tString name){
	int idx = 0, encontrado = 0;
	int result = ERROR_PLAYER_NOT_FOUND;
	
	//go over games and search for an empty or waitingplayer game.
	while(idx < MAX_GAMES && encontrado == 0){
		if(strcmp(name, games[idx].player1Name) == 0 || strcmp(name, games[idx].player2Name) == 0){
			result = idx;
			encontrado = 1; 
		}		
		idx++;		
	}
	return result;
}


void initGameByIndex (int index){

	//First select randomly who starts the move.
	if ((rand() % 2) == 0) {
		games[index].currentPlayer = player1;
	}
	else {
		games[index].currentPlayer = player2;
	}
	
	//Initialize the board.
	initBoard(games[index].board);

	//Initialize the player1 name.
	memset(games[index].player1Name, 0, STRING_LENGTH);
	//Initialize the player2 name.
	memset(games[index].player1Name, 0, STRING_LENGTH);

	//Initialize endOfGame
	games[index].endOfGame = FALSE;
	
	//Initialize game status
	games[index].status = gameEmpty;
}


int *registerplayer_1_svc(tMessage *argMsg, struct svc_req *rqstp){

	static int  result;
	int gameIndex;

		// Init...
		gameIndex = -1;
		
		// Search for an empty game
		//Server is full
		if ((gameIndex = searchEmptyGame()) == ERROR_SERVER_FULL) {
			result = ERROR_SERVER_FULL;
		}
		//Server is not full
		else {
			
			//The name is not registered
			if (locateGameForPlayer(argMsg->msg) == ERROR_PLAYER_NOT_FOUND) {
				
				//The game in the gameIndex position is waitingPlayer
				if (games[gameIndex].status == gameWaitingPlayer) {
					strcpy(games[gameIndex].player2Name, argMsg->msg);
					games[gameIndex].status = gameReady;
					result = OK_NAME_REGISTERED;
				}
				//The game in the gameIndex position is empty
				else {
					strcpy(games[gameIndex].player1Name, argMsg->msg);
					games[gameIndex].status = gameWaitingPlayer;
					result = OK_NAME_REGISTERED;
				}
			}
			//The name is registered
			else {
				result = ERROR_NAME_REPEATED;
			}
		}

	return &result;
}


tBlock *getgamestatus_1_svc(tMessage *argMsg, struct svc_req *rqstp){

	tBlock *response;
	int gameIndex;

		// Init...
		response = (tBlock*) malloc (sizeof (tBlock));

		// Locate the game
		gameIndex = locateGameForPlayer(argMsg->msg);
		
		//Encontrado
		if(gameIndex != ERROR_PLAYER_NOT_FOUND){
			
			//The board is not full
			if (!isBoardFull(games[gameIndex].board)) {
				
				//There is a winner
				if (checkWinner(games[gameIndex].board, games[gameIndex].currentPlayer)) {
					
					//Check if the currentPlayer is player1
					if (games[gameIndex].currentPlayer == player1) {
						
						//Winner is player1Name (player name given as parameter is equal to player1Name)
						if (strcmp(argMsg->msg, games[gameIndex].player1Name) == 0) {
							response->code = GAMEOVER_WIN;
							sprintf(response->msg, "FINISH!! YOU WIN THE MATCH!!");
						}
						//player2Name Lose
						else {
							response->code = GAMEOVER_LOSE;
							sprintf(response->msg, "FINISH!! YOU LOSE THE MATCH!");
						}
					}
					//Current player is player2
					else {
						
						//player1Name Lose (player name given as parameter is equal to player1Name)
						if (strcmp(argMsg->msg, games[gameIndex].player1Name) == 0) {
							response->code = GAMEOVER_LOSE;
							sprintf(response->msg, "FINISH!! YOU LOSE THE MATCH!");
						}
						//Winner is player2Name
						else {
							response->code = GAMEOVER_WIN;
							sprintf(response->msg, "FINISH!! YOU WIN THE MATCH!!");
						}
					}

					//The match ended with a winner, initilize the game, again.
					initGameByIndex(gameIndex);
				}
				//There is not a winner, the game status is waitingplayer.
				else if (games[gameIndex].status == gameWaitingPlayer) {
					response->code = TURN_WAIT;
					sprintf(response->msg, "Waiting for rival...");
				}
				//There is not a winner, either game state is not waitingPlayer
				else {
					
					//Check if the currentPlayer is player1
					if (games[gameIndex].currentPlayer == player1) {
						
						//Turn_move is player1Name (player name given as parameter is equal to player1Name)
						if (strcmp(argMsg->msg, games[gameIndex].player1Name) == 0) {
							response->code = TURN_MOVE;
							sprintf(response->msg, "Its your turn. You play with: o");
						}
						//Turn_wait is player2Name
						else {
							response->code = TURN_WAIT;
							sprintf(response->msg, "Your rival is thinking... plesase. Wait! You play with: x");
						}
					}
					//Current player is player2
					else {
						
						//Turn_wait is player1Name (player name given as parameter is equal to player1Name)
						if (strcmp(argMsg->msg, games[gameIndex].player1Name) == 0) {
							response->code = TURN_WAIT;
							sprintf(response->msg, "Your rival is thinking... please. Wait! You play with: o");
						}
						//Turn_move is player2Name
						else {
							response->code = TURN_MOVE;
							sprintf(response->msg, "Its your turn. You play with: x");
						}
					}
				}
			}
			//The board is full
			else {
					response->code = GAMEOVER_DRAW;
					sprintf(response->msg, "FINISH THE GAME! THE BORAD IS FULL!! DRAW!!!");
					initGameByIndex(gameIndex);
			}
		}
		//No encontrado
		else {
			response->code = ERROR_PLAYER_NOT_FOUND;
			sprintf(response->msg, "Jugador no registrado, imposible obtener estado de la partida.");
		}

		//Copy to response the board game in gameIndex position.
		strcpy(response->board, games[gameIndex].board);

	return response;
}


tBlock * insertchipinboard_1_svc(tColumn *argCol, struct svc_req *rqstp){

	tMove moveResult;
	tBlock *response;
	int gameIndex;
	
		// Init...
		response = (tBlock*) malloc (sizeof (tBlock));

		// Locate the game
		gameIndex = locateGameForPlayer (argCol->player);

		//Move is possible
		if ((moveResult = checkMove(games[gameIndex].board, argCol->column)) == OK_move) {
			insertChip(games[gameIndex].board, games[gameIndex].currentPlayer, argCol->column);
		}

		//Check if board isn't full
		if (!isBoardFull(games[gameIndex].board)) {
			
			//Check if currentPlayer win the match
			if (checkWinner(games[gameIndex].board, games[gameIndex].currentPlayer)) {
				response->code = GAMEOVER_WIN;
				sprintf(response->msg, "FINISH!! YOU WIN THE MATCH!!");
			}
			//There isn't a winner
			else {
				//Change the currentPlayer
				games[gameIndex].currentPlayer = switchPlayer(games[gameIndex].currentPlayer);

				//Check if the currentPlayer is player1
				if (games[gameIndex].currentPlayer == player1) {
					
					//Turn_move is player1Name (player name given as parameter is equal to player1Name)
					if (strcmp(argCol->player, games[gameIndex].player1Name) == 0) {
						response->code = TURN_MOVE;
						sprintf(response->msg, "Its your turn. You play with: o");
					}
					//Turn_wait is player2Name 
					else {
						response->code = TURN_WAIT;
						sprintf(response->msg, "Your rival is thinking... please. Wait! You play with: x");
					}
				}
				//Current player is player2
				else {
					
					//Turn_wait is player1Name (player name given as parameter is equal to player1Name)
					if (strcmp(argCol->player, games[gameIndex].player1Name) == 0) {
						response->code = TURN_WAIT;
						sprintf(response->msg, "Your rival is thinking... please. Wait! You play with: o");
					}
					//Turn_move is player2Name
					else {
						response->code = TURN_MOVE;
						sprintf(response->msg, "Its your turn. You play with: x");
					}
				}
			}
		}
		//Board is full game ended with DRAW.
		else {
			response->code = GAMEOVER_DRAW;
			sprintf(response->msg, "FINISH THE GAME! THE BOARD IS FULL! DRAW!");
		}

		//copy in the response the board.
		strcpy(response->board, games[gameIndex].board);

	return response;
}
