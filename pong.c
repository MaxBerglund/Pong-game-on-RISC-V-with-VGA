/* 
    pong.c
    By Max Berglund.
    Last modified: 2024-11-20
    This file is in the public domain.
*/

#define screen_width 640
#define screen_heigth 480
#define player_position 5

/* Coordinate variables */
int player1_x = player_position;
int player2_x = screen_width - player_position;
int player1_y = screen_heigth/2;
int player2_y = screen_heigth/2;
int ball_x = screen_width/2;
int ball_y = screen_heigth/2;

/* Velocity variables */
int player1_dy = 0;
int player2_dy = 0;
int ball_dx = 0;
int ball_dy = 0;

/* Size variables */
int paddle_width = 2;   // Indicates the width of each player paddle.
int ball_size = 1;      // Indicates the length of a side of the square ball.

/* Score variables */
int player1_score = 0;  // Displayed on the leftmost 7-segment display.
int player2_score = 0;  // Displayed on the rightmost 7-segement display.

/* Game state variables */
int game_state = 0;     // Game is active if 1, game is over if 0.
int game_time = 0;      // Amount of seconds since beginning of game.

void initialize_game() {
    player1_dy = player2_dy = 0;
    ball_dx = -1;
    ball_dy = 0;
    
    player1_y = player2_y = screen_heigth/2;
    ball_x = screen_width/2;
    ball_y = screen_heigth/2;

    player1_score = player2_score = 0;

    game_time = 0;
    game_state = 1;
}

/* Increments the score and updates the 7-segments display. */
int increment_score(player_number) {
    // Increment the relevant players score

    /*
    if(value == 0) *displayPointer = 0b01000000;
    if(value == 1) *displayPointer = 0b01111001;
    if(value == 2) *displayPointer = 0b00100100;
    if(value == 3) *displayPointer = 0b00110000;
    if(value == 4) *displayPointer = 0b00011001;
    if(value == 5) *displayPointer = 0b00010010;
    if(value == 6) *displayPointer = 0b00000010;
    if(value == 7) *displayPointer = 0b01111000;
    if(value == 8) *displayPointer = 0b00000000;
    if(value == 9) *displayPointer = 0b00010000;
    */
}


int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}



/*

Notes:
- If the first switch is active, move the player2 paddle up. 
- If the second switch is active, move the player2 paddle down.
- If none or both of the first and second switches are active, do nothing.
- Vice versa for player1 with the tenth and ninth switch.

- Should make a "restart" function which restarts the game. Is called upon when the push button is pressed.

- If the balls coordinates are above or below the screen, bounce. 
- If the balls coordinates are to the right of the screen, increment player1_score and call the restart function. Vice versa for the left side of the screen.

*/




/*

Ideas for special game modes:
- MEGA-BALL: Makes the ball much bigger.
- FAST-BALL: Ball becomes progressively faster during the course of the game
- REVERSE-PADDLES: The opponents switch controls are reversed when the third switch is used.
- PRECISION-PONG: Makes the paddles much smaller so that you must be more precise in blocking the ball.

*/
