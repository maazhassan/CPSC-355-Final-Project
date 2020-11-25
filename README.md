# CPSC-355-Final-Project
Final project for CPSC 355, with two versions written in C and ARMv8 Assembly. This is the C version.

To input a move, simply enter the coordinates in the form: x y

The scores.log file should be placed in the same directory as mygame.c. If this file is
not present, it will be created. 5 sample scores have been provided in the scores.log file
so that the "display top n scores" feature may be easily tested without having to spend
too much time playing through the game.

A score is only logged on a win or a loss, not if the game is quit prematurely. The format
of the scores.log file should not be changed, or else it will result in errors when trying
to parse the information from it.

The time pack bonus has also been implemented, and will add 10-40 seconds to the timer,
depending on the size of the board.
