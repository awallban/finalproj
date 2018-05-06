
#include<iostream>
#include<cmath>
#include<mpi.h>
#include<string>
#include<vector>
#include<stddef.h>
#include<stdlib.h>
#include<time.h>

using namespace std;


MPI_Datatype mpi_board_type;
MPI_Datatype mpi_range_type;

////////////////////////////////////////////////////////////////////////////////
//fun with functions
//
typedef struct Board_s{
	char b[8][8] = {
		{'_','X','_','X','_','X','_','X'},
		{'X','_','X','_','X','_','X','_'},
		{'_','X','_','X','_','X','_','X'},
		{'_','_','_','_','_','_','_','_'},
		{'_','_','_','_','_','_','_','_'},
		{'O','_','O','_','O','_','O','_'},
		{'_','O','_','O','_','O','_','O'},
		{'O','_','O','_','O','_','O','_'}
	};

	int score = 0;
} Board;
typedef struct Range{
	int min;
	int max;
} Range;

void copyBoard(Board& original, Board& copied){
    for(int i =0; i<8;i++)
	    for(int j=0; j < 8;j++)
		    copied.b[i][j] = original.b[i][j];
    copied.score = original.score;
}



//checks for multiple jumps for a given piece of the board
vector<Board> multipleJumps(Board board, int row, int col, bool maximizer){
	vector<Board>results;
	if(maximizer==true){
		//check - can this piece move forward at all?
			if(row+1<8){
				//check - can this piece capture anything to the right?
				if(col+1 <= 8 && row+2 < 8 && board.b[row+1][col+1]=='O'){
					if(col+2 < 8 && board.b[row+2][col+2] == '_'){
						board.b[row+2][col+2]='X';
						board.b[row][col] = '_';
						board.b[row+1][col+1]='_';
						results.push_back(board);
						// make recursive call to see if it can still capture stuff
						// gather those recursive results into a vector
						// append total moves vector with results from recursive calls
						vector<Board> recursiveBoards;
						recursiveBoards = multipleJumps(board,row+2,col+2,maximizer);
						//cout << "SIZE OF RECURSIVE BOARDS: " << recursiveBoards.size() << endl;
						results.insert(results.end(),recursiveBoards.begin(),recursiveBoards.end());
						// replace stuff
						board.b[row+2][col+2]='_';
						board.b[row][col]='X';
						board.b[row+1][col+1]='O';
					}
				}
				//check - can this piece capture anything to the left?
				if(col-1 >= 0 && row+2 < 8 && board.b[row+1][col-1]=='O'){
					if(col-2 >= 0 && board.b[row+2][col-2] == '_'){
						board.b[row+2][col-2]='X';
						board.b[row][col] = '_';
						board.b[row+1][col-1]='_';
						results.push_back(board);
						// make recursive call to see if it can still capture stuff
						// gather those recursive results into a vector
						// append total moves vector with results from recursive calls
						vector<Board> recursiveBoards;
						recursiveBoards = multipleJumps(board,row+2,col-2,maximizer);
						//cout << "SIZE OF RECURSIVE BOARDS: " << recursiveBoards.size() << endl;
						results.insert(results.end(),recursiveBoards.begin(),recursiveBoards.end());
						// replace stuff
						board.b[row+2][col-2]='_';
						board.b[row][col]='X';
						board.b[row+1][col-1]='O';
					}
				}
			}

	}
	else if(maximizer==false){
			//check - can this piece move forward at all?
			if(row-1>=0){
				//check - can this piece capture anything to the right?
				if(col+1 < 8 && row-2 >= 0 && board.b[row-1][col+1]=='X'){
					if(col+2 < 8 && board.b[row-2][col+2] == '_'){
						board.b[row-2][col+2]='O';
						board.b[row][col] = '_';
						board.b[row-1][col+1]='_';
						results.push_back(board);
						// make recursive call to see if it can still capture stuff
						// gather those recursive results into a vector
						// append total moves vector with results from recursive calls
						vector<Board> recursiveBoards;
						recursiveBoards = multipleJumps(board,row-2,col+2,maximizer);
						results.insert(results.end(),recursiveBoards.begin(),recursiveBoards.end());
						// replace stuff
						board.b[row-2][col+2]='_';
						board.b[row][col]='O';
						board.b[row-1][col+1]='X';
					}
				}
				//check - can this piece capture anything to the left?
				if(col-1 >= 0 && row-2 >= 0 && board.b[row-1][col-1]=='X'){
					if(col-2 >= 0 && board.b[row-2][col-2] == '_'){
						board.b[row-2][col-2]='O';
						board.b[row][col] = '_';
						board.b[row-1][col-1]='_';
						results.push_back(board);
						// make recursive call to see if it can still capture stuff
						// gather those recursive results into a vector
						// append total moves vector with results from recursive calls
						vector<Board> recursiveBoards;
						recursiveBoards = multipleJumps(board,row-2,col-2,maximizer);
						results.insert(results.end(),recursiveBoards.begin(),recursiveBoards.end());
						// replace stuff
						board.b[row-2][col-2]='_';
						board.b[row][col]='O';
						board.b[row-1][col-1]='X';
					}
				}
			}	
	}

	return results;
}


//figure out what moves are possible and append them to a vector, return the vector
vector<Board> getPossibleMoves(Board board, bool maximizer){
	vector<Board> moves;
	if(maximizer==true){
		//search array for maximizer pieces, which move from top to bottom
		for(int i = 0; i <= 7; i++){
			for(int j = 0; j <= 7; j++){
				// is this piece a maximizer?
				if(board.b[i][j]=='X'){
					Board copy;
					for(int k = 0; k < 8; k++){
						for (int l = 0; l < 8; l++){
							copy.b[k][l]=board.b[k][l];
						}
					}
					//check - can this piece move forward at all?
					if(i+1<8){
						//check - can this piece capture anything to the right?
						if(j+1 <= 7 && i+2 <= 7 && copy.b[i+1][j+1]=='O'){
							if(j+2 <= 7 && copy.b[i+2][j+2] == '_'){
								copy.b[i+2][j+2]='X';
								copy.b[i][j] = '_';
								copy.b[i+1][j+1]='_';
								moves.push_back(copy);
								vector<Board>multJumps;
								multJumps = multipleJumps(copy,i+2,j+2,maximizer);
								//cout << "SIZE OF MULT JUMPS: " << multJumps.size() << endl;
								// make recursive call to see if it can still capture stuff
								// gather those recursive results into a vector
								// append total moves vector with results from recursive calls
								moves.insert(moves.end(),multJumps.begin(),multJumps.end());
								// replace stuff
								copy.b[i+2][j+2]='_';
								copy.b[i][j]='X';
								copy.b[i+1][j+1]='O';
							}
						}
						//check - can this piece capture anything to the left?
						if(j-1 >= 0 && i+2 <= 7 && copy.b[i+1][j-1]=='O'){
							if(j-2 >= 0 && copy.b[i+2][j-2] == '_'){
								copy.b[i+2][j-2]='X';
								copy.b[i][j] = '_';
								copy.b[i+1][j-1]='_';
								moves.push_back(copy);
								vector<Board>multJumps;
								multJumps = multipleJumps(copy,i+2,j-2,maximizer);
								//cout << "SIZE OF MULT JUMPS: " << multJumps.size() << endl;
								// make recursive call to see if it can still capture stuff
								// gather those recursive results into a vector
								// append total moves vector with results from recursive call
								moves.insert(moves.end(),multJumps.begin(),multJumps.end());
								// replace stuff
								copy.b[i+2][j-2]='_';
								copy.b[i][j]='X';
								copy.b[i+1][j-1]='O';
							}
						}
						//check - can this piece move right?
						if(j+1 <=7 && i+1 <= 7 && copy.b[i+1][j+1]=='_'){
							copy.b[i+1][j+1]='X';
							copy.b[i][j]='_';
							moves.push_back(copy);
							//replace stuff
							copy.b[i+1][j+1]='_';
							copy.b[i][j]='X';
						}
						//check - can this piece move left?
						if(j-1 >= 0 && i+1 <= 7 && board.b[i+1][j-1]=='_'){
							copy.b[i+1][j-1]='X';
							copy.b[i][j]='_';
							moves.push_back(copy);
							//replace stuff
							copy.b[i+1][j-1]='_';
							copy.b[i][j]='X';
						}
					}
				}
			}
		}
	}
	if(maximizer==false){
		//search array for minimizer pieces, which move from bottom to top
		for(int i = 7; i >= 0; i--){
			for(int j = 7; j >= 0; j--){
				// is this piece a minimizer?
				if(board.b[i][j]=='O'){
					Board copy;
					for(int k = 0; k < 8; k++){
						for (int l = 0; l < 8; l++){
							copy.b[k][l]=board.b[k][l];
						}
					}
					//check - can this piece move forward at all?
					if(i-1>=0){
						//check - can this piece capture anything to the right?
						if(j+1 <= 7 && i-2 >= 0 && copy.b[i-1][j+1]=='X'){
							if(j+2 <= 8 && copy.b[i-2][j+2] == '_'){
								copy.b[i-2][j+2]='O';
								copy.b[i][j] = '_';
								copy.b[i-1][j+1]='_';
								moves.push_back(copy);
								// make recursive call to see if it can still capture stuff
								// gather those recursive results into a vector
								// append total moves vector with results from recursive calls
								// replace stuff
								copy.b[i-2][j+2]='_';
								copy.b[i][j]='O';
								copy.b[i-1][j+1]='X';
							}
						}
						//check - can this piece capture anything to the left?
						if(j-1 >= 0 && i-2 >= 0 && copy.b[i-1][j-1]=='X'){
							if(j-2 >= 0 && copy.b[i-2][j-2] == '_'){
								copy.b[i-2][j-2]='O';
								copy.b[i][j] = '_';
								copy.b[i-1][j-1]='_';
								moves.push_back(copy);
								// make recursive call to see if it can still capture stuff
								// gather those recursive results into a vector
								// append total moves vector with results from recursive calls
								// replace stuff
								copy.b[i-2][j-2]='_';
								copy.b[i][j]='O';
								copy.b[i-1][j-1]='X';
							}
						}
						//check - can this piece move right?
						if(j+1 <= 7 && i-1 >= 0 && copy.b[i-1][j+1] == '_'){
							copy.b[i-1][j+1]='O';
							copy.b[i][j]='_';
							moves.push_back(copy);
							//replace stuff
							copy.b[i-1][j+1]='_';
							copy.b[i][j]='O';
						}
						//check - can this piece move left?
						if(j-1 >= 0 && i-1 >= 0 && board.b[i-1][j-1] == '_'){
							copy.b[i-1][j-1]='O';
							copy.b[i][j]='_';
							moves.push_back(copy);
							//replace stuff 
							copy.b[i-1][j-1]='_';
							copy.b[i][j]='O';
						}

					}
				}
			}
		}
	}

return moves;	
}
//prints the gameboard
void printBoard(Board board){
	cout << board.score<<"\n================" << endl;
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			cout << board.b[i][j] << '|';
		}
		cout << endl;
	}
	cout << "================" << endl;
}

//determines if the game is over and who won
int scoreBoard(Board board){
	bool X=false;
	bool O=false;	//call players X and O 
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			if(board.b[i][j]=='X'){
				X=true;
		
			}
			if(board.b[i][j]=='O'){
				O=true;
			}
		}
			
	}
	//if there are pieces on both sides
	if(X&&O){
		return 0;
	}
	if(X){
		return 10;
	}
	if(O){
		return -10;
	}
	return 999999; //there are no pieces. something is wrong; we should never get here
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Board minimax(Board original, bool maximizer){
	//check to see if anyone's won the game yet
	int score = scoreBoard(original);
	if(score!=0){
		original.score=score;
		return original;
	}
	
	//if not, see if any moves can be made
	vector<Board> options=getPossibleMoves(original,maximizer);

	//will print incremental steps
	//cout <<"From"<<endl;
	//printBoard(original);
	//cout <<"options"<<endl;
	//for(auto i: options)
	//printBoard(i);
	

	//now, if it's a draw...
	if(options.size()==0){
		//cout <<"draw"<<endl;
		return original;
	}
	//if the game is still going:
	//IF YOU ARE THE MAXIMIZER
	if(maximizer == true){
		for(int i = 0; i < options.size(); i++){
			options[i]=minimax(options[i], !maximizer); //flipping the bool
			
		}
		Board best;						//the highest scoring route
		best.score = -100;
		for(auto i: options){					//i is a board state in this case
			if(i.score>best.score){
				copyBoard(i, best);
			}

		}
		return best;
	}
	//IF YOU ARE THE MINIMIZER
	else if(maximizer == false){
		for(int i =0; i< options.size(); i++){
			options[i]=minimax(options[i], !maximizer);
		}
		Board best;
		best.score = 100;
		for(auto i:options){
			if(i.score<best.score){
				copyBoard(i,best);
			}
		return best;
	
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void parallelSubMaster(bool maximizer, int id, int parentId, Range childRange, Board original){
	vector<Board> boards = getPossibleMoves(original,maximizer);
	if(childRange.min+boards.size() > childRange.max){
		// send 0 to parent, tag 10
		//GENERAL STRUCTURE OF MPI SEND FOR REF
		/*
		/MPI_Send(
				    void* data,
				        int count,
					    MPI_Datatype datatype,
					        int destination,
						    int tag,
						        MPI_Comm communicator)*/
		//Not needed as we are doing range now
		//MPI_Send(0 , 1, MPI_INT, parentId, 10, MPI_COMM_WORLD);
		Board result = minimax(original, maximizer);
		// send result to parent tag 11
		MPI_Send(&result, 1, mpi_board_type, parentId, 11,MPI_COMM_WORLD);
		return;
	}
	//int totalUsed = 0;
	
	vector<Range> ranges;
	int width = childRange.max - childRange.min-boards.size();
	for(int i =0; i < boards.size();i++){
		Range temp;
		temp.min = (childRange.min+boards.size()) + width/boards.size()*i;
		temp.max = (childRange.min+boards.size()) + width/boards.size()*(i+1) -1;//Off by one errors are the worst
		ranges.push_back(temp);
	}
	

	for(int i =0; i<boards.size();i++){
		//Send !maximizer state
		bool temp = !maximizer;
		MPI_Send(&temp, 1, MPI_INT, childRange.min+i, 03, MPI_COMM_WORLD);
		//Send childRange to children
		MPI_Send(&ranges[i], 1, mpi_range_type, childRange.min+i, 02, MPI_COMM_WORLD);
		MPI_Send(&boards[i], 1, mpi_board_type, childRange.min+i, 01, MPI_COMM_WORLD);

		/* Not needed; range now
		int used = 0;
		MPI_Recv(&used, 1, MPI_INT, childStartId+i, 10,MPI_COMM_WORLD);
		totalUsed+=used;
		childStartId+=used;
		*/
		// send boards[i] to core childStartId+i, tag 01
		// get number of cores used back from children tag 10, update childStartId
	}
	//MPI_Send(&totalUsed, 1, MPI_INT, parentId, 10, MPI_COMM_WORLD);
	//Send number of cores used to master tag 10
	for(int i=0; i<boards.size();i++){
		//Get boards back from cores;might as well refill boards, tag 11
		MPI_Recv(&boards[i], 1,mpi_board_type, MPI_ANY_SOURCE, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	Board best;
	best.score=0;
	if(maximizer){
		for(auto i:boards)
			if(best.score<i.score)
				copyBoard(i,best);
	}
	else{
		for(auto i:boards)
			if(best.score>i.score)
				copyBoard(i,best);
	}
	MPI_Send(&best, 1, mpi_board_type, parentId, 11,MPI_COMM_WORLD);
	//return best board to master
	
	
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	
	MPI_Init(&argc, &argv);

	//YAY custom MPI data types...yay.....yeah
	const int nitems=2;
	int blocklengths[2] = {64,1};
	MPI_Datatype types[2] = {MPI_CHAR,MPI_INT};
	MPI_Aint offsets[2];
	offsets[0] = offsetof(Board, b);
	offsets[1] = offsetof(Board, score);

	MPI_Type_create_struct(nitems,blocklengths,offsets,types,&mpi_board_type);
	MPI_Type_commit(&mpi_board_type);
	

	const int bitems=2;
	int blocklength[2] = {1,1};
	MPI_Datatype tpes[2] = {MPI_INT, MPI_INT};
	MPI_Aint offset[2];
	offset[0] = offsetof(Range, min);
	offset[1] = offsetof(Range, max);
	MPI_Type_create_struct(bitems,blocklength,offset,tpes,&mpi_range_type);
	MPI_Type_commit(&mpi_range_type);


	int worldRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	int worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	clock_t start = clock();

	Board emptyBoard;

//	printBoard(emptyBoard);

//	vector<Board> moves = getPossibleMoves(emptyBoard,true);
//	for (int i = 0; i < moves.size();i++){
//		printBoard(moves[i]);
//	}
	
//	cout << "CORE " << worldRank << " IS HOT." << endl;
//	cout << "WORLDSIZE: " << worldSize << endl;

	if(worldRank == 0){
		Board best;
		vector<Board> boards = getPossibleMoves(emptyBoard, true);

		if(worldSize < boards.size()){ 
			
			cout << "Not enough cores supplied, Will run sequentially" <<endl;
			best = minimax(emptyBoard, true);
			clock_t end = (clock()-start)/CLOCKS_PER_SEC;
			cout << "best endgame is: " << endl;
			printBoard(best);
			cout << "Time to find this board: " << end << " seconds." << endl;
			return 0;
		}

		
		for(int i =0; i < boards.size();i++){
			
			int chunkSize = (worldSize-boards.size()-2)/boards.size();
			Range r;
			r.min = boards.size()+1 + i*chunkSize;
			r.max = boards.size()+1 + (i+1)*chunkSize;
			bool maximizer = false;
			MPI_Send(&maximizer, 1, MPI_INT, i+1, 03, MPI_COMM_WORLD);
			MPI_Send(&r, 1, mpi_range_type, i+1, 02, MPI_COMM_WORLD);
			//send the maximizer state tag 03, childRange tag 02
			MPI_Send(&boards[i], 1,mpi_board_type, i+1, 01, MPI_COMM_WORLD);
		}	
			for(int i =0; i < boards.size();i++){
				MPI_Recv(&boards[i], 1,mpi_board_type, MPI_ANY_SOURCE, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// Get boards back from cores; might as well refill boards, tag 11
		}
	
		best.score =-10;
		for(auto i:boards){
			if(i.score>best.score){
				copyBoard(i,best);
			}
		}
		printBoard(best);
	}
		//print best board option

	else{
		MPI_Status status;
		Board original;
		MPI_Recv(&original, 1,mpi_board_type, MPI_ANY_SOURCE, 01, MPI_COMM_WORLD,&status);
		int parentId = status.MPI_SOURCE;
		//get childRange tag 02, maximizer state, tag 03
		// get board and childStartId, save parentId, save maximizer state, tag 01
		bool maximizer;
		MPI_Recv(&maximizer, 1, MPI_INT, parentId, 03, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		Range childRange;
		MPI_Recv(&childRange, 1, mpi_range_type, parentId, 02, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		parallelSubMaster(maximizer, worldRank, parentId, childRange, original);
	}
	MPI_Finalize();
	return 0;
}

