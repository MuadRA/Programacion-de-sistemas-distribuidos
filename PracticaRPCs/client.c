#include "client.h"
#include <unistd.h>
#include <sys/time.h>

/** Timer interval */
#define TIMER_SEC 3

/** Flag to debug the client side */
#define DEBUG_CLIENT 1

/** Client handle */
CLIENT *clientPlayer;


void setTimer (){

	struct itimerval timer;

		// Set the timer
		timer.it_value.tv_sec = TIMER_SEC;
		timer.it_value.tv_usec = 0;
		timer.it_interval = timer.it_value;


		if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
			perror("Error while setting the timer\n");
			exit(1);
		}
}

void stopTimer (){

	struct itimerval timer;

		// Set the timer
		timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 0;
		timer.it_interval = timer.it_value;


		if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
			perror("Error while stopping the timer\n");
			exit(1);
		}

}

void *alarmHandler (void){

	if (DEBUG_CLIENT)
		printf ("[Client] Timer()! -> Waking up...\n");
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

void startGame(char *host){

	tMessage playerName;				/** Player name */
	unsigned int endOfGame;				/** Flag to control the end of the game */
	tBlock *response;					/** Response from server */
	int* resCode;						/** Result for registering the player */
	tColumn column;						/** Player move */
	CLIENT *clnt;

		// Init...
		response = (tBlock*) malloc (sizeof(tBlock));
		resCode = (int*) malloc (sizeof (int));
		endOfGame = FALSE;
		*resCode = 0;

		// Create client handle
		clnt = clnt_create(host, CONECTA4, CONECTA4VER, "udp");

		//check client handle 
		if (clnt == NULL) {
			clnt_pcreateerror(host);
			exit(1);
		}

		// While player is not registered in the server side
		while (*resCode != OK_NAME_REGISTERED) {
			
			do {
				// Init player's name
				memset(&playerName, 0, STRING_LENGTH);
				printf("Enter player name:");
				fgets(&playerName, STRING_LENGTH - 1, stdin);

				// Remove '\n'
				playerName.msg[strlen(playerName.msg) - 1] = 0;
			} while (strlen(playerName.msg) <= 2);
			
			//Call to remote process registerPlayer_1, to register player name.
			resCode = registerplayer_1(&playerName, clnt); 

			//If server is full
			if (*resCode == ERROR_SERVER_FULL) {
				printf("Impossible Register player, SERVER FULL. \n");
			}
			//if nickname entered is already registered.
			else if (*resCode == ERROR_NAME_REPEATED) {
				printf("Impossible Register player, NICKNAME IS ALREADY REGISTERED \n");
			}
		}

		// Player is successfully registered
		if (DEBUG_CLIENT)
			printf ("Player registered... setting up the alarm!\n");

		// Set alarm...
		if (signal(SIGALRM, (void (*)(int)) alarmHandler) == SIG_ERR) {
			perror("Error while setting a handler for SIGALRM");
			exit(1);
		}

		// While game continues...
		while (!endOfGame){
			
			//Call remote process to ask the server the status of the game.
			response = getgamestatus_1(playerName.msg, clnt);
			
			//print the board 
			printBoard(response->board, response->msg);

			// Other player is thinking... WAIT
			if (response->code == TURN_WAIT){
				
				// Set timer to wake up from pause...
				setTimer ();

				// Player waits...
				pause();

				// Stop timer!
				stopTimer();
			}
			//If client have the turn to move.
			else if (response->code == TURN_MOVE) {
				
				//Read the column, to insert chip
				column.column = readMove();
				sprintf(column.player, playerName.msg);
				
				//Call remote process to insert chip
				response = insertchipinboard_1(&column, clnt);
			}
			//Now check if the match finished.
			if (response->code == GAMEOVER_WIN || response->code == GAMEOVER_LOSE || response->code == GAMEOVER_DRAW) {
				endOfGame = TRUE;
				
				if(response->code == GAMEOVER_WIN)
					//print the board 
					printBoard(response->board, response->msg);
			}
			
			
		}
		
	// Destroy the client handler
	clnt_destroy(clnt);
}


int main (int argc, char *argv[]){

	char *host;

		if (argc < 2) {
			printf ("usage: %s server_host\n", argv[0]);
			exit (1);
		}

		// Get host
		host = argv[1];

		// Start the game
		startGame (host);

	exit (0);
}
