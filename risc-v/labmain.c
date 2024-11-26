#include <stdint.h> 

extern void enable_interrupt(void);

#define screen_width 320
#define screen_heigth 240
#define player_position 8
#define initial_ball_velocity -1
#define initial_ball_size 1
#define initial_paddle_width 8
#define player_velocity 5
#define PI 3.14159

extern void initialize_game();
extern void increment_score(int player_number);
extern void rotate_ball_vector_counter_clockwise(int degrees);
extern void rotate_ball_vector_clockwise(int degrees);
extern void handle_collision();
extern void move_ball();
extern void move_paddles();
extern void initialize_game_time();
extern int get_sw(void);
extern int get_btn(void);
extern int get_digit(int value, int digitNumber);
extern void set_paddles_velocity();
extern void set_special_game_modes();


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
extern int game_time;
extern int reverse_paddles1;
extern int reverse_paddles2;
extern int fast_ball;


volatile char *VGA = (volatile char*) 0x08000000;
// VGA control registers
volatile int *VGA_CTRL = (volatile int*) 0x04000100;


void handle_interrupt(unsigned cause) {
}


/**
 * Sets all pixels on the screen to black.
 */
void reset_screen(){
    for (int i = 0; i < screen_width * screen_heigth; i++) {
            VGA[i] = 0x00; // Black
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


int main() {
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

            reset_screen();

            draw_ball();

            draw_paddle1();

            draw_paddle2();

            // TODO Print everything... 

            if (player1_score >= 5) {
                // TODO "Print game_over, player 1 wins!"...
                game_state = 0;

                draw_diagonal_line_ltr();

                draw_diagonal_line_rtl();
            }

            if (player2_score >= 5) {
                // TODO "Print game_over, player 2 wins!"...
                game_state = 0;
                
                draw_diagonal_line_ltr();

                draw_diagonal_line_rtl();
            }
        }

        // Update VGA control registers (double-buffering simulation)
        // Updates the screen
        *(VGA_CTRL + 1) = (unsigned int)(VGA);
        *(VGA_CTRL + 0) = 0;

        // Delay. Can we find a more efficent delay?
        for (int i = 0; i < 1000000; i++) {
            asm volatile("nop");
        }
    
    }
    
}
