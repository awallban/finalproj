#include<bits/stdc++.h>
#include<iostream>
#include<ctime>
#include<mpi.h>
#include<vector>
using namespace std;

// GLOBAL VARIABLES
// =====================================================
// =====================================================
struct Move
{
	int row,col;
};

char player = 'x',opponent = 'o';

// FUNCTION DEFINITIONS
// =====================================================
// =====================================================

// empty board function (are there spaces left?)
// ==============================================
int isMovesLeft(char board[3][3]){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(board[i][j] == '_'){
				// return true
				return 1;
			}
		}
	}
	// return false
	return 0;
}
// winning board function (is the current board terminal?)
// =======================================================
int winningBoard(char b[3][3]){
	//check for row victories
	for(int row = 0; row < 3; row++){
		if(b[row][0]==b[row][1] && b[row][1]==b[row][2]){
			if(b[row][0]==player){
				return +10;
			}
			else if(b[row][0]==opponent){
				return -10;
			}
		}
	}
	//check for column victories
	for(int col = 0; col < 3; col++){
		if(b[0][col]==b[1][col] && b[1][col]==b[2][col]){
			if(b[0][col]==player){
				return +10;
			}
			else if (b[0][col]==opponent){
				return -10;
			}
		}
	}
	//check for diagonal victories
	if(b[0][0]==b[1][1] && b[1][1]==b[2][2]){
		if(b[0][0]==player){
			return +10;
		}
		else if(b[0][0]==opponent){
			return -10;
		}
	}
	if(b[0][2]==b[1][1] && b[1][1]==b[2][0]){
		if(b[0][2]==player){
			return +10;
		}
		else if(b[0][2]==opponent){
			return -10;
		}
	}
	//if no victories, return 0
	return 0;
}
// minimax function
// ====================================================
int minimax(char board[3][3], int depth, bool isMax){
	//check the current board
	int score = winningBoard(board);
	//if the maximizing player has won, return their score
	if(score==10){
		return score;
	}
	//if the minimizing player has won, return their score
	if(score==-10){
		return score;
	}
	//now, if it's a draw...
	if(isMovesLeft(board)==false){
		return 0;
	}
	// now, for all other cases (I.E., game's still goin'...)
	//if maximizer's turn, use minimax to find best move
	if (isMax){
		// if you're maximizer, find the top score you can get by minimaxing all possible board positions recursively
		int topScore = -100;
		//iterate through the board
		for(int i = 0;i < 3;i++){
			for (int j = 0;j < 3;j++){
				//make your move
				if(board[i][j]=='_'){
					board[i][j]=player;
					// call minimax
					topScore = max(topScore,minimax(board,depth+1,!isMax));
					board[i][j]='_';
					}					
				}
			}
			return topScore;
		}
	//if minimizer's turn, use minimax to find best move
	else {
		// if you're the minimizer, find the least score you can by minimaxing all possible board positions recursively
		int topScore = 100;
		//iterate through the board
		for(int i = 0;i < 3;i++){
			for (int j = 0;j < 3;j++){
				//make your move
				if(board[i][j]=='_'){
					board[i][j]=opponent;
					//call minimax recursively
					topScore=min(topScore,minimax(board,depth+1,!isMax));
					board[i][j]='_';
				}
			}
		}
			return topScore;
	}
}

// current player's best move function
// ================================================
Move findBestMove(char board[3][3],int isPlayer){
	int bestMaxVal = -100;
	int bestMinVal = 100;
	Move bestMove;
	bestMove.row=-1;
	bestMove.col=-1;
	//look through board, run minimax on all cells, return best possible move.
	for(int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			//player tries a move
			if(board[i][j] == '_' && isPlayer==1){
				board[i][j] = player;
				//was this a good move? check by seeing what the minimizing player's reaction
				//would be for player's attempted move
				int moveVal = minimax(board,0,false);
				board[i][j]='_';
				//update bestMove
				if(moveVal > bestMaxVal){
					bestMove.row=i;
					bestMove.col=j;
					bestMaxVal=moveVal;
				}
			}
			//opponent tries a move
			else if(board[i][j] == '_' && isPlayer==0){
				board[i][j] = opponent;
				//was this a good move? check by seeing what the minimizing player's reaction
				//would be for player's attempted move
				int moveVal = minimax(board,0,true);
				board[i][j]='_';
				//update bestMove
				if(moveVal < bestMinVal){
					bestMove.row=i;
					bestMove.col=j;
					bestMinVal=moveVal;
				}
			}
		}
	}
	// print out end result(win, lose, draw) for current player
	//if(isPlayer==1){
	//	cout << "Best move value is:" << bestMaxVal << "for PLAYER" << endl;
	//}
	//else {
	//	cout << "Best move value is:" << bestMinVal << "for OPPONENT" << endl;
	//}
	return bestMove;
}
// MAIN CODE
// ==============================
// ==============================
int main(int argc, char** argv){
	MPI_Init(NULL,NULL);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Status status;
	int coreScore = -9;
	int currPlayer = 1;
	int gameState = 0;
	int isBoardGood = 1;
	int results[8];
	int bestCore = 99;
	clock_t t;
	int row, col;
	char board[3][3] = 
		{
		{'_','_','_'},
		{'_','_','_'},
		{'_','_','_'}
		};
	//t = clock();
	if(world_rank == 0){
		Move bestMove = findBestMove(board,currPlayer);
		if(currPlayer == 1){
			board[bestMove.row][bestMove.col]=player;
		}
		else {
			board[bestMove.row][bestMove.col]=opponent;
		}
		
		row = bestMove.row;
		col = bestMove.col;
		// switch players
		currPlayer = !currPlayer;
	}
	t = clock();
	// tell other cores about needed information
	MPI_Bcast(&currPlayer,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&col,1,MPI_INT,0,MPI_COMM_WORLD);
	// every processor creates a board that has the initial move, and tries their independent move
	if(world_rank != 0){
		// copy board
		board[row][col]=player;
		// create new local board
		row = world_rank/3;
		col = world_rank%3;
		board[row][col] = opponent;
		currPlayer = !currPlayer;
	}
	// begin playing the game for every core
	while((gameState==0)&&(isBoardGood == 1)){
		Move bestMove = findBestMove(board,currPlayer);
		if(currPlayer == true){
			board[bestMove.row][bestMove.col]=player;
		}
		else {
			board[bestMove.row][bestMove.col]=opponent;
		}
		// check if game over
		gameState = winningBoard(board);
		// check if board full
		isBoardGood = isMovesLeft(board);
		// switch players
		currPlayer = !currPlayer;
	}
	t = clock()-t;
	coreScore = gameState;
	
	MPI_Gather(&coreScore,1,MPI_INT,&results,1,MPI_INT,0,MPI_COMM_WORLD);
	if(world_rank == 0){
		//int bestCore = 99;
		for (int i = 1; i < 8; i++){
			if(results[i]<bestCore){
				bestCore=results[i];
			}
		}
		//cout << "BEST CORE IS: " << i << " with score: " << bestCore << endl;
		cout << "Elapsed processing time for core "<< world_rank << " : " << ((float)t)/CLOCKS_PER_SEC << " seconds." << endl;
		
	}
	MPI_Bcast(&bestCore,1,MPI_INT,0,MPI_COMM_WORLD);
	if(world_rank == bestCore){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				cout << board[i][j] << "|";
			}
			cout << endl;
		}
	}
	MPI_Finalize();
	return 0;
}
