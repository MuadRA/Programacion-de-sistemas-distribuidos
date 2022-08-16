#include "bmpBlackWhite.h"
#include "mpi.h"

/** Show log messages */
#define SHOW_LOG_MESSAGES 1

/** Enable output for filtering information */
#define DEBUG_FILTERING 0

/** Show information of input and output bitmap headers */
#define SHOW_BMP_HEADERS 0


int main(int argc, char** argv){

	tBitmapFileHeader imgFileHeaderInput;			/** BMP file header for input image */
	tBitmapInfoHeader imgInfoHeaderInput;			/** BMP info header for input image */
	tBitmapFileHeader imgFileHeaderOutput;			/** BMP file header for output image */
	tBitmapInfoHeader imgInfoHeaderOutput;			/** BMP info header for output image */
	char* sourceFileName;							/** Name of input image file */
	char* destinationFileName;						/** Name of output image file */
	int inputFile, outputFile;						/** File descriptors */
	unsigned char *outputBuffer;					/** Output buffer for filtered pixels */
	unsigned char *inputBuffer;						/** Input buffer to allocate original pixels */
	unsigned char *auxPtr;							/** Auxiliary pointer */
	unsigned int rowSize;							/** Number of pixels per row */
	unsigned int rowsPerProcess;					/** Number of rows to be processed (at most) by each worker */
	unsigned int rowsSentToWorker;					/** Number of rows to be sent to a worker process */
	unsigned int receivedRows;						/** Total number of received rows */
	unsigned int threshold;							/** Threshold */
	unsigned int currentRow;						/** Current row being processed */
	unsigned int currentPixel;						/** Current pixel being processed */
	unsigned int outputPixel;						/** Output pixel */
	unsigned int readBytes;							/** Number of bytes read from input file */
	unsigned int writeBytes;						/** Number of bytes written to output file */
	unsigned int totalBytes;						/** Total number of bytes to send/receive a message */
	unsigned int numPixels;							/** Number of neighbour pixels (including current pixel) */
	unsigned int currentWorker;						/** Current worker process */
	unsigned int *processIDs;
	tPixelVector vector;							/** Vector of neighbour pixels */
	int imageDimensions[2];							/** Dimensions of input image */
	double timeStart, timeEnd;						/** Time stamps to calculate the filtering time */
	int size, rank, tag;							/** Number of process, rank and tag */
	MPI_Status status;								/** Status information for received messages */
	int flag;										/** Flag to indicate if the master finish send all image.*/
	int row;										/** Indicate the row in which worker start filter*/
	int rows_aux;									/** Indicates the row in which master start store the data receive by worker */
	int nRows;										/** Number of rows worker have to filter */
	int all_data_sent;								/** Flag indicates the end of sending data to worker */
	int all_data_received;							/** Flag indicates the end of receiving data from worker. */
	
		// Init
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		tag = 1;
		srand(time(NULL));

		// Check the number of processes
		if (size<2){

			if (rank == 0)
				printf ("This program must be launched with (at least) 2 processes\n");

			MPI_Finalize();
			exit(0);
		}

		// Check arguments
		if (argc != 5){

			if (rank == 0)
				printf ("Usage: ./bmpFilterDynamic sourceFile destinationFile threshold numRows\n");

			MPI_Finalize();
			exit(0);
		}

		// Get input arguments...
		sourceFileName = argv[1];
		destinationFileName = argv[2];
		threshold = atoi(argv[3]);
		rowsPerProcess = atoi(argv[4]);

		// Allocate memory for process IDs vector
		processIDs = (unsigned int *) malloc (size*sizeof(unsigned int));

		// Master process
		if (rank == 0){

			// Process starts
			timeStart = MPI_Wtime();

			// Read headers from input file
			readHeaders (sourceFileName, &imgFileHeaderInput, &imgInfoHeaderInput);
			readHeaders (sourceFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Write header to the output file
			writeHeaders (destinationFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Calculate row size for input and output images
			rowSize = (((imgInfoHeaderInput.biBitCount * imgInfoHeaderInput.biWidth) + 31) / 32 ) * 4;

			// Show info before processing
			if (SHOW_LOG_MESSAGES){
				printf ("[MASTER] Applying filter to image %s (%dx%d) with threshold %d. Generating image %s\n", sourceFileName,
						rowSize, imgInfoHeaderInput.biHeight, threshold, destinationFileName);
				printf ("Number of workers:%d -> Each worker calculates (at most) %d rows\n\n", size-1, rowsPerProcess);
			}

			// Show headers...
			if (SHOW_BMP_HEADERS){
				printf ("Source BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderInput, &imgInfoHeaderInput);
				printf ("Destination BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderOutput, &imgInfoHeaderOutput);
			}

			// Open source image
			if((inputFile = open(sourceFileName, O_RDONLY)) < 0){
				printf("ERROR: Source file cannot be opened: %s\n", sourceFileName);
				exit(1);
			}

			// Open target image
			if((outputFile = open(destinationFileName, O_WRONLY | O_APPEND, 0777)) < 0){
				printf("ERROR: Target file cannot be open to append data: %s\n", destinationFileName);
				exit(1);
			}

			// Allocate memory to copy the bytes between the header and the image data
			outputBuffer = (unsigned char*) malloc ((imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE) * sizeof(unsigned char));

			// Copy bytes between headers and pixels
			lseek (inputFile, BIMAP_HEADERS_SIZE, SEEK_SET);
			read (inputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);
			write (outputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);

			// Allocate memory to copy the pixels of the image.
			inputBuffer = (unsigned char*) malloc((rowSize * sizeof(unsigned char) * imgInfoHeaderInput.biHeight));
			outputBuffer = (unsigned char*) malloc((rowSize * sizeof(unsigned char) * imgInfoHeaderInput.biHeight));
			
			//Read pixels image, write in inputFile, if error show messageError.
			if ((readBytes = read(inputFile, inputBuffer, rowSize * imgInfoHeaderInput.biHeight)) != (rowSize * imgInfoHeaderInput.biHeight)) {
				showError("Can't read from inputFile");
			}
			
			printf("------------------------------------------------\n");	
			//Show in console the dimensions of the image that will be filter.
			printf("MASTER: Dimension of imgInfoHeaderInput.biHeight: %d \n", imgInfoHeaderInput.biHeight);
			printf("MASTER: Dimension of rowSize: %d \n", rowSize);
			printf("------------------------------------------------\n\n");

			//Init row, we sart send data from row 0
			row = 0; 
			
			//indicates the start row for worker i (pos 0 is master not used)
			int tabla[size];
			
			//Indicates the number of parts the image has been divided
			int tam_aux = imgInfoHeaderInput.biHeight / rowsPerProcess;
			
			if(imgInfoHeaderInput.biHeight % rowsPerProcess != 0){
				tam_aux += 1;
			}
			printf("MASTER MUESTRA EL TAMAÑO DEL ALTO: %d \n\n", imgInfoHeaderInput.biWidth);
			
			//Indicates the number of parts master have receive or send
			int enviados = 0, recibidos = 0;
			
			flag = 0;
			for(int i = 1; i < size; i++){
				MPI_Send(&imgInfoHeaderInput.biHeight, sizeof(rowSize), MPI_INT, i, tag, MPI_COMM_WORLD);
				//Send rowSize, row and buffer to worker i
				MPI_Send(&rowSize, sizeof(rowSize), MPI_INT, i, tag, MPI_COMM_WORLD);
				MPI_Send(&row, sizeof(row), MPI_INT, i, tag, MPI_COMM_WORLD);		
				MPI_Send(&inputBuffer[row * rowSize], rowSize * rowsPerProcess, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
				
				enviados++;
				
				tabla[i] = row;
				
				//Not the last part
				if((row + rowsPerProcess) < imgInfoHeaderInput.biHeight){
					printf("------------------------------------------------\n");
					printf("MASTER: Send by worker %d From row: %d, to row: %d \n", i, row, row + rowsPerProcess);
					
					row += rowsPerProcess;	
				}
				//Last part
				else{
					printf("------------------------------------------------\n");
					printf("MASTER: Send by worker %d  From row: %d, to row: %d \n", i, row, row + (imgInfoHeaderInput.biHeight % rowsPerProcess));
					row += (imgInfoHeaderInput.biHeight % rowsPerProcess);
				}		
				
				printf("------------------------------------------------\n");
			}
			
			all_data_sent = 0;
			
			while(flag == 0){
				//Recieve number of rows processed by the worker first
				MPI_Recv(&nRows, sizeof(nRows), MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				printf("HOLA \n");
				rows_aux = tabla[status.MPI_SOURCE];
				
				printf("------------------------------------------------\n");
				printf("MASTER: Receiving from process %d row: %d \n", status.MPI_SOURCE, rows_aux);
				printf("MASTER: Receiving from process %d nRows: %d \n", status.MPI_SOURCE, nRows);
				printf("MASTER recibe paquete %d  del buffer del proceso %d \n", recibidos,  status.MPI_SOURCE);
				printf("------------------------------------------------\n\n");
				
				//Receive the buffer data form worker
				MPI_Recv(&outputBuffer[rows_aux * rowSize], rowSize * nRows, MPI_UNSIGNED_CHAR, status.MPI_SOURCE, tag, MPI_COMM_WORLD, &status);
				recibidos++;
				
				
				//All data parts of the image are send 
				if(enviados == tam_aux - 1){
					all_data_sent = 1;
				}
				//Al data parts of the image are not send
				else{
					all_data_sent = 0;
				}
				
				//Not all parts has been sent to workers.
				if(all_data_sent == 0){
					
					MPI_Send(&row, sizeof(row), MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
					MPI_Send(&inputBuffer[row * rowSize], rowSize * nRows, MPI_UNSIGNED_CHAR, status.MPI_SOURCE, tag, MPI_COMM_WORLD);
					enviados++;
					
					tabla[status.MPI_SOURCE] = row;
					
					//No End
					if((row + rowsPerProcess) < imgInfoHeaderInput.biHeight){
						printf("------------------------------------------------\n");
						printf("MASTER: From row: %d, to row: %d \n", row, row + rowsPerProcess);
						row += rowsPerProcess;	
					}
					//The end
					else{
						printf("------------------------------------------------\n");
						printf("MASTER: From row: %d, to row: %d \n", row, row + (imgInfoHeaderInput.biHeight % rowsPerProcess));
						row += (imgInfoHeaderInput.biHeight % rowsPerProcess);
					}
						printf("------------------------------------------------\n\n");
					
				}
				
				printf("MASTER packets receive %d from total of: %d \n", recibidos, tam_aux - 1);
				
				//All data have been received
				if(recibidos == tam_aux - 1){
					flag = 1;
				}
				//All data have not been received
				else{
					flag = 0;
				}
				
				//If the flag indicating the end of master is on, send to all workers -1 to indicate the end of the worker.
				if(flag == 1){
					nRows = -1;
					for(int i = 1; i < size; i++){
						MPI_Send(&nRows, sizeof(nRows), MPI_INT, i, tag, MPI_COMM_WORLD);
					}
				}
				
			}
			
			//Write in outputFile the pixels store in outputBuffer.
			if((writeBytes = write(outputFile, outputBuffer,rowSize * imgInfoHeaderInput.biHeight)) != (rowSize * imgInfoHeaderInput.biHeight)){
				showError("Can't write in outputFile");
			}
			

			// Close files
			close (inputFile);
			close (outputFile);

			// Process ends
			timeEnd = MPI_Wtime();

			// Show processing time
			printf("Filtering time: %f\n",timeEnd-timeStart);
			
			//Free memory of the buffers
			free(inputBuffer);
			free(outputBuffer);
		}


		// Worker process
		else{
			MPI_Recv(&imgInfoHeaderInput.biHeight, sizeof(rowSize), MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			printf("WORKER MUESTRA EL TAMAÑO DEL ALTO: %d \n\n", imgInfoHeaderInput.biWidth);
			printf("WORKER MUESTRA EL TAMAÑO DEL ancho: %d \n\n", imgInfoHeaderInput.biHeight);
			
			all_data_received = 0;
			
			//Receive rowSize
			MPI_Recv(&rowSize, sizeof(rowSize), MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			
			//Allocate memory 
			inputBuffer = (unsigned char *) malloc(rowSize * sizeof(unsigned char) * rowsPerProcess);
			outputBuffer = (unsigned char *) malloc(rowSize * sizeof(unsigned char) * rowsPerProcess);
			
			//Until all data is received and filter continue the while.
			while(all_data_received == 0){
				//Receive row
				MPI_Recv(&row, sizeof(row), MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
				
				//if not the flag indicating the end of worker. (not all data is rceive)
				if(row != -1){
					
					//not last part
					if((row + rowsPerProcess) < imgInfoHeaderInput.biHeight){ //Porque el alto en el worker cambia?
						nRows = rowsPerProcess;	
					}
					//Last part
					else{
						nRows = imgInfoHeaderInput.biHeight % rowsPerProcess;
					}
		
					printf("\nWORKER: filter until row: %d \n\n", nRows + row);
					
					//Receive the buffer data (pixels)
					MPI_Recv(inputBuffer, rowSize * nRows, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);
					
						for(int current_row = 0; current_row < nRows; current_row++){
							
							//Process the row 
							for(int pixel = 0; pixel < rowSize; pixel++){
								numPixels = 0;
								
								//If first pixel (1 neighbour)
								if(pixel == 0){
									vector[numPixels] = inputBuffer[pixel + (rowSize * current_row)];
									numPixels++;
									vector[numPixels] = inputBuffer[pixel + 1 + (rowSize * current_row)];
									numPixels++;
								}
								//If last pixel (1 neighbour)
								else if(pixel == imgInfoHeaderInput.biWidth - 1){
									vector[numPixels] = inputBuffer[pixel - 1 + (rowSize * current_row)];
									numPixels++;
									vector[numPixels] = inputBuffer[pixel + (rowSize * current_row)];
									numPixels++;
								}
								//If not the last, not the first, (3 neighbour)
								else{
									vector[numPixels] = inputBuffer[pixel - 1 + (rowSize * current_row)];
									numPixels++;
									vector[numPixels] = inputBuffer[pixel + (rowSize * current_row)];
									numPixels++;
									vector[numPixels] = inputBuffer[pixel + 1 + (rowSize * current_row)];
									numPixels++;
								}
								
								//Filter the pixels and copy in outputBuffer.
								outputBuffer[pixel + (rowSize * current_row)] = calculatePixelValue(vector, numPixels,threshold , DEBUG_FILTERING);
							}
							
						}
					
				//Send to master nRows
				MPI_Send(&nRows, sizeof(nRows), MPI_INT, 0, tag, MPI_COMM_WORLD);
				//Send data image 
				MPI_Send(outputBuffer, rowSize * nRows, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
				}
				//All data is receive and filter
				else{
					all_data_received = 1;
				}
			}
			
			printf("WORKER END \n");
			
			//Liberate memory of buffers.
			free(outputBuffer);
			free(inputBuffer);
		}

		// Finish MPI environment
		MPI_Finalize();


}
