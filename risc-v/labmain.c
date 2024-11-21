#include <stdint.h> 

extern void enable_interrupt(void);

void handle_interrupt(unsigned cause) {
}



int main() {
    volatile char *VGA = (volatile char*) 0x08000000;

    // Screen dimensions
    const int width = 320;
    const int height = 240;

    // Clear the screen to black
    for (int i = 0; i < width * height; i++) {
        VGA[i] = 0x00; // Black
    }

    // Square ball properties
    int ball_size = 3;  // Size of the square (length of one side)
    int ball_x = 160;    // Initial x-position (center)
    int ball_y = 120;    // Initial y-position (center)
    int dx = 1;          // Horizontal speed
    int dy = 1;          // Vertical speed

    // VGA control registers
    volatile int *VGA_CTRL = (volatile int*) 0x04000100;

    while (1) {
        // Clear the screen to black
        for (int i = 0; i < width * height; i++) {
            VGA[i] = 0x00; // Black
        }

        // Draw the square ball
        for (int y = 0; y < ball_size; y++) {
            for (int x = 0; x < ball_size; x++) {
                int px = ball_x + x;
                int py = ball_y + y;
                if (px >= 0 && px < width && py >= 0 && py < height) {
                    VGA[py * width + px] = 0xFF; // White pixel
                }
            }
        }

        // Update square ball position
        ball_x += dx;
        ball_y += dy;

        // Bounce off edges
        if (ball_x < 0 || ball_x + ball_size >= width) {
            dx = -dx;
        }
        if (ball_y < 0 || ball_y + ball_size >= height) {
            dy = -dy;
        }

        // Update VGA control registers (double-buffering simulation)
        *(VGA_CTRL + 1) = (unsigned int)(VGA);
        *(VGA_CTRL + 0) = 0;

        // Delay
        for (int i = 0; i < 1000000; i++) {
            asm volatile("nop");
        }
    }

    return 0;
}
