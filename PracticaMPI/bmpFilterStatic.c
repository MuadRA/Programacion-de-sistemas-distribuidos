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
	unsigned int threshold;							/** Threshold */
	unsigned int currentRow;						/** Current row being processed */
	unsigned int currentPixel;						/** Current pixel being processed */
	unsigned int outputPixel;						/** Output pixel */
	unsigned int readBytes;							/** Number of bytes read from input file */
	unsigned int writeBytes;						/** Number of bytes written to output file */
	unsigned int totalBytes;						/** Total number of bytes to send/receive a message */
	unsigned int numPixels;							/** Number of neighbour pixels (including current pixel) */
	unsigned int currentWorker;						/** Current worker process */
	tPixelVector vector;							/** Vector of neighbour pixels */
	int imageDimensions[2];							/** Dimensions of input image */
	double timeStart, timeEnd;						/** Time stamps to calculate the filtering time */
	int size, rank, tag;							/** Number of process, rank and tag */
	MPI_Status status;								/** Status information for received messages */
	unsigned int nRows;								/** Size of eah slice that it will be sent to the worker to realize the filter */
	unsigned int row;								/** From which row each worker starts filter the portion of the image */


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
		if (argc != 4){

			if (rank == 0)
				printf ("Usage: ./bmpFilterStatic sourceFile destinationFile threshold\n");

			MPI_Finalize();
			exit(0);
		}

		// Get input arguments...
		sourceFileName = argv[1];
		destinationFileName = argv[2];
		threshold = atoi(argv[3]);


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

			// Allocate memory to copy the bytes between the header and the image data (for copy headers)
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
			
			//Divide the rows of the image into size - 1 parts to send to the workers.
			nRows = imgInfoHeaderInput.biHeight / (size - 1);
			
			printf("------------------------------------------------\n");
			//show in console the size of each part that will be sent to the workers.
			printf("MASTER: Number of rows of each part: ");
			
			for (int i = 1; i < size; i++) {
				//If is the last part.
				if (i == size - 1) {
					printf("%d \n", (nRows + (imgInfoHeaderInput.biHeight) % (size - 1)));
				}
				//If is not the last part.
				else {
					printf(" %d, ", nRows);
				}
			}

			//Show in console the dimensions of the image that will be filter.
			printf("MASTER: Dimension of imgInfoHeaderInput.biHeight: %d \n", imgInfoHeaderInput.biHeight);
			printf("MASTER: Dimension of rowSize: %d \n", rowSize);
			printf("------------------------------------------------\n\n");
			

			//init row in which worker start filer the image
			row = 0;

			//sent to the workers each part of the image division data.
			for (int i = 1; i < size; i++) {
				
				//If the last part add the rows left by the division.
				if(i == size - 1){
					nRows += (imgInfoHeaderInput.biHeight % (size - 1));
				}
				
				//Send to the worker i the data of the image.
				MPI_Send(&rowSize, sizeof(rowSize), MPI_INT, i, tag, MPI_COMM_WORLD);
				MPI_Send(&row, sizeof(row), MPI_INT, i, tag, MPI_COMM_WORLD);
				MPI_Send(&nRows, sizeof(nRows), MPI_INT, i, tag, MPI_COMM_WORLD);
				
				//Image pixels
				MPI_Send(&inputBuffer[row * rowSize], rowSize * nRows, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD);
					
				row += nRows;
			}
			
			//Receive from workers each par of the image division data.
			for (int i = 1; i < size; i++) {
				//Receive from worker, from which row receive the data.
				MPI_Recv(&row, sizeof(row), MPI_INT, i, tag, MPI_COMM_WORLD, &status);
				//Receive from worker, who many rows had processed the worker.
				MPI_Recv(&nRows, sizeof(nRows), MPI_INT, i, tag, MPI_COMM_WORLD, &status);
				
				//Receive the pixels proccessed by worker i
				MPI_Recv(&outputBuffer[row * rowSize], rowSize * nRows, MPI_UNSIGNED_CHAR, i, tag, MPI_COMM_WORLD, &status);
				
				//Show in terminal the data processed by the worker i
				printf("------------------------------------------------\n");
				printf("MASTER: Receiving from worker %d in which row start the filter: %d \n", i, row);
				printf("MASTER: Receiving from worker %d the nRows filter: %d \n", i, nRows);
				printf("------------------------------------------------\n\n");
			}
			
			//Write the data image (pixels) in the output file
			if((writeBytes = write(outputFile, outputBuffer, rowSize * imgInfoHeaderInput.biHeight)) != (rowSize * imgInfoHeaderInput.biHeight)){
				showError("Can't write in outputFile");
			}
			
			// Close files
			close (inputFile);
			close (outputFile);

			// Process ends
			timeEnd = MPI_Wtime();

			// Show processing time
			printf("Filtering time: %f\n",timeEnd-timeStart);

			free(inputBuffer);
			free(outputBuffer);
		}


		// Worker process
		else{
			//Receive from master rowSize
			MPI_Recv(&rowSize, sizeof(rowSize), MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

			//Receive from master in which row worker start processing.
			MPI_Recv(&row, sizeof(row), MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			
			//Receive from master number of rows has to process the worker.
			MPI_Recv(&nRows, sizeof(nRows), MPI_INT,0,tag, MPI_COMM_WORLD, &status);
			
			//Allocate memory in buffers for store the pixels data receive from master.
			inputBuffer = (unsigned char *) malloc(rowSize * sizeof(unsigned char) * nRows);
			outputBuffer = (unsigned char *) malloc(rowSize * sizeof(unsigned char) * nRows);
			
			//Receive pixels from master to worker.
			MPI_Recv(inputBuffer, rowSize * nRows, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		
			//Show in terminal from which row start filter the pixels.
			printf("------------------------------------------------\n");
			printf("WORKER: Filter from row: %d, to row: %d,  nRows: %d \n", row, row +nRows, nRows);
			printf("------------------------------------------------\n\n");
			
			
			//Process the pixels to filter the data image.
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

			//Send to master row
			MPI_Send(&row, sizeof(row), MPI_INT, 0, tag, MPI_COMM_WORLD);
			
			//Send to master nRows
			MPI_Send(&nRows, sizeof(nRows), MPI_INT, 0, tag, MPI_COMM_WORLD);
			
			//Send data image 
			MPI_Send(outputBuffer, rowSize * nRows, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
			
			free(inputBuffer);
			free(outputBuffer); 
		}

		// Finish MPI environment
		MPI_Finalize();
}
