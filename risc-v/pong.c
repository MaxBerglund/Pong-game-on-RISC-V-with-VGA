/* 
    pong.c
    By Max Berglund.
    Last modified: 2024-11-25
    This file is in the public domain.
*/

#include <math.h>

#define screen_width 320
#define screen_heigth 240
#define player_position 8
#define initial_ball_velocity -1
#define initial_ball_size 1
#define initial_paddle_height 8
#define player_velocity 5
#define PI 3.14159

/* Coordinate variables */
// int player1_x = player_position;                 // NOT NEEDED
// int player2_x = screen_width - player_position;  // NOT NEEDED
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
int paddle_height = initial_paddle_height;    // Indicates the width of each player paddle.
int ball_size = initial_ball_size;  // Indicates the length of a side of the square ball.

/* Score variables */
int player1_score = 0;              // Displayed on the leftmost 7-segment display.
int player2_score = 0;              // Displayed on the rightmost 7-segement display.

/* Game state variables */
int game_state = 0;                 // Game is active if 1, game is over if 0.
int game_time = 0;                  // Amount of seconds since beginning of game.
int reverse_paddles1 = 0;           // Special game mode that reverses the control of the paddles for player 1.
int reverse_paddles2 = 0;           // Special game mode that reverses the control of the paddles for player 2.
int fast_ball = 0;                  // Special game mode that makes the ball progressively faster during the course of the game.

/*
Special game modes:
- MEGA-BALL: Makes the ball much bigger.
- FAST-BALL: Ball becomes progressively faster during the course of the game
- REVERSE-PADDLES: The opponents switch controls are reversed when the third switch is used.
- PRECISION-PONG: Makes the paddles much smaller so that you must be more precise in blocking the ball.
*/


/**
 * Resets the global variables and starts the game from the starting point.
 */
void initialize_game() {
    player1_dy = player2_dy = 0;
    ball_dx = initial_ball_velocity;
    ball_dy = 0;
    
    player1_y = player2_y = screen_heigth/2;
    ball_x = screen_width/2;
    ball_y = screen_heigth/2;

    player1_score = player2_score = 0;

    game_time = 0;
    game_state = 1;
    initialize_game_time();
}

/**
 * Increments the score of the given player_number and updates the 7-segments display.
 * Parameter: player_number is either 1 or 2. Does nothing otherwise.
 */
void increment_score(int player_number) {
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
void handle_collision() {
    if (ball_dy == 0) {                                 // Case to handle when the ball is moving in a straight line along the x-axis.
        ball_dx = -ball_dx;                             // Immediately rotate the ball vector 180 degrees.
    } else if (ball_dx == 0) {                          // Case to handle when the ball is moving in a straight line along the y-axis.
        int random = rand() % 2;                        // Generates a random number that is either 0 or 1
        if(random) {
            rotate_ball_vector(5);                      // Rotate counter-clockwise.
        } else {
            rotate_ball_vector(-5);                     // Rotate clockwise.
        }
    } else if (ball_dy >= 0) {                          // Case to handle when the move is moving upwards.
        if(ball_y == 0 || ball_y == screen_heigth) {    // Case to handle when the ball collided with the upper or lower wall.
            rotate_ball_vector(90);                     // Rotate 90 degrees counter-clockwise.
        } else {
            rotate_ball_vector(-90);                    // Rotate 90 degrees clockwise.
        }
    } else if (ball_dy <= 0) {                          // Case to handle when the move is moving downwards.
        if(ball_y == 0 || ball_y == screen_heigth) {    // Case to handle when the ball collided with the upper or lower wall.
            rotate_ball_vector(-90);                    // Rotate 90 degrees clockwise.
        } else {
            rotate_ball_vector(90);                     // Rotate 90 degrees counter-clockwise.
        }
    } else {                                            // Else statement to handle the case when something unforeseen happens.
        ball_x = screen_width/2;                        // Reset the ball x-coordinate.
        ball_y = screen_heigth/2;                       // Reset the ball y-coordinate.
        ball_dx = initial_ball_velocity;                // Reset the ball velocity along the x-axis.
        ball_dy = 0;                                    // Reset the ball velocity along the y-axis.
    }
    // Should we make it so that the ball rotates a different amount of degrees depending on where it hits the paddles OR the speed of the paddles?
}

/**
 * Move the ball one step along its velocity vector.
 */
void move_ball() {
    if(fast_ball) {
        ball_dx += 0.1;
        ball_dy += 0.1;
    }
    ball_x += ball_dx;  // Move the ball along the x-axis by its corresonding motion vector.
    ball_y += ball_dy;  // Move the ball along the y-axis by its corresonding motion vector.

    if(ball_y == 0 || ball_y == screen_heigth) handle_collision();                                                                                      // Case when ball collides with the upper or lower wall.
    if(ball_x == player_position && (ball_y == player1_y + paddle_height/2 || ball_y == player1_y - paddle_height/2)) handle_collision();                 // Case when the ball collides with player 1's paddle.
    if(ball_x == screen_width - player_position && (ball_y == player2_y + paddle_height/2 || ball_y == player2_y - paddle_height/2)) handle_collision();  // Case when the ball collides with player 2's paddle.
    
    if(ball_x == 0) {                       // Case if player 2 scores.
        increment_score(2);                 // Increment the score of player 2.
        ball_dx = initial_ball_velocity;    // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;            // Reset the ball position.
        ball_y = screen_heigth/2;
    } else if (ball_x == screen_width) {    // Case if player 1 scores.
        increment_score(1);                 // Increment the score of player 1.
        ball_dx = initial_ball_velocity;    // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;            // Reset the ball position.
    }
}

/**
 * Moves the paddles one step along their motion vectors if the relevant switches are active.
 */
void move_paddles() {
    /* Should the following function be removed because it is included in the later ones? */
    /*
    if (reverse_paddles1) {         // Checks if the special game mode REVERSE PADDLES is activated for player 1.
        player1_y -= player1_dy;    // Moves the paddle in the opposite intended direction by its motion vector.
    } else {
        player1_y += player1_dy;    // Moves the paddle in the intended direction by its motion vector.
    }

    if (reverse_paddles2) {         // Checks if the special game mode REVERSE PADDLES is activated for player 2.
        player2_y -= player2_dy;    // Moves the paddle in the opposite intended direction by its motion vector.
    } else {
        player2_y += player2_dy;    // Moves the paddle in the intended direction by its motion vector.
    }
    */

    /* The following code ensures that the paddles cannot leave the screen border. */
    if (player1_y - paddle_height/2 > 0 && player1_y + paddle_height/2 < screen_heigth) {     // Checks that player 1 is within the screen borders.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            player1_y -= player1_dy;                                                        // Moves the paddle in the unintended direction by its motion vector.
        } else {
            player1_y += player1_dy;                                                        // Moves the paddle in the intended direction by its motion vector.
        }
    } else if (player1_y - paddle_height/2 < 0) {                                            // Checks if the player 1 paddle is below the screen.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player1_dy < 0) player1_y -= player1_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's upwards.
        } else {
            if (player1_dy > 0) player1_y += player1_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's upwards.
        }
    } else if (player1_y + paddle_height/2 > screen_heigth) {                                // Checks if the player 1 paddle is above the screen.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player1_dy > 0) player1_y -= player1_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's downwards.
        } else {
            if (player1_dy < 0) player1_y += player1_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's downwards.
        }
    }

    if (player2_y - paddle_height/2 > 0 && player2_y + paddle_height/2 < screen_heigth) {     // Checks that player 2 is within the screen borders.
        if (reverse_paddles2) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            player2_y -= player2_dy;                                                        // Moves the paddle in the unintended direction by its motion vector.
        } else {
            player2_y += player2_dy;                                                        // Moves the paddle in the intended direction by its motion vector.
        }
    } else if (player2_y - paddle_height/2 < 0) {                                            // Checks if the player 2 paddle is below the screen.
        if (reverse_paddles2) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player2_dy < 0) player2_y -= player2_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's upwards.
        } else {
            if (player2_dy > 0) player2_y += player2_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's upwards.
        }
    } else if (player2_y + paddle_height/2 > screen_heigth) {                                // Checks if the player 2 paddle is above the screen.
        if (reverse_paddles2) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player2_dy > 0) player2_y -= player2_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's downwards.
        } else {
            if (player2_dy < 0) player2_y += player2_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's downwards.
        }
    }

    /* Below is a more hardcoded solution that just resets the paddle position if it surpasses the screen border. REVERSE PADDLES is not included. */
    /*
    if (player1_y - paddle_height/2 < 0) {   // Checks that player 1 paddle is within the screen border and resets it otherwise.
        player1_y = 0 + paddle_height/2;
    } else if (player1_y + paddle_height/2 > screen_heigth) {
        player1_y = screen_heigth - paddle_height/2;
    }

    if (player2_y - paddle_height/2 < 0) {   // Checks that player 2 paddle is within the screen border and resets it otherwise.
        player2_y = 0 + paddle_height/2;
    } else if (player2_y + paddle_height/2 > screen_heigth) {
        player2_y = screen_heigth - paddle_height/2;
    }
    */

    // Should we make the paddles accelerate instead of instantaneously reaching a certain speed?
}



/**
 * Returns the integer value which contains the statuses of the toggle-switches on the board.
 * For example, if the first and the second switches are turned on while all other switches are off, this function will return 0b0000000011.
 */
int get_sw( void ) {
  volatile int *switchPointer = (volatile int*) 0x04000010; // Creates a pointer that points to the memory adress where the toggle-switches are. It is volatile so that the compiler doesn't do any unneccessary optimisations that might alter the behaviour of the RISC-V board.
  return *switchPointer;
}

/**
 * Returns an integer which holds the status of the second button.
 */
int get_btn( void ) {
  volatile int *buttonPointer = (volatile int*) 0x040000d0; // Creates a pointer that points to the memory adress where the second push-button is. It is volatile so that the compiler doesn't do any unneccessary optimisations that might alter the behaviour of the RISC-V board.
  return *buttonPointer;
}

/**
 * Isolates a bit in a binary number. 
 * Parameter: value, the value to extract the digit from.
 * Parameter: digitNumber, is the zero indexed position of the digit to be extracted. For example, 0 extracts the first digit of value and 1 extracts the second digit of value.
 */
int get_digit(int value, int digitNumber) {
  return (value >> (digitNumber)) & 1;  // Rightshift value so that the digit we want is signified by the least significant bits, and then isolate that bit by a logical bitwise AND operation with 1.
}

/**
 * Sets the velocity for the paddles of both player 1 and 2 according to the value of the switches on the RISC-V board.
 */
void set_paddles_velocity () {
    int switchValues = get_sw();                                                                            // The integer value which contains the statuses of the toggle-switches on the board.

    if (get_digit(switchValues, 0) == 1 && get_digit(switchValues, 1) == 0) player2_dy = player_velocity;   // If the first switch is active while the second is not, move player 2 upward.
    if (get_digit(switchValues, 0) == 0 && get_digit(switchValues, 1) == 1) player2_dy = -player_velocity;  // If the first switch is inactive while the second is active, move player 2 downward.
    if ((get_digit(switchValues, 0) == 0 && get_digit(switchValues, 1) == 0) || (get_digit(switchValues, 0) == 1 && get_digit(switchValues, 1) == 1)) player2_dy = 0;   // If both the first and second switches have the same state, make player 2 stationary.
            
    if (get_digit(switchValues, 9) == 1 && get_digit(switchValues, 8) == 0) player1_dy = player_velocity;  // If the tenth switch is active while the ninth is not, move player 1 upward.
    if (get_digit(switchValues, 9) == 0 && get_digit(switchValues, 8) == 1) player1_dy = -player_velocity; // If the tenth switch is inactive while the ninth is active, move player 1 downward.
    if ((get_digit(switchValues, 9) == 0 && get_digit(switchValues, 8) == 0) || (get_digit(switchValues, 9) == 1 && get_digit(switchValues, 8) == 1)) player1_dy = 0;  // If both the tenth and ninth switches have the same state, make player 1 stationary.

}

/**
 * Sets the values of the variables handling the special game modes according to the value of the switches on the RISC-V board.
 * This method currently makes use of an inefficent polling method and could be improved.
 */
void set_special_game_modes () {
    int switchValues = get_sw();        // The integer value which contains the statuses of the toggle-switches on the board.

    if (get_digit(switchValues, 2)) {   // If switch 3 is active, reverse the controls of player 1.
        reverse_paddles1 = 1;
    } else {
        reverse_paddles1 = 0;
    }

    if (get_digit(switchValues, 7)) {   // If switch 8 is active, reverse the controls of player 2.
        reverse_paddles2 = 1;
    } else {
        reverse_paddles2 = 0;
    }

    if (get_digit(switchValues, 3)) {   // If switch 4 is active, MEGA-BALL.
        ball_size = 30;
    } else {
        ball_size = initial_ball_size;
    }

    if (get_digit(switchValues, 4)) {   // If switch 5 is active, PRECISION-PONG.
        paddle_height = 2;
    } else {
        paddle_height = initial_paddle_height;
    }

    if (get_digit(switchValues, 5)) {   // If switch 6 is active, FAST-BALL.
        fast_ball = 1;
    } else {
        fast_ball = 0;
    }
}

int main(int argc, char const *argv[]) {
    initialize_game();                      // Set up the global variables for the game.

    while (1) {                             // Main game loop.
        
        if (get_btn()) {
            initialize_game;                // If the push-button is pressed, reset the game. *Should we use this inefficent polling method? Maybe replace with interruption method, but only after we have made it work to avoid painstaking debugging.*
        } 

        if(game_state) {
            
            set_special_game_modes();       // Sets special game modes according to the states of the switches.
            
            set_paddles_velocity();         // Sets the velocity of the paddles according to the states of the switches.
            
            move_ball();                    // Moves the ball and handles collisions.
            
            move_paddles();                 // Moves the paddles according to the input of the switches.    

            if (player1_score >= 5) {
                // TODO "Print game_over, player 1 wins!"
                game_state = 0;
            }

            if (player2_score >= 5) {
                // TODO "Print game_over, player 2 wins!"
                game_state = 0;
            }
        }

        // Delay...

    }
    
    /*
    Check if game state is active (1). *should we use this inefficent polling method?*
    Read from the push-button. Run initialize_game() if it is pressed to reset the game.
    Read from the switches. Update all velocities according to the outer switch values.
    Update the game mode if indicated by the switches.

    Move the ball. If the ball has the same x-coordinate as either of the left or right walls (goals), increment the relevant player's score and reset the ball.               
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
