# 2048 Game in C

This is a C program that implements the popular 2048 game. The game is played on a 4x4 grid of tiles, each with a power of two. The player can use the keyboard to move the tiles left, right, up, or down. When two tiles with the same value collide, they merge into one tile with double the value. The game ends when the board is full and no more moves are possible. The goal is to reach the 2048 tile or the highest possible score.

## Features

- The program uses ANSI escape codes to display colors and move the cursor on the terminal.
- The program uses the termios library to read keyboard input without waiting for the enter key.
- The program uses a simple menu system to start a new game, display the score, or exit.
- The program uses a random number generator to spawn new tiles with either 2 or 4 as values.
- The program uses a simple algorithm to rotate and flip the board to handle different directions of movement.
- The program checks for game over conditions and displays the final score.

## How to run

To run the program, you need a C compiler and a terminal that supports ANSI escape codes. You can compile the program with the following command:

```bash
gcc -o 2048 2048.c
```

Then you can run the program with the following command:

```bash
./2048
```

To play the game, use the following keys:

- k: move down
- i: move up
- j: move left
- l: move right
- q: quit the game

Have fun!
