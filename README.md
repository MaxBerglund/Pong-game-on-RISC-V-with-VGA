# Pong-game-on-RISC-V-with-VGA
This repository belongs to the Mini Project of Max Berglund and Milla Gradin for the KTH course IS1500.

# Description
This repository contains a two-player Pong game written in C-code which can be run on a RISC-V board with a VGA output. The outer switches on the RISC-V board are used to control the player paddles, while other switches can be used to alter the game in different ways such as making the ball move at different speeds or making the paddles behave differently. A player increases their score by making the ball touch the border behind their opponent, and the game ends when a player reaches a certain score. The push button can be used to restart the game.

# Implementation
[TO BE ADDED]


# How to run
[TO BE ADDED]

Our version of the Pong game requires a Dtek RISC-V board and an external screen to operate. To load the code to the board the files, where the code is found, must be compiled by using the 'make' statement in the virtual UTM. Thereafter you must run the binary main file 'main.bin' by writing 'dtekv-run main.bin', the game will show up on the screen.

