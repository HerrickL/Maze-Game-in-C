/****************************************************************************************************************
* CS 344
* Name: L. Herrick
* Date: 7/10/15
* Program Description: Maze game that creates a directiory based off of the format [username].rooms.[PID],
* creates 7 randomly selected rooms, creates start and end points, creates, 3-6 random connections between the
* rooms.  User recieves console output for starting room and is asked which room to procede to from a list of
* possible connections.  Valites user input and moves to the selected room. For each room after the starting room,
* both a count of moves as well as a list of rooms moved through are saved in arrays.  When the user reaches the
* end room, they see console output that lets them know the game is done along with how many "steps" they took
* and what rooms they went through. 
*****************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>


int ConnectionCount(int* array);
int AlreadyContains(int* array, int room);
void buildRooms(char* directory, char* startRoom);


int main(void)
{
	int stillPlaying=1; 		//bool for gameplay trigger
	char* curFile = malloc(9);	//room name placemarker
	char* roomPath=malloc(30);	//string for directory/filename
	char* pathList[100] = {NULL};	//as per prof comment, won't test to/past 100
	int steps = 0;			//steps counter
	int validInput;			//used as bool for user input validation

	//create directory of format [username].rooms.[PID]
	int maxSize = 50; 
	char* dir = malloc(maxSize);
	int pid = getpid();
	char* preDir = "adventure.rooms.";
	snprintf(dir, maxSize, "%s%d", preDir, pid); 
	mkdir(dir, 0770);
	
	//confirm made, prep for game
	DIR* directory = opendir(dir);
	if(directory == NULL){
		printf("Can't make directory.\n");
		exit(0);
	}
	//generate 7 diff random rooms
	buildRooms(dir, curFile);

	//gameplay
	while(stillPlaying==1){
		char* curConnects[6] = {NULL};	//holds room name of current connections		
		char* lineInFile = malloc(23);	//buffer for reading lines in file
		int index=0;			//index marker for curConnect

		//write room name to path of format [directory]/[roomName]
		FILE *roomFile;
		strcpy(roomPath, dir);
		strcat(roomPath,"/");
		strcat(roomPath,curFile);

		//open file
		roomFile = fopen(roomPath, "r");
		if(roomFile == NULL){
			printf("Error: cannot open room file.\n");
			exit(0);
		}	

		//read file	
		while(fgets(lineInFile, 24, roomFile) != NULL ){
			//collect connections
			if((strncmp(lineInFile,"C", 1))==0) {		
				//save until end of line;
				char* conName=malloc(16);					
				strncpy(conName, &lineInFile[14], 8);
				strtok(conName, "\n");
				
				//add to connections array
				curConnects[index]=conName;
				index++;	
			}
			//check room type for whether or not to continue game
			if((strncmp(lineInFile,"ROOM TYPE: END_ROOM",19))==0){
				stillPlaying=0;
			}
		}		
		//if not end room
		if(stillPlaying !=0){
			char* userInput = malloc(30); 
			//write connection room options- careful for given format
			printf("\n");
			printf("CURRENT LOCATION: %s\n", curFile);			
			printf("POSSIBLE CONNECTIONS: ");
			for(index=0; index<6; index++){
				if(curConnects[index] != NULL){
					printf("%s",curConnects[index]);
					//formating for list
					if(curConnects[index+1] != NULL && index != 5){
						printf(", ");
					}
					else{
						printf(".\n");
					}
				}
			}
			printf("WHERE TO?>");			

			//read user inpuut
			scanf("%s",userInput);
				
			//if option, update curFile
			validInput = 0;
			for(index=0; index<6; index++){
				//if it's valid
				if(curConnects[index] != NULL){
					if((strncmp(userInput, curConnects[index], 10))==0){
						strncpy(curFile, userInput, 8);	//update curFile to input
						pathList[steps]=userInput;	//add to taken path list
						steps++;			//increment step count
						validInput = 1;			//True - valid
						index=6;			//exit loop
					}
				}
			}
			//invalid input - error
			if(validInput == 0)
			{
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
		}
		//if exiting loop, cleanup
		if(stillPlaying == 0){
			free(lineInFile);
			int x;
			for(x=0; x<6; x++){
				free(curConnects[x]);
			}
		}		
	}
	closedir(directory);

	//end room message with winning path - careful for given format
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	//print taken path
	int x;
	for(x=0; x<steps; x++){
		printf("%s\n",pathList[x]);
	}
	
	//clean up before ending
	for(x=0; x<steps; x++){
		free(pathList[x]);
	}
	free(dir);
	free(curFile);
	free(roomPath);
	return 0;
}


/*---------------------------------------------------------------------------------------------------
 * Function to build rooms for maze game.  Randomly chooses 7 of 10 rooms, creates a start/middles/
 * end rooms.  Assigns connections to rooms where rooms have a minimum of 3 and a maximum of 6
 * connections.  Write rooms and room information to file inside given directory.
 * Param: char pointer to directory name, char pointer to initialized variable waiting to be assigned to 
 * starting room name.
 * Pre: directory is made and pointer is not null 
 * Post: 7 rooms files added to directory with room information written to room file, startRoom is
 * assigned to the string name of the starting room.
 ---------------------------------------------------------------------------------------------------*/
void buildRooms(char* directory, char* startRoom){
	srand(time(NULL));			//random seed for all rand calls
	int curRoom, start, end, numConnect;	//variables for index of rooms and number of connects
	int connections[10][6] = {{0}};		//matrix for connections
	char* curPath=malloc(30);		//string for [directory]/[roomname]

	char* room[10];				//room name array
	room[0]="Iwata";
	room[1]="Rosalina";
	room[2]="Reggie";
	room[3]="Zelda";
	room[4]="Sakurai";
	room[5]="Isabelle";
	room[6]="Miyamoto";
	room[7]="Samus";
	room[8]="Fiora";
	room[9]="Toadette";

	char* type[3];				//room type array
	type[0]="START_ROOM";
	type[1]="MID_ROOM";
	type[2]="END_ROOM";

	//random swap rooms of all 10, later take 7
	for(curRoom=9; curRoom>=0; curRoom--){
		//random # between 0-9
		int newSpot = rand()%(curRoom+1);
		//swap
		char* temp = room[curRoom];
		room[curRoom] = room[newSpot];
		room[newSpot] = temp;
	}
	//determine random room type for 7 rooms
	start = rand()%7;
	strcpy(startRoom, room[start]);
	do{
		end = rand()%7;
	}while(end == start);

	//Randomize connections - must have 3 to 6
	for(curRoom=0; curRoom<7; curRoom++){
		int x, count;
		int numConnect=0;
			
		//get number of connections	
		count = ConnectionCount(connections[curRoom]);
	
		//if not already at min rand connects
		if(count < 3){
			// get it to min (3), don't over max (6)
			while(numConnect < 3 || numConnect > 6){
				numConnect = rand() % 4 + 3;
				//address previous entries
				numConnect += count;
			}
		}

		//randomize rooms for connections
		for(x=count; x < numConnect; x++){
			while(!connections[curRoom][x]){
				//rand a room #
				int otherRoom = rand()% 7 + 1; //account for 0=Null in int array - so (1-7)
			
				//check if room has connection capacity open
				int openSpace = ConnectionCount(connections[otherRoom-1]);
				
				//is this already a connection
				int contains = AlreadyContains(connections[curRoom], otherRoom);
				//is it this room
				if((otherRoom-1) == curRoom){ //account for 0=null
					contains = 1; //can not contain itself, assign true to loop
				}	
			
				//assign connection
				if(openSpace < 6 && contains == 0){
					connections[curRoom][x]=otherRoom;
					connections[otherRoom-1][openSpace]=curRoom+1;
				}
			}	
		}

	}
	
	//create files for 7 rooms
	for(curRoom=0; curRoom<7; curRoom++){
				
		//adjust path for file
		strcpy(curPath, directory);
		strcat(curPath,"/");
		strcat(curPath,room[curRoom]);
		
		//create file
		FILE *file = fopen(curPath, "w");
		if(file == NULL){
			printf("Error opening %c.\n", room[curRoom]);
			exit(0);
		}
		//write name to file
		fprintf(file, "ROOM NAME: %s\n", room[curRoom]);
		//create connections
		int x;
		for(x=0; x<6; x++){
			if(connections[curRoom][x]){
				int number = connections[curRoom][x] - 1; // to account for int array 0=null
				fprintf(file, "CONNECTION %d: %s\n", (x+1), room[number]);
			}
		}	
		//write type
		if(curRoom == start){
			fprintf(file, "ROOM TYPE: %s\n", type[0]);
		}
		else if(curRoom == end){
			fprintf(file, "ROOM TYPE: %s\n", type[2]);
		}
		else{
			fprintf(file, "ROOM TYPE: %s\n", type[1]);
		}
		fclose(file);	
	}
	free(curPath);	//prep for loop, cleanup
}




/*-------------------------------------------------------------------------------------------------
 * Function to count non-null elements in array
 * Param: pointer to int array
 * Pre: array is not null
 * Post: number of non-null elements
--------------------------------------------------------------------------------------------------*/
int ConnectionCount(int* array){
	int connection;
	int count=0;
	for(connection=0; connection<6; connection++){
		if(array[connection]){
			count++;
		}
	}
	return count;
}


/*--------------------------------------------------------------------------------------------------
 * Funciton to check if int is in array.
 * Param: pointer to int array, int representing a room index
 * Pre: array is not null
 * Post: 1 if found in array, 0 if not found in array
---------------------------------------------------------------------------------------------------*/
int AlreadyContains(int* array, int room){
	int x;
	for(x=0; x<6; x++){
		if(array[x] == room){ 
			return 1; //true - contains
		}
	}
	return 0; //false - does not contain
}
