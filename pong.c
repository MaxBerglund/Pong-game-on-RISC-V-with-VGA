/* 
    pong.c
    By Max Berglund.
    Last modified: 2024-11-21
    This file is in the public domain.
*/

#include <math.h>

#define screen_width 320
#define screen_heigth 240
#define player_position 8
#define PI 3.14159

/* Coordinate variables */
// int player1_x = player_position;
// int player2_x = screen_width - player_position;
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

/**
 * Resets the global variables and starts the game from the starting point.
 */
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

/**
 * Increments the score of the given player_number and updates the 7-segments display.
 * Parameter: player_number is either 1 or 2. Does nothing otherwise.
 */
int increment_score(player_number) {
    if (player_number == 1) {
        volatile int *displayPointer = (volatile int*) 0x04000050; // Creates a pointer that points to the memory adress where the 7-segment displays are. It is volatile so that the compiler doesn't do any unneccessary optimisations that might alter the behaviour of the dtek-board.
        player1_score++;

        if(player1_score == 0) *displayPointer = 0b01000000;
        if(player1_score == 1) *displayPointer = 0b01111001;
        if(player1_score == 2) *displayPointer = 0b00100100;
        if(player1_score == 3) *displayPointer = 0b00110000;
        if(player1_score == 4) *displayPointer = 0b00011001;
        if(player1_score == 5) *displayPointer = 0b00010010;
        if(player1_score == 6) *displayPointer = 0b00000010;
        if(player1_score == 7) *displayPointer = 0b01111000;
        if(player1_score == 8) *displayPointer = 0b00000000;
        if(player1_score == 9) *displayPointer = 0b00010000;
    }

    if (player_number == 2) {
        volatile int *displayPointer = (volatile int*) 0x04000050 + 4; // Creates a pointer that points to the memory adress where the 7-segment displays are. It is volatile so that the compiler doesn't do any unneccessary optimisations that might alter the behaviour of the dtek-board.
        player2_score++;

        if(player2_score == 0) *displayPointer = 0b01000000;
        if(player2_score == 1) *displayPointer = 0b01111001;
        if(player2_score == 2) *displayPointer = 0b00100100;
        if(player2_score == 3) *displayPointer = 0b00110000;
        if(player2_score == 4) *displayPointer = 0b00011001;
        if(player2_score == 5) *displayPointer = 0b00010010;
        if(player2_score == 6) *displayPointer = 0b00000010;
        if(player2_score == 7) *displayPointer = 0b01111000;
        if(player2_score == 8) *displayPointer = 0b00000000;
        if(player2_score == 9) *displayPointer = 0b00010000;
    }
}

/**
 * Rotate the ball vector by the given amount of degrees.
 * The method that is used to do this is by multiplying the vector with the rotation matrix for a 2D vector.
 */
void rotate_ball_vector(int degrees) {
    degrees *= PI / 180;                                        // Convert the amount of degrees to radians.
    ball_dx = ball_dx * cos(degrees) - ball_dy * sin(degrees);  // Calculate the x-vector after rotation.
    ball_dy = ball_dx * sin(degrees) + ball_dy * cos(degrees);  // Calculate the y-vector after rotation.
}

/**
 * Handles collisions for the ball by changing its direction.
 */
void handle_collison() {
    if (ball_dy == 0) {             // Case to handle when the ball is moving in a straight line along the x-axis.
        ball_dx = -ball_dx;         // Immediately rotate the ball vector 180 degrees.
    } else if (ball_dx == 0) {      // Case to handle when the ball is moving in a straight line along the y-axis.
        int random = rand() % 2;    // Generates a random number that is either 0 or 1
        if(random) {
            rotate_ball_vector(5);  // Rotate counter-clockwise.
        } else {
            rotate_ball_vector(-5); // Rotate clockwise.
        }
    } else if (ball_dy >= 0) {      // Case to handle when the move is moving upwards.
        rotate_ball_vector(90);     // Rotate 90 degrees counter-clockwise.
    } else if (ball_dy <= 0) {      // Case to handle when the move is moving downwards.
        rotate_ball_vector(-90);    // Rotate 90 degrees clockwise.
    } 
    // Do we need an else statement? Perhaps teleport the ball to the center if something unforeseen happens
    // Should we make it so that the ball rotates a different amount of degrees depending on where it hits the paddles OR the speed of the paddles?
}

/**
 * Move the ball one step along its velocity vector.
 */
void move_ball() {
    ball_x += ball_dx;  // Move the ball along the x-axis by its corresonding motion vector.
    ball_y += ball_dy;  // Move the ball along the y-axis by its corresonding motion vector.

    if(ball_y == 0 || ball_y == screen_heigth) handle_collision();                                                                                      // Case when ball collides with the upper or lower wall.
    if(ball_x == player_position && (ball_y == player1_y + paddle_width/2 || ball_y == player1_y - paddle_width/2)) handle_collision();                 // Case when the ball collides with player 1's paddle.
    if(ball_x == screen_width - player_position && (ball_y == player2_y + paddle_width/2 || ball_y == player2_y - paddle_width/2)) handle_collision();  // Case when the ball collides with player 2's paddle.
}

/**
 * Moves the paddles one step along their motion vectors if the relevant switches are active.
 */
void move_paddles() {
    player1_y += player1_dy;
    player2_y += player2_dy;
    // TODO
    // Implement a stop when the paddle edge reaches the end of the screen.
    // Should we make the paddles accelerate instead of instantaneously reaching a certain speed?
    // Implement the REVERSE-PADDLES special game mode by subtracting the velocity instead of adding it.
}

int main(int argc, char const *argv[]) {
    initialize_game();

    /*
    Check if game state is active (1). *should we use this inefficent polling method?*
    Read from the push-button. Run initialize_game() if it is pressed to reset the game.
    Read from the switches. Update all velocities according to the outer switch values.
    Update the game mode if indicated by the switches.

    Move the ball. If the ball has the same x-coordinate as either of the left or right walls (goals), increment the relevant player's score and reset the ball.
    */

    move_ball();                        // Moves the ball and handles collisions.

    if(ball_x == 0) {                   // Case if player 2 scores.
        increment_score(2);             // Increment the score of player 2.
        ball_dx = -1;                   // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;        // Reset the ball position.
        ball_y = screen_heigth/2;
    } else if (ball_x == screen_width) { // Case if player 1 scores.
        increment_score(1);             // Increment the score of player 1.
        ball_dx = -1;                   // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;        // Reset the ball position.
        ball_y = screen_heigth/2;
    }
    
    /*
    Move the paddles, according to the switches
    Check if collision. If true, rotate the ball velocity vector 90 degrees to the right if ball_dy is positive, otherwise rotate it 90 degrees to the left.

    Game over, set game_state to 0
    Display game over on the screen
    Restart the game using initialize_game() if the push-button is pressed
    */

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

- Should we make it so that the ball turns more sharply when different parts of the paddle are hit?
- Should we instead make it so that the ball turns more sharply when the paddle hits it with speed?
- Should we make it so that the paddles acutally accelerate to amplify this effect, instead of just letting them reach their speed instantaneously?
*/

/*

Ideas for special game modes:
- MEGA-BALL: Makes the ball much bigger.
- FAST-BALL: Ball becomes progressively faster during the course of the game
- REVERSE-PADDLES: The opponents switch controls are reversed when the third switch is used.
- PRECISION-PONG: Makes the paddles much smaller so that you must be more precise in blocking the ball.

*/
