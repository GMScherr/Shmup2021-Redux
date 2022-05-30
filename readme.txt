SG-Redux -
Written by Gabriel Mendez Scherr 2022
License : None, all software should be free

Library Prerequisites :
-OpenGL Utility Toolkit
-OpenGL Extension Wrangler
-C Standard Library Maths

Instructions on how to run :
-Download the entire package into its own directory.
-Open the terminal on directory.
-Use the makefile by typing the word "make".
-Run the the program "TP1".

Notes :
-I don't know who or what to blame, but sometimes things just stop working. Using make clean and then make, not modifying anything, makes it work again. If that is the case, try make clean and pray. One example of this is textures not loading correctly between one test and the other where nothing about the texture was touched on.
-All texture images are contained in the project directory. Do not alter their names or remove them under risk of texture loading failure.
-OpenGL struggles with drawing large textures and they lag the game to an unplayable state. But it shouldn't, I suspect this is due to poor performance on the virtual machine's side. Therefore, if you wish to activate backgrounds you can do so by changing the BACKGROUND_STATUS global flag to "true" in the .h file and recompiling the code.
Game controls :
-Directional arrows to move the character
-'z' to focus move : your character will move at half speed.
-'x' to shoot
-'ESC' to pause

Has been implemented :
-Basic game functionalities (Player interaction, collisions and OpenGL usage)
-Animated Textures
-Scrolling Background
-Scoring System
-Lives