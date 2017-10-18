#include <iostream>
#include <ctime>
#include <string>
#include <Windows.h>

//mains//
void loop(char **board, int **positions, int y, int x); //main game loop where the functions repeat every cycle
void draw(char **board, int **positions, int y, int x); //puts everything onto the command prompt
void logic(char **board, int **positions, int y, int x, int movement, int &player_location, int &level, int &moveStatus); //where logic happens
																														  //subs//
void movement(int &movement, bool playerFlag); //where player makes a move
void fill(char **board, int **positions, int y, int x); //fills the array
void fillLadder(char **board, int **positions, int y, int x, int length); //fill and draw ladder
void fillSnake(char **board, int **positions, int y, int x, int length); //fill and draw snake
bool checkWin(int y, int x, int player_location, int level); //where a win is determined
void updatePlayerLevel(int **positions, int y, int x, int player_location, int &level, int &moveStatus); //where player level updates if snake/ladder hit

using namespace std;

//store player variables
struct Player {
	//dictates player movement on the board
	int movement, location, level;
	//dictates which direction the player will move: 1 = right, -1 = left
	int movestatus = 1;
	//representation of player on board
	char symbol;
	//determines if player can play
	bool on = false;
};

Player player1;
Player player2;

int main()
{
	cout << "---Snakes & Ladders---" << endl;
	srand(time(0));

	int size_y;
	int size_x;

	cout << "What game size do you want? Pick the number of rows and columns." << endl;
	cout << "For balance, best size is 10x10." << endl;
	cout << "Rows:";
	cin >> size_y;
	cout << "Columns:";
	cin >> size_x;

	int flag;
	cout << "Press 2 if you want to play multiplayer (2 player). Press 1 for singleplayer." << endl;
	cin >> flag;
	if (flag == 2)
		player2.on = true;

	cout << "Player 1: Select a letter/number on the keyboard for your character." << endl;
	cin >> player1.symbol;
	if (player2.on == true)
	{
		cout << "Player 2: Select a letter/number on the keyboard for your character." << endl;
		cin >> player2.symbol;
	}

	system("CLS"); //clears command prompt

	//create an 2D array to store board chars for drawing
	char **board;
	board = new char *[size_y];
	//create an 2D array to store board position values 
	int **position_status;
	position_status = new int *[size_y];

	for (int i = 0; i < size_y; i++)
	{
		board[i] = new char[size_x];
		position_status[i] = new int[size_x];
	}


	fill(board, position_status, size_y, size_x);
	draw(board, position_status, size_y, size_x);
	loop(board, position_status, size_y, size_x);

	char stop;
	cout << "Press any character to end the programme" << endl;
	cin >> stop;

	//when game ends, this code executes
	//delete the pointers
	for (int i = 0; i < size_y; i++)
	{
		delete[] board[i];
		delete[] position_status[i];
	}
	delete board;
	delete position_status;

	return 0;
}


void loop(char **board, int **positions, int y, int x)
{
	//initalise x location of players
	player1.location = 0;
	player2.location = 0;
	//initalise y location of players
	player1.level = y - 1;
	player2.level = y - 1;
	//game flag
	bool game = true;

	while (game == true)
	{
		movement(player1.movement, true); //allow player to make move
		logic(board, positions, y, x, player1.movement, player1.location, player1.level, player1.movestatus); //update location & level

		if (checkWin(y, x, player1.location, player1.level) == true) //check if player won the game
		{
			cout << "Player 1 won the game." << endl;
			game = false;
		}
		else //if not 
		{
			updatePlayerLevel(positions, y, x, player1.location, player1.level, player1.movestatus); //check if player has hit a snake or ladder
			board[player1.level][player1.location] = player1.symbol; //fill board with new player location
			draw(board, positions, y, x); //draw the board
		}
		if (player2.on == true)
		{
			movement(player2.movement, false);
			logic(board, positions, y, x, player2.movement, player2.location, player2.level, player2.movestatus);

			if (checkWin(y, x, player2.location, player2.level) == true)
			{
				cout << "Player 2 won the game." << endl;
				game = false;
			}
			else
			{
				updatePlayerLevel(positions, y, x, player2.location, player2.level, player2.movestatus);
				board[player2.level][player2.location] = player2.symbol;
				draw(board, positions, y, x);
			}
		}
	}
}

void draw(char **board, int **positions, int y, int x)
{
	//draw the starting and finishing point
	if (y % 2 != 0)
		board[0][x - 1] = 'O';
	else
		board[0][0] = 'O';

	board[y - 1][0] = 'O';

	//draw the level numbers
	for (int i = 0; i < y; i++)
	{
		if ((y - i) > 9)
			cout << y - i << " |";
		else
			cout << y - i << "  |";
		for (int y = 0; y < x; y++)
		{
			cout << board[i][y] << " ";
		}
		cout << endl;
	}

	if (y < 8)
	{
		//just two ladders and one snake
		fillLadder(board, positions, y - 1, 2, y / 2);
		fillLadder(board, positions, y / 2, 4, 2);
		fillSnake(board, positions, 0, 1, -1 * y / 2 + 1);
	}
	if (y >= 8 && y <= 12)
	{
		//just three ladders and three snakes
		fillLadder(board, positions, y - 1, 3, 3);
		fillLadder(board, positions, 4, x - 3, 2);
		fillLadder(board, positions, 2, x - 2, 2);
		fillSnake(board, positions, 0, 4, -4);
		fillSnake(board, positions, 4, x - 4, -4);
		fillSnake(board, positions, 4, 0, -3);
	}

	if (y > 12)
	{
		//just 4 ladders and 4 snakes
		fillLadder(board, positions, y - 1, 3, 6);
		fillLadder(board, positions, 6, x - 3, 5);
		fillLadder(board, positions, 7, x - 2, 6);
		fillLadder(board, positions, 9, 3, 3);
		fillSnake(board, positions, 0, 4, -7);
		fillSnake(board, positions, 4, x - 4, -6);
		fillSnake(board, positions, 7, 0, -3);
		fillSnake(board, positions, 5, 2, -2);
	}

}

void logic(char **board, int **positions, int y, int x, int movement, int &player_location, int &level, int &moveStatus)
{
	int remainder;
	int previous_location = player_location;
	int previous_level = level;

	if (moveStatus == 1)
	{
		player_location += movement; //if status is set to right, move right
	}
	if (moveStatus == -1)
	{
		player_location -= movement; //if status is set to left, move left
	}

	if (player_location > x - 1) //if too far from board, update the level and move accordingly
	{
		if (level != 0)
		{
			moveStatus = -1;
			remainder = player_location - (x);
			player_location = (x - 1) - remainder;
			level--;
		}
	}

	if (player_location < 0) //if too far from board, update the level and move accordingly
	{
		if (level != 0)
		{
			moveStatus = 1;
			remainder = x - 2 - player_location - (x - 1);
			player_location = remainder;
			level--;
		}
	}

	board[previous_level][previous_location] = '-'; //fill last position of player with a dash
}

void movement(int &movement, bool playerFlag)
{
	//true means player 1
	//false means player 2
	int diceRoll;
	int status = 0;
	if (playerFlag == true)
	{
		cout << "Player 1: Press 1 to roll the dice." << endl;
		cin >> status;
		if (status == 1)
		{
			diceRoll = 1 + rand() % 6;
			movement = diceRoll;
			cout << "Player 1 rolled a " << diceRoll << endl;
		}
	}
	if (playerFlag == false)
	{
		cout << "Player 2: Press 2 to roll the dice." << endl;
		cin >> status;
		if (status == 2)
		{
			diceRoll = 1 + rand() % 6;
			movement = diceRoll;
			cout << "Player 2 rolled a " << diceRoll << endl;
		}
	}
}
void fill(char **board, int **positions, int y, int x)
{
	//fill the array with dashes to mark a place
	//fill the position array with default 0s... positive number means ladder, negative number means snake
	for (int i = 0; i < y; i++)
	{
		for (int w = 0; w < x; w++)
		{
			board[i][w] = '-';
			positions[i][w] = 0;
		}
	}

	//fill the array with a start and finish point, both denoted by O
	if (y % 2 != 0)
		board[0][x - 1] = 'O';
	else
		board[0][0] = 'O';

	board[y - 1][0] = 'O';

	//fill board and position arrays with snakes and ladders
	if (y < 8)
	{
		//just two ladders and one snake
		fillLadder(board, positions, y - 1, 2, y / 2);
		fillLadder(board, positions, y / 2, 4, 2);
		fillSnake(board, positions, 0, 1, -1 * y / 2 + 1);
	}
	if (y >= 8 && y <= 12)
	{
		//just three ladders and three snakes
		fillLadder(board, positions, y - 1, 3, 3);
		fillLadder(board, positions, 4, x - 3, 2);
		fillLadder(board, positions, 2, x - 2, 2);
		fillSnake(board, positions, 0, 4, -4);
		fillSnake(board, positions, 4, x - 4, -4);
		fillSnake(board, positions, 4, 0, -3);
	}

	if (y > 12)
	{
		//just 4 ladders and 4 snakes
		fillLadder(board, positions, y - 1, 3, 6);
		fillLadder(board, positions, 6, x - 3, 5);
		fillLadder(board, positions, 7, x - 2, 6);
		fillLadder(board, positions, 9, 3, 3);
		fillSnake(board, positions, 0, 4, -7);
		fillSnake(board, positions, 4, x - 4, -6);
		fillSnake(board, positions, 7, 0, -3);
		fillSnake(board, positions, 5, 2, -2);
	}

}


bool checkWin(int y, int x, int player_location, int level)
{
	//if player reaches the winning location, return true.
	//winning location depends if board y-axis is even or odd
	if (player_location <= 0 && level == 0)
		return true;

	if (player_location >= x - 1 && level == 0)
		return true;

	return false;
}

void fillLadder(char **board, int **positions, int y, int x, int length)
{
	//fill positions array with value of ladder
	//fill board array with L to denote ladder positions
	positions[y][x] = length;

	for (int i = 0; i < length; i++)
	{
		board[y - i][x] = 'L';
	}
}

void fillSnake(char **board, int **positions, int y, int x, int length)
{
	//fill positions array with value of snake
	//fill board array with S to denote snake positions
	positions[y][x] = length;

	length = length * -1;

	for (int i = 0; i < length; i++)
	{
		board[y + i][x] = 'S';
	}
}

void updatePlayerLevel(int **positions, int y, int x, int player_location, int &level, int &moveStatus)
{
	//if a player hits a ladder and IS NOT WINNING, update level
	if (checkWin(y, x, player_location, level) == false)
	{
		if (*(*(positions + level) + player_location) > 0)
		{
			cout << "You hit a ladder" << endl;
			int length = positions[level][player_location];
			level = level - length;
			if (length % 2 != 0)
			{
				moveStatus = moveStatus * -1;
			}
		}
		//if a player hits a snake and IS NOT WINNING, update level
		if (*(*(positions + level) + player_location) < 0)
		{
			cout << "You hit a snake" << endl;
			int length = positions[level][player_location];
			level = level - length;
			if (length % 2 != 0)
			{
				moveStatus = moveStatus * -1;
			}
		}
	}
}
