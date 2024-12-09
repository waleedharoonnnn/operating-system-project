/*

FAST - National University of Computer and Emerging Sciences
CS2006 - Operating Systems FALL 2024
Project Code C++ : A* Search Algorithm ( multithreading )

*/

#include <bits/stdc++.h>
#include <cstdlib>
#include <pthread.h>
using namespace std;

#define ROW 9
#define COL 10

typedef pair<int, int> Pair; // shortcut for int, int pair type
typedef pair<double, pair<int, int> > pPair; // shortcut for pair<int, pair<int, int>> type

struct cell {
	int parent_i, parent_j; // Row and Column index of parent
	double f, g, h; // f = g + h
};

bool closedList[ROW][COL]; // boolean 2D array initialised to false ; no cell has been included yet 
cell cellDetails[ROW][COL]; // 2D array of struct cell to hold details

/*
open list where f = g + h, and i, j are the row and column index of that cell
Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
This open list is implemented as a set of pair of pair.
*/

set<pPair> openList;

bool foundDest = false; // initially false as destination is not reached.

double gNew, hNew, fNew; // To store the 'g', 'h' and 'f' of the 8 successors

int i, j; //Variables for the parameters of the closed list.

int grid[ROW][COL] =
//	1 -  not blocked , 0 - blocked
{ 	{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
        { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },
        { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
        { 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 },
	{ 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1 }	};
Pair src = make_pair(8, 0); // Source is the left-most bottom-most corner

Pair dest = make_pair(0, 0); // Destination is the left-most top-most corner

// functions used by pthreads to traverse grid 
void *firstSuccessor(void *arg);
void *secondSuccessor(void *arg);
void *thirdSuccessor(void *arg);
void *fourthSuccessor(void *arg);
void *fifthSuccessor(void *arg);
void *sixthSuccessor(void *arg);
void *seventhSuccessor(void *arg);
void *eighthSuccessor(void *arg);

void displaygrid(){

	cout << "\n   --------------- G R I D -------------- \n" << endl;

	for (int c = 0; c < COL ; c++){		printf("%4d", c);	}
	cout << endl;

	for (int r = 0 ; r < ROW ; r++){

		printf("%d  ", r);

		for (int c = 0; c < COL ; c++){

			if (r==src.first && c==src.second) cout << "S   ";

			else if (r==dest.first && c==dest.second) cout << "D   ";

			else if (grid[r][c] == 0) cout << "*   ";

			else cout << "    ";

		}
		cout << endl;

	}
	cout << endl;
}

bool isValid(int row, int col)	// Returns true if row number and column number in range
{ 	return (row >= 0) && (row < ROW) && (col >= 0)	&& (col < COL); 	}

bool isUnBlocked(int grid[][COL], int row, int col)	// check whether the given cell is blocked or not
{	return (grid[row][col] == 1);	}

bool isDestination(int row, int col, Pair dest)		// check whether destination cell has  been reached or not
{	return (row == dest.first && col == dest.second);	}

double calculateHValue(int row, int col, Pair dest)	// function to calculate the 'h' heuristics.
{
	return (
		(double)sqrt(
			(row - dest.first) * (row - dest.first)	+ (col - dest.second) * (col - dest.second)
		)
	);
}

void tracePath(cell cellDetails[][COL], Pair dest) // function to trace the path from the source to destination
{
	printf("\nPath ");

	int row = dest.first;
	int col = dest.second;

	stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row	&& cellDetails[row][col].parent_j == col)) {

		Path.push(make_pair(row, col));

		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;

		row = temp_row;
		col = temp_col;
	}

	Path.push(make_pair(row, col));

	while (!Path.empty()) {
		pair<int, int> p = Path.top();
		Path.pop();
		printf("-> ( %d, %d ) ", p.first, p.second);

	}

}

// function to find shortest path between given source to destination cell
// A* Search Algorithm

void aStarSearch(int grid[][COL], Pair src, Pair dest) {

	if (isValid(src.first, src.second) == false) 
	{ cout << "Source invalid\n" ;	}

	else if (isValid(dest.first, dest.second) == false) 
	{	cout << "Destination invalid\n"; }

	else if (isUnBlocked(grid, src.first, src.second) == false	|| isUnBlocked(grid, dest.first, dest.second)	== false) 
	{	cout << "Path blocked\n" ;	}

	else if (isDestination(src.first, src.second, dest)	== true) 
	{	printf("Already at destination\n");	}

	else{

		memset(closedList, false, sizeof(closedList));

		for (i = 0; i < ROW; i++) {
			for (j = 0; j < COL; j++) {

				cellDetails[i][j].f = FLT_MAX;
				cellDetails[i][j].g = FLT_MAX;
				cellDetails[i][j].h = FLT_MAX;

				cellDetails[i][j].parent_i = -1;
				cellDetails[i][j].parent_j = -1;
			}
		}

		// Initialising the parameters of the starting node
		i = src.first, j = src.second;

		cellDetails[i][j].f = 0.0;
		cellDetails[i][j].g = 0.0;
		cellDetails[i][j].h = 0.0;

		cellDetails[i][j].parent_i = i;
		cellDetails[i][j].parent_j = j;

		openList.insert(make_pair(0.0, make_pair(i, j))); // Put the starting cell on the open list and set its 'f' as 0

		while (!openList.empty()) {
			pPair p = *openList.begin();

			openList.erase(openList.begin()); // Remove this vertex from the open list

			// Add this vertex to the closed list
			i = p.second.first;
			j = p.second.second;
			closedList[i][j] = true;

			/*
			Generating all the 8 successor of this cell

			Cell-->Popped Cell (i, j)
			N --> North	 (i-1, j)
			S --> South	 (i+1, j)
			E --> East	 (i, j+1)
			W --> West		 (i, j-1)
			N.E--> North-East (i-1, j+1)
			N.W--> North-West (i-1, j-1)
			S.E--> South-East (i+1, j+1)
			S.W--> South-West (i+1, j-1)*/

			//-------------------------------------------------------------------------------
			//----------------- MULTITHREADING IMPLEMENTATION -------------------------------
			//-------------------------------------------------------------------------------

			pthread_t T[8]; 

			//create each thread and their respective functions are called.
			pthread_create(&T[0], NULL, firstSuccessor, NULL);
			pthread_create(&T[1], NULL, secondSuccessor, NULL);
			pthread_create(&T[2], NULL, thirdSuccessor, NULL);
			pthread_create(&T[3], NULL, fourthSuccessor, NULL);
			pthread_create(&T[4], NULL, fifthSuccessor, NULL);
			pthread_create(&T[5], NULL, sixthSuccessor, NULL);
			pthread_create(&T[6], NULL, seventhSuccessor, NULL);
			pthread_create(&T[7], NULL, eighthSuccessor, NULL);

			for (int i=0 ; i<8 ; i++)	pthread_join(T[i], NULL);

		}

		// When the destination cell is not found
		if (foundDest == false)	cout << "Failed to find the Destination Cell\n" ;

	}
}
//----------- 1st Successor (North) ------------

void* firstSuccessor(void *arg){

	if(foundDest){	return NULL;	}
	if (isValid(i - 1, j) ) {

		if (isDestination(i - 1, j, dest)) {

			cellDetails[i - 1][j].parent_i = i;
			cellDetails[i - 1][j].parent_j = j;

			printf("[ North ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}
		else if (!closedList[i - 1][j]	&& isUnBlocked(grid, i - 1, j)) {

			gNew = cellDetails[i][j].g + 1.0;
			hNew = calculateHValue(i - 1, j, dest);
			fNew = gNew + hNew;

			if (cellDetails[i - 1][j].f == FLT_MAX	|| cellDetails[i - 1][j].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i - 1, j)));

				cellDetails[i - 1][j].f = fNew;
				cellDetails[i - 1][j].g = gNew;
				cellDetails[i - 1][j].h = hNew;

				cellDetails[i - 1][j].parent_i = i;
				cellDetails[i - 1][j].parent_j = j;
			}
		}
	}
	return NULL;
}

	//----------- 2nd Successor (South) ------------

void* secondSuccessor(void *arg){

	if(foundDest){	return NULL;	}

	if (isValid(i + 1, j)) {

		if (isDestination(i + 1, j, dest)) {

			cellDetails[i + 1][j].parent_i = i;
			cellDetails[i + 1][j].parent_j = j;

			printf("[ South ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}
		else if (!closedList[i + 1][j] && isUnBlocked(grid, i + 1, j)) {

			gNew = cellDetails[i][j].g + 1.0;
			hNew = calculateHValue(i + 1, j, dest);
			fNew = gNew + hNew;

			if (cellDetails[i + 1][j].f == FLT_MAX	|| cellDetails[i + 1][j].f > fNew) {	

				openList.insert(make_pair(fNew, make_pair(i + 1, j)));

				cellDetails[i + 1][j].f = fNew;
				cellDetails[i + 1][j].g = gNew;
				cellDetails[i + 1][j].h = hNew;

				cellDetails[i + 1][j].parent_i = i;
				cellDetails[i + 1][j].parent_j = j;
			}
		}
	}
	return NULL;
}

	//----------- 3rd Successor (East) ------------

void* thirdSuccessor(void *arg){

	if (isValid(i, j + 1)) {

		if (isDestination(i, j + 1, dest)) {

			cellDetails[i][j + 1].parent_i = i;
			cellDetails[i][j + 1].parent_j = j;

			printf("[ East ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}
		else if (!closedList[i][j + 1]&& isUnBlocked(grid, i, j + 1)) {
			gNew = cellDetails[i][j].g + 1.0;
			hNew = calculateHValue(i, j + 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i][j + 1].f == FLT_MAX	|| cellDetails[i][j + 1].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i, j + 1)));

				cellDetails[i][j + 1].f = fNew;
				cellDetails[i][j + 1].g = gNew;
				cellDetails[i][j + 1].h = hNew;

				cellDetails[i][j + 1].parent_i = i;
				cellDetails[i][j + 1].parent_j = j;
			}
		}
	}
	return NULL;
}

	//----------- 4th Successor (West) ------------

void* fourthSuccessor(void *arg){

	if (isValid(i, j - 1)) {

		if (isDestination(i, j - 1, dest)) {

			cellDetails[i][j - 1].parent_i = i;
			cellDetails[i][j - 1].parent_j = j;

			printf("[ West ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}

		else if (!closedList[i][j - 1]	&& isUnBlocked(grid, i, j - 1)) {

			gNew = cellDetails[i][j].g + 1.0;
			hNew = calculateHValue(i, j - 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i][j - 1].f == FLT_MAX	|| cellDetails[i][j - 1].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i, j - 1)));

				cellDetails[i][j - 1].f = fNew;
				cellDetails[i][j - 1].g = gNew;
				cellDetails[i][j - 1].h = hNew;

				cellDetails[i][j - 1].parent_i = i;
				cellDetails[i][j - 1].parent_j = j;
			}
		}
	}

	return NULL;
}

	//----------- 5th Successor (North-East)------------

void* fifthSuccessor(void *arg){

	if(foundDest){	return NULL;	}

	if (isValid(i - 1, j + 1)) {

		if (isDestination(i - 1, j + 1, dest)) {

			cellDetails[i - 1][j + 1].parent_i = i;
			cellDetails[i - 1][j + 1].parent_j = j;

			printf("[ North East ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}

		else if (!closedList[i - 1][j + 1]	&& isUnBlocked(grid, i - 1, j + 1)) {

			gNew = cellDetails[i][j].g + 1.414;
			hNew = calculateHValue(i - 1, j + 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i - 1][j + 1].f == FLT_MAX	|| cellDetails[i - 1][j + 1].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i - 1, j + 1)));

				cellDetails[i - 1][j + 1].f = fNew;
				cellDetails[i - 1][j + 1].g = gNew;
				cellDetails[i - 1][j + 1].h = hNew;

				cellDetails[i - 1][j + 1].parent_i = i;
				cellDetails[i - 1][j + 1].parent_j = j;
			}
		}
	}

	return NULL;
}

	//----------- 6th Successor (North-West)------------

void* sixthSuccessor(void *arg){

	if(foundDest){	return NULL;	}
	if (isValid(i - 1, j - 1) ) {

		if (isDestination(i - 1, j - 1, dest)) {

			cellDetails[i - 1][j - 1].parent_i = i;
			cellDetails[i - 1][j - 1].parent_j = j;

			printf("[ North West ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}

		else if (!closedList[i - 1][j - 1] && isUnBlocked(grid, i - 1, j - 1)) {

			gNew = cellDetails[i][j].g + 1.414;
			hNew = calculateHValue(i - 1, j - 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i - 1][j - 1].f == FLT_MAX	|| cellDetails[i - 1][j - 1].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i - 1, j - 1)));

				cellDetails[i - 1][j - 1].f = fNew;
				cellDetails[i - 1][j - 1].g = gNew;
				cellDetails[i - 1][j - 1].h = hNew;

				cellDetails[i - 1][j - 1].parent_i = i;
				cellDetails[i - 1][j - 1].parent_j = j;
			}
		}
	}
	return NULL;
}

	//----------- 7th Successor (South-East)------------

void* seventhSuccessor(void *arg){

	if(foundDest){	return NULL;	}
	if (isValid(i + 1, j + 1)) {

		if (isDestination(i + 1, j + 1, dest)) {

			cellDetails[i + 1][j + 1].parent_i = i;
			cellDetails[i + 1][j + 1].parent_j = j;

			printf("[ South East ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}

		else if (!closedList[i + 1][j + 1] && isUnBlocked(grid, i + 1, j + 1)) {

			gNew = cellDetails[i][j].g + 1.414;
			hNew = calculateHValue(i + 1, j + 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i + 1][j + 1].f == FLT_MAX	|| cellDetails[i + 1][j + 1].f > fNew) {

				openList.insert(make_pair(fNew, make_pair(i + 1, j + 1)));

				cellDetails[i + 1][j + 1].f = fNew;
				cellDetails[i + 1][j + 1].g = gNew;
				cellDetails[i + 1][j + 1].h = hNew;

				cellDetails[i + 1][j + 1].parent_i = i;
				cellDetails[i + 1][j + 1].parent_j = j;
			}
		}
	}
	return NULL;
}

	//----------- 8th Successor (South-West)------------

void* eighthSuccessor(void *arg){

	if(foundDest){		return NULL;	}

	if (isValid(i + 1, j - 1)) {

		if (isDestination(i + 1, j - 1, dest) ) {

			cellDetails[i + 1][j - 1].parent_i = i;
			cellDetails[i + 1][j - 1].parent_j = j;

			printf("[ South West ] Destination cell found \n");

			tracePath(cellDetails, dest);
			foundDest = true;

			return NULL;
		}

		else if (!closedList[i + 1][j - 1]	&& isUnBlocked(grid, i + 1, j - 1)) {

			gNew = cellDetails[i][j].g + 1.414;
			hNew = calculateHValue(i + 1, j - 1, dest);
			fNew = gNew + hNew;

			if (cellDetails[i + 1][j - 1].f == FLT_MAX	|| cellDetails[i + 1][j - 1].f > fNew) {

				openList.insert(make_pair(	fNew, make_pair(i + 1, j - 1)));

				cellDetails[i + 1][j - 1].f = fNew;
				cellDetails[i + 1][j - 1].g = gNew;
				cellDetails[i + 1][j - 1].h = hNew;

				cellDetails[i + 1][j - 1].parent_i = i;
				cellDetails[i + 1][j - 1].parent_j = j;
			}
		}
	}
	return NULL;
}
void clear_screen(){
	#ifdef WINDOWS
		std::sytem("cls");
	#else
		std::system("clear");
	#endif
}

int main(){

	cout<<"\x1b[93m\t\t.-------..-------..-------..-------..-------..-------..-------..-------.\n";
	cout<<"\t\t|       ||       ||       ||       ||       ||       ||       ||       |\n";
	cout<<"\t\t|   A   ||   *   ||   S   ||   E   ||   A   ||   R   ||   C   ||   H   |\n";
	cout<<"\t\t|       ||       ||       ||       ||       ||       ||       ||       |\n";
	cout<<"\t\t'-------''-------''-------''-------''-------''-------''-------''-------'\n\x1b[97m"; 

	//system("pause");
	do{
		cout<<"\nPress Enter to continue.";
	}while(cin.get() != '\n');

	clear_screen();
	//system("cls");

	displaygrid();

	//system("pause");
	do{
		cout<<"\nPress Enter to continue.";
	}while(cin.get() != '\n');
	clear_screen();
	//system("cls");
	displaygrid();
	aStarSearch(grid, src, dest);

	cout << "\n\n";

}
