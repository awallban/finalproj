
#include<iostream>
#include<cmath>
#include<mpi.h>
#include<string>
#include<vector>
#include<stddef.h>
#include<stdlib.h>

using namespace std;


MPI_Datatype mpi_board_type;


////////////////////////////////////////////////////////////////////////////////
//fun with functions
//
typedef struct Board_s{
	char b[8][8];
	int score;
} Board;
void copyBoard(Board& original, Board& copied){
    for(int i =0; i<8;i++)
	    for(int j=0; j < 8;j++)
		    copied.b[i][j] = original[i][j];
    copied.score = orginal.score;
}

//TODO
vector<Board> getPossibleMoves(Board original){
	


return vector<>	
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
	int score = scoreBoard(original);
	if(score!=0){
		original.score=score;
		return original;
	}
	vector<Board> options=getPossibleMoves(original);


	//now, if it's a draw...
	if(options.size()==0){
		return original;
	}
	//if the game is still going:
	//IF YOU ARE THE MAXIMIZER
	if(maximizer){
		for(int i = 0; i < options.size(); i++){
			options[i]=minimax(options[i], !maximizer); //flipping the bool
			
		}
		Board best;						//the highest scoring route
		best.score = -10;
		for(auto i: options){					//i is a board state in this case
			if(i.score>best.score){
				copyBoard(i, best);
			}

		}
		return best;
	}
	//IF YOU ARE THE MINIMIZER
	else{
		for(int i =0; i< options.size(); i++){
			options[i]=minimax(options[i], !maximizer);
		}
		Board best;
		best.score = 10;
		for(auto i:options){
			if(i.score<best.score){
				copyBoard(i,best);
			}
			return best;
	
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void parallelSubMaster(bool maximizer, int id, int parentId, int* childRange, Board original){
	vector<Board> boards = getPossibleMoves(original);
	if(childRange[0]+boards.size() > numCores){//TODO:numCores
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
	//TODO:Build sub ranges
	for(int i =0; i<boards.size();i++){
		//TODO:Send !maximizer state
		//TODO:Send childRange to children
		MPI_Send(&boards[i], 1, mpi_board_type, childRange[0]+i, 01, MPI_COMM_WORLD);

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
		MPI_Recv(&boards[i], 1,mpi_board_type, MPI_ANY_SOURCE, 11, MPI_COMM_WORLD);
	}
	Board best;
	best.score=0;
	if(maximizer{
		for(auto i:board)
			if(best.score<i.score)
				copyBoard(i,best);
	}
	else{
		for(auto i:board)
			if(best.score>i.score)
				copyBoard(i,best);
	}
	MPI_Send(&best, 1, mpi_board_type, parentId, 11,MPI_COMM_WORLD);
	//return best board to master
	
	
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	
	MPI_Init(&argc, &argv);


	const int nitems=2;
	int blocklengths[2] = {64,1};
	MPI_Datatype types[2] = {MPI_CHAR,MPI_INT}
	MPI_Aint offsets[2];
	offsets[0] = offsetof(Board, b);
	offsets[1] = offsetof(Board, score);

	MPI_Type_create_struct(nitems,blocklengths,offsets,types,&mpi_board_type);
	MPI_Type_commit(&mpi_board_type);
	
	if(isRoot){
		vector<Board> boards = getPossibleMoves(emptyBoard);
		for(int i =0; i < boards.size();i++){

			// Send boards[i] to core i, tag 01
			//TODO:Check if we have enough cores
			//TODO:Also send the maximizer state, childRange
			MPI_Send(&boards[i], 1,mpi_board_type, i+1, 01, MPI_COMM_WORLD);
		}
		for(int i =0; i < boards.size();i++){
			MPI_Recv(&boards[i], 1,mpi_board_type, MPI_ANY_SOURCE, 11, MPI_COMM_WORLD)
			// Get boards back from cores; might as well refill boards, tag 11
		}

		Board best;
		best.score =-10;
		for(auto i:boards){
			if(i.score>best.score){
				boardCopy(i,best);
			}
		}
		//TODO:print best board option

	}
	else{
		MPI_Status status;
		Board original;
		MPI_Recv(&original, 1,mpi_board_type, MPI_ANY_SOURCE, 01, MPI_COMM_WORLD,&status);
		int parentId = status.MPI_SOURCE;
		//TODO:get childRange, maximizer state
		// get board and childStartId, save parentId, save maximizer state, tag 01
		parallelSubMaster();
		//TODO:fill params ^
	}
	return 0;
}
