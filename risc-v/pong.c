/* 
    pong.c
    By Max Berglund.
    Last modified: 2024-11-25
    This file is in the public domain.
*/

#define screen_width 320
#define screen_height 240
#define player_position 8
#define initial_ball_velocity -3
#define initial_ball_size 3
#define initial_paddle_height 8
#define player_velocity 5
#define PI 3.14159

/* Coordinate variables */
// int player1_x = player_position;                 // NOT NEEDED
// int player2_x = screen_width - player_position;  // NOT NEEDED
int player1_y = screen_height/2;
int player2_y = screen_height/2;
int ball_x = screen_width/2;
int ball_y = screen_height/2;

/* Velocity variables */
int player1_dy = 0;
int player2_dy = 0;
int ball_dx = 0;
int ball_dy = 0;

/* Size variables */
int paddle_height = initial_paddle_height;  // Indicates the width of each player paddle.
int ball_size = initial_ball_size;          // Indicates the length of a side of the square ball.

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
 * Initializes the builtin timer on the RISV-V board and uses it to keep track of the surpassed time during the game.
 */
void initialize_game_time() {
    // TODO
    // Configure the game_time variable so that it uses the RISC-V board timer for accurately counting the time and displaying it on the 7-segment displays.
}

/**
 * Resets the global variables and starts the game from the starting point.
 */
void initialize_game() {
    player1_dy = player2_dy = 0;
    ball_dx = initial_ball_velocity;
    ball_dy = 0;
    
    player1_y = player2_y = screen_height/2;
    ball_x = screen_width/2;
    ball_y = screen_height/2;

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
 * Rotate the ball vector by the given amount of degrees counter-clockwise.
 * There are preset rotation degrees. 15, 30, 45, 60, 75, 90. Automatically rounds to the closest degree.
 */
void rotate_ball_vector_counter_clockwise(int degrees) {
    if (degrees != 15 && degrees != 30 && degrees != 45 && degrees != 60 && degrees != 75 && degrees != 90) {   // If-statement to set the amount of degrees to rotate to one of the preset amount of degrees.
        if (degrees <= 22) degrees = 15;
        if (degrees > 22 && degrees <= 37) degrees = 30;
        if (degrees > 37 && degrees <= 52) degrees = 45;
        if (degrees > 52 && degrees <= 67) degrees = 60;
        if (degrees > 67 && degrees <= 82) degrees = 75;
        if (degrees > 82) degrees = 90;
    }
    
    if (degrees == 15) {
        ball_dx = ball_dx * 0.965 - ball_dy * 0.262;    // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 0.262 + ball_dy * 0.965;    // Calculate the y-vector after rotation.
    } else if (degrees == 30) {
        ball_dx = ball_dx * 0.863 - ball_dy * 0.506;    // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 0.506 + ball_dy * 0.863;    // Calculate the y-vector after rotation.
    } else if (degrees == 45) {
        ball_dx = ball_dx * 0.707 - ball_dy * 0.707;    // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 0.707 + ball_dy * 0.707;    // Calculate the y-vector after rotation.
    } else if (degrees == 60) {
        ball_dx = ball_dx * 0.498 - ball_dy * 0.867;    // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 0.867 + ball_dy * 0.498;    // Calculate the y-vector after rotation.
    } else if (degrees == 75) {
        ball_dx = ball_dx * 0.258 - ball_dy * 0.966;    // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 0.966 + ball_dy * 0.258;    // Calculate the y-vector after rotation.
    } else if (degrees == 90) {
        ball_dx = ball_dx * 0 - ball_dy * 1;            // Calculate the x-vector after rotation.
        ball_dy = ball_dx * 1 + ball_dy * 0;            // Calculate the y-vector after rotation.
    }

    /* The following method instead uses a method of multyplying the balls velocity vector with the rotational matrix for a 2D vector. It unfortunately requires #include <math.h> to work, which apparently isn't supported on the RISC-V board.
    degrees *= PI / 180;                                        // Convert the amount of degrees to radians.
    ball_dx = ball_dx * cos(degrees) - ball_dy * sin(degrees);  // Calculate the x-vector after rotation.
    ball_dy = ball_dx * sin(degrees) + ball_dy * cos(degrees);  // Calculate the y-vector after rotation.
    */
}

/**
 * Rotate the ball vector by the given amount of degrees clockwise.
 * There are preset rotation degrees. 15, 30, 45, 60, 75, 90. Automatically rounds to the closest degree.
 * Makes use of the rotate_ball_vector_counter_clockwise function.
 */
void rotate_ball_vector_clockwise(int degrees) {
    rotate_ball_vector_counter_clockwise(degrees);  // Rotate the ball clockwise.
    ball_dy = -ball_dy;                             // Reverse the y-vector to make the rotation clockwise instead.
}

/**
 * Handles collisions for the ball by changing its direction.
 * Parameter: Collision_type indicates what type of collision happened. 1 for upper or lower walls. 2 for bounce on player 1. 3 for bounce on player 2. For any other integer, do nothing.
 */
void handle_collision(int collision_type) {
    if (ball_dy == 0) {                                 // Case to handle when the ball is moving in a straight line along the x-axis.
        ball_dx = -ball_dx;                             // Immediately rotate the ball vector 180 degrees.
    } else if (ball_dx == 0) {                          // Case to handle when the ball is moving in a straight line along the y-axis.
            rotate_ball_vector_clockwise(5);
    } else if (ball_dy >= 0) {                          // Case to handle when the move is moving upwards.
        if(collision_type == 1) {                       // Case to handle when the ball collided with the upper or lower wall.
            rotate_ball_vector_counter_clockwise(90);   // Rotate 90 degrees counter-clockwise.
        } else {
            rotate_ball_vector_clockwise(90);           // Rotate 90 degrees clockwise.
        }
    } else if (ball_dy <= 0) {                          // Case to handle when the move is moving downwards.
        if(collision_type == 1) {                       // Case to handle when the ball collided with the upper or lower wall.
            rotate_ball_vector_clockwise(90);           // Rotate 90 degrees clockwise.
        } else {
            rotate_ball_vector_counter_clockwise(90);   // Rotate 90 degrees counter-clockwise.
        }
    } 

    /* Code for handling collision with the player 1 paddle. */
    if (collision_type == 2) {
        if (ball_y >= player1_y && ball_y <= player1_y + paddle_height/2) {
            rotate_ball_vector_clockwise(75);
        }
        if (ball_y < player1_y && ball_y >= player1_y - paddle_height/2) {
            rotate_ball_vector_counter_clockwise(75);
        }
    }
    
    /* Code for handling collision with the player 2 paddle. */
    if (collision_type == 3) {
        if (ball_y >= player2_y && ball_y <= player2_y + paddle_height/2) {
            rotate_ball_vector_counter_clockwise(75);
        }
        if (ball_y < player2_y && ball_y >= player2_y - paddle_height/2) {
            rotate_ball_vector_clockwise(75);
        }
    }
    
    /*
    else {                                            // Else statement to handle the case when something unforeseen happens.
        ball_x = screen_width/2;                        // Reset the ball x-coordinate.
        ball_y = screen_height/2;                       // Reset the ball y-coordinate.
        ball_dx = initial_ball_velocity;                // Reset the ball velocity along the x-axis.
        ball_dy = 0;                                    // Reset the ball velocity along the y-axis.
    }
    */
    
    // Should we make it so that the ball rotates a different amount of degrees depending on where it hits the paddles OR the speed of the paddles?
}

/**
 * Move the ball one step along its velocity vector and handle potential collisions.
 */
void move_ball() {
    if(fast_ball) {
        ball_dx += 0.1; // Increase the ball velocity along the x-axis.
        ball_dy += 0.1; // Increase the ball velocity along the y-axis.
    }
    ball_x += ball_dx;  // Move the ball along the x-axis by its corresonding motion vector.
    ball_y += ball_dy;  // Move the ball along the y-axis by its corresonding motion vector.

    if(ball_y == 0 || ball_y == screen_height) handle_collision(1);                                                                                          // Case when ball collides with the upper or lower wall.
    if(ball_x == player_position && (ball_y <= player1_y + paddle_height/2 && ball_y >= player1_y - paddle_height/2)) handle_collision(2);                   // Case when the ball collides with player 1's paddle.
    if(ball_x == screen_width - player_position && (ball_y <= player2_y + paddle_height/2 && ball_y >= player2_y - paddle_height/2)) handle_collision(3);    // Case when the ball collides with player 2's paddle.
    
    if(ball_x == 0) {                       // Case if player 2 scores.
        increment_score(2);                 // Increment the score of player 2.
        ball_dx = initial_ball_velocity;    // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;            // Reset the ball position.
        ball_y = screen_height/2;
    } else if (ball_x == screen_width) {    // Case if player 1 scores.
        increment_score(1);                 // Increment the score of player 1.
        ball_dx = initial_ball_velocity;    // Reset the ball velocity.
        ball_dy = 0;
        ball_x = screen_width/2;            // Reset the ball position.
    }
}

/**
 * Moves the paddles one step along their motion vectors if the relevant switches are active.
 * If the paddles leave the screen border, they can only move in the direction that is back towards the screen.
 */
void move_paddles() {
    /* For player 1. */
    if (player1_y - paddle_height/2 > 0 && player1_y + paddle_height/2 < screen_height) {   // Checks that player 1 is within the screen borders.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            player1_y -= player1_dy;                                                        // Moves the paddle in the unintended direction by its motion vector.
        } else {
            player1_y += player1_dy;                                                        // Moves the paddle in the intended direction by its motion vector.
        }
    } else if (player1_y - paddle_height/2 < 0) {                                           // Checks if the player 1 paddle is below the screen.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player1_dy < 0) player1_y -= player1_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's upwards.
        } else {
            if (player1_dy > 0) player1_y += player1_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's upwards.
        }
    } else if (player1_y + paddle_height/2 > screen_height) {                               // Checks if the player 1 paddle is above the screen.
        if (reverse_paddles1) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player1_dy > 0) player1_y -= player1_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's downwards.
        } else {
            if (player1_dy < 0) player1_y += player1_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's downwards.
        }
    }

    /* For player 2. */
    if (player2_y - paddle_height/2 > 0 && player2_y + paddle_height/2 < screen_height) {   // Checks that player 2 is within the screen borders.
        if (reverse_paddles2) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            player2_y -= player2_dy;                                                        // Moves the paddle in the unintended direction by its motion vector.
        } else {
            player2_y += player2_dy;                                                        // Moves the paddle in the intended direction by its motion vector.
        }
    } else if (player2_y - paddle_height/2 < 0) {                                           // Checks if the player 2 paddle is below the screen.
        if (reverse_paddles2) {                                                             // Checks if the special game mode REVERSE PADDLES is activated.
            if (player2_dy < 0) player2_y -= player2_dy;                                    // Moves the paddle in the unintended direction by its motion vector IF it's upwards.
        } else {
            if (player2_dy > 0) player2_y += player2_dy;                                    // Moves the paddle in the intended direction by its motion vector IF it's upwards.
        }
    } else if (player2_y + paddle_height/2 > screen_height) {                               // Checks if the player 2 paddle is above the screen.
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
    } else if (player1_y + paddle_height/2 > screen_height) {
        player1_y = screen_height - paddle_height/2;
    }

    if (player2_y - paddle_height/2 < 0) {   // Checks that player 2 paddle is within the screen border and resets it otherwise.
        player2_y = 0 + paddle_height/2;
    } else if (player2_y + paddle_height/2 > screen_height) {
        player2_y = screen_height - paddle_height/2;
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


/*

Notes:
- If the first switch is active, move the player2 paddle up. 
- If the second switch is active, move the player2 paddle down.
- If none or both of the first and second switches are active, do nothing.
- Vice versa for player1 with the tenth and ninth switch.
- Restart the game by pressing the push-button.

- Should we make it so that the ball turns more sharply when different parts of the paddle are hit?
- Should we instead make it so that the ball turns more sharply when the paddle hits it with speed?
- Should we make it so that the paddles acutally accelerate to amplify this effect, instead of just letting them reach their speed instantaneously?
*/
