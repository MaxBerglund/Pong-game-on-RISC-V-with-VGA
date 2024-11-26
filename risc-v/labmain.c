#include <stdint.h> 
#include <stdlib.h>

extern void enable_interrupt(void);

#define screen_width 320
#define screen_heigth 240
#define player_position 8
#define player_width 5
#define initial_ball_velocity -3
#define initial_ball_size 7
#define initial_paddle_heigth 25
#define initial_paddle_width 5
#define player_velocity 5
#define PI 3.14159

extern void initialize_game();
extern void rotate_ball_vector_counter_clockwise(int degrees);
extern void move_ball();
extern void move_paddles();
extern int get_btn(void);
extern void set_paddles_velocity();
extern void set_special_game_modes();
extern void seven_segment_display(int display, int number);

/* Coordinate variables */
extern int player1_y;
extern int player2_y;
extern int ball_x;
extern int ball_y;

/* Velocity variables */
extern int player1_dy;
extern int player2_dy;
extern int ball_dx;
extern int ball_dy;

/* Size variables */
extern int paddle_height;
extern int ball_size;

/* Score variables */
extern int player1_score;          
extern int player2_score;

/* Game state variables */
extern int game_state;
extern int reverse_paddles1;
extern int reverse_paddles2;
extern int fast_ball;
extern int seconds;
extern int minutes;

int timeoutCount = 0;
int two_seconds = 0;

/* VGA variables */
volatile char *VGA = (volatile char*) 0x08000000;       // Write pixels to the screen
volatile int *VGA_CTRL = (volatile int*) 0x04000100;    // VGA control registers, used to update the screen


void handle_interrupt(unsigned cause) {
}


/**
 * Sets all pixels on the screen to black.
 */
void reset_screen(){
    for (int i = 0; i < screen_width * screen_heigth; i++) {
            VGA[i] = 0x00; //Black
        }

}

/**
 * Draws the ball on the screen.
 */
void draw_ball (){

    for (int y = 0; y < ball_size; y++) {
        for (int x = 0; x < ball_size; x++) {
            int px = ball_x + x;
            int py = ball_y + y;
            if (px >= 0 && px < screen_width && py >= 0 && py < screen_heigth) {
                VGA[py * screen_width + px] = 0xFF; // White pixel
            }

        } ball_clear();
    }

}

    void ball_clear(){
        for (int y = 0; y < ball_size; y++) {
            for (int x = 0; x < ball_size; x++) {
                int px = ball_x + x;
                int py = ball_y + y;
                if (px >= 0 && px < screen_width && py >= 0 && py < screen_heigth) {
                    VGA[py * screen_width + px] = 0xFF; // Black pixel
                }
            }
    }


}

/**
 * Draws the paddle of player 1 on the screen.
 */
void draw_paddle1(){

    for (int y = 0; y < paddle_height; y++) {
        for (int x = 0; x < initial_paddle_width; x++) {
            int px = player_position + x;
            int py = player1_y - paddle_height / 2 + y;
            if (px >= 0 && px < screen_width && py >= 0 && py < screen_heigth) {
                VGA[py * screen_width + px] = 0xFF; // White pixel
            }
        }
    }
}

/**
 * Draws the paddle of player 2 on the screen.
 */
void draw_paddle2(){

    for (int y = 0; y < paddle_height; y++) {
        for (int x = screen_width - player_position - initial_paddle_width; x < screen_width-player_position; x++) {
            int px = player_position + x;
            int py = player2_y - paddle_height / 2 + y;
            if (px >= 0 && px < screen_width && py >= 0 && py < screen_heigth) {
                VGA[py * screen_width + px] = 0xFF; // White pixel
            }
        }
    }
}

void draw_diagonal_line_ltr() {
    for (int y = 100; y < screen_heigth -99; y++) {
        for (int x = 50; x < screen_width; x++) {
            int px = x;
            int py = y;
            if (x==y) {
                VGA[py * screen_width + px + 35] = 0xFF; // White pixel
            }
        }
    }
}

void draw_diagonal_line_rtl() {
    for (int y = screen_heigth-100; y > 99; y--) {
        for (int x = screen_width - 100; x > 0; x--) {
            int px = x;
            int py = y;
            if (y==screen_heigth-x) {   
                VGA[py * screen_width + px + 35] = 0xFF; // White pixel
            }
        }
    }
}

void green_screen() {
    for (int i = 0; i < screen_width * screen_heigth; i++) {
            VGA[i] = 0x811331;
        }
}

/**
 * Updates the seconds and minutes values and sends them to the 7-segment displays.
 */
void update_timer() {
    //rotate_ball_vector_counter_clockwise(45);
    
    two_seconds++;
    if(fast_ball && two_seconds >= 2) { // If the special game_mode FAST-BALL is active, increase the ball speed each 2 seconds.
        two_seconds = 0;                // Reset the five-second status.
        if(ball_dx > 0) {
            if(ball_dx != 0) {
                ball_dx += 1;           // Increase the ball velocity along the x-axis.
            }
        } else {
            if(ball_dx != 0) {
                ball_dx -= 1;           // Increase the ball velocity along the x-axis.
            }
        }
        if(ball_dy > 0) {
            if(ball_dy != 0) {
                ball_dy += 1;           // Increase the ball velocity along the y-axis.
            }
        } else {
            if(ball_dy != 0) {
                ball_dy -= 1;           // Increase the ball velocity along the x-axis.
            }
        }
    }

    timeoutCount = 0;
    if(seconds >= 59) {             
        minutes++;
        seconds = 0;
    } else {
        seconds++;
    }
    seven_segment_display(2, seconds % 10);     // Send the ones digit of the seconds to the third 7-segment display.
    seven_segment_display(3, seconds / 10);     // Send the tens digit of the seconds to the fourth 7-segment display.
    seven_segment_display(4, minutes % 10);     // Send the ones digit of the minutes to the fifth 7-segment display.
    seven_segment_display(5, minutes / 10);     // Send the tens digit of the minutes to the sixth 7-segment display.
}


int main() {
    initialize_game();                          // Set up the global variables for the game.
    volatile int *timeoutPointer = (volatile int*) 0x04000020; // Creates a pointer that points to the memory adress where the timer is. It is volatile so that the compiler doesn't do any unneccessary optimisations that might alter the behaviour of the dtek-board.

    while (1) {                                 // Main game loop.
        
        if (get_btn()) {
            initialize_game();                  // If the push-button is pressed, reset the game. *Should we use this inefficent polling method? Maybe replace with interruption method, but only after we have made it work to avoid painstaking debugging.*
        } 

        if(game_state) {
            
            if((*timeoutPointer & 1) == 1) {    // Check if the timeout event flag is true. In that case, run the program.
                timeoutCount++;
                *timeoutPointer &= ~1;          // Reset the timeout flag.

                if(timeoutCount == 10) update_timer(); // Increments and updates the game timer.

                set_special_game_modes();       // Sets special game modes according to the states of the switches.
                
                set_paddles_velocity();         // Sets the velocity of the paddles according to the states of the switches.
                
                move_ball();                    // Moves the ball and handles collisions.

                reset_screen();                 // Reset the screen by making every pixel on it black.

                move_paddles();                 // Moves the paddles according to the input of the switches.   

                draw_ball();                    // Set the pixels where the ball is to white.

                draw_paddle1();                 // Set the pixels where the player 1 paddle is to white.

                draw_paddle2();                 // Set the pixels where the player 2 paddle is to white.

                if (player1_score >= 5) {

                    game_state = 0;

                    green_screen();

                    draw_diagonal_line_ltr();

                    draw_diagonal_line_rtl();
                }
                
                if (player2_score >= 5) {

                    game_state = 0;

                    green_screen();

                    draw_diagonal_line_ltr();

                    draw_diagonal_line_rtl();
                }
            }
        }

        // Update VGA control registers (double-buffering simulation)
        // Updates the screen
        *(VGA_CTRL + 1) = (unsigned int)(VGA);
        *(VGA_CTRL + 0) = 0;
    }
    
}
