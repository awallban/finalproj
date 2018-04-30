#include<bits/stdc++.h>
#include<iostream>
#include<ctime>
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
bool isMovesLeft(char board[3][3]){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(board[i][j] == '_'){
				return true;
			}
		}
	}
	return false;
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
int minimax(char board[3][3], int depth, bool isMax, int alpha, int beta){
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
	// THIS IS WEIRD!!!!
	// TODO FIX THIS?
	if(isMovesLeft(board)==false){
		return 0;
	}
	// now, for all other cases (I.E., game's still goin'...)
	
	//if maximizer's turn, use minimax to find best move
	if (isMax){
		// if you're maximizer, find the top score you can get by minimaxing all possible board positions recursively
		int v = alpha;
		//iterate through the board
		//// TODO check v>B
		for(int i = 0;i < 3;i++){
			for (int j = 0;j < 3;j++){
				//make your move
				if(board[i][j]=='_'){
					//board[i][j]=player;
					//call minimax, alphabeta (recursion), choose best possible move
					if(v<beta){
						board[i][j]=player;
						v = max(v,minimax(board,depth+1,!isMax,alpha,beta));
						board[i][j]='_';
						//beta = v;
					}
					board[i][j]='_';
					
				}
			}
		}
		return v;
	}
	//if minimizer's turn, use minimax to find best move
	else {
		// if you're the minimizer, find the least score you can by minimaxing all possible board positions recursively
		int v = beta;
		//iterate through the board
		for(int i = 0;i < 3;i++){
			for (int j = 0;j < 3;j++){
				//make your move
				if(board[i][j]=='_'){
					board[i][j]=opponent;
					if(v>alpha){
						//call minimax (recursion), choose best possible move
						board[i][j]=opponent;
						v = min(v,minimax(board,depth+1,!isMax,alpha,beta));
						board[i][j]='_';
					}
					board[i][j] ='_';
				}
			}
		}
		return v;
	}
}

// current player's best move function
// ================================================
Move findBestMove(char board[3][3],bool isPlayer){
	int bestMaxVal = -100;
	int bestMinVal = 100;
	Move bestMove;
	bestMove.row=-1;
	bestMove.col=-1;
	//look through board, run minimax on all cells, return best possible move.
	for(int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			//player tries a move
			if(board[i][j] == '_' && isPlayer==true){
				board[i][j] = player;
				//was this a good move? check by seeing what the minimizing player's reaction
				//would be for player's attempted move
				int moveVal = minimax(board,0,false,bestMaxVal,bestMinVal);

				board[i][j]='_';

				//update bestMove
				if(moveVal > bestMaxVal){
					bestMove.row=i;
					bestMove.col=j;
					bestMaxVal=moveVal;
				}
			}
			
			//opponent tries a move
			else if(board[i][j] == '_' && isPlayer==false){
				board[i][j] = opponent;
				//was this a good move? check by seeing what the minimizing player's reaction
				//would be for player's attempted move
				int moveVal = minimax(board,0,true,bestMaxVal,bestMinVal);

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
	if(isPlayer==true){
		cout << "Best move value is:" << bestMaxVal << "for PLAYER" << endl;
	}
	else {
		cout << "Best move value is:" << bestMinVal << "for OPPONENT" << endl;
	}
	return bestMove;
}
// MAIN CODE
// ==============================
// ==============================
int main(){
	char board[3][3] = 
	{
		{'_','_','_'},
		{'_','_','_'},
		{'_','_','_'}
	};
	bool currPlayer = true;
	int gameState = 0;
	bool isBoardGood = true;
	clock_t t;
	// print initial board:
	cout << "Initial board: " << endl;
	for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++){
				cout << board[j][k] << '|';
			}
			cout << endl;
		}
	t = clock();
	// begin playing the game
	while((gameState==0)&&(isBoardGood == true)){
		Move bestMove = findBestMove(board,currPlayer);

		cout << "The best move on the board to make is: " << "ROW: " << bestMove.row << " COL: " << bestMove.col << endl;
		if(currPlayer == true){
			board[bestMove.row][bestMove.col]=player;
		}
		else {
			board[bestMove.row][bestMove.col]=opponent;
		}
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++){
				cout << board[j][k] << '|';
			}
			cout << endl;
		}
		// check if game over
		gameState = winningBoard(board);
		// check if board full
		isBoardGood = isMovesLeft(board);
		// switch players
		currPlayer = !currPlayer;
	}
	t = clock()-t;
	cout << "Elapsed processing time: " << ((float)t)/CLOCKS_PER_SEC << " seconds." << endl;
	return 0;
}
