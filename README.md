# Tetris

## Backstory
I chose to create this game because it is a fun old school game, that almost everyone has played and knows its rules. It was a great challenge developing my first embedded project, even if I had a hard time figuring out how to actually make it.

## The menu
When we first start the project, a greeting message is displayed for 4 seconds. Atfer that the menu appears and with the joystick you can navigate through it (making a little sound). The menu follows the lower schema:
  - Start Game: starting the game
  - Highscore: showing the first 3 best scores of all time.
  - Settings: here you can change you nickname and set the game difficulty
  - About: informations about the game and the author with a github link 

## Game logic
- The joystick controlls every move you can make
    - UP: rotate the current block to fit better
    - DOWN: move the block faster to your desired position
    - LEFT: move the block one step to the left
    - RIGHT: move the block one step to the right
- You have to complete lines by moving differently shaped pieces, which descend onto the playing field. 
- The completed lines disappear and grant the player points (you gain substantially more points if you can make more lines disappear at once).
- You lose when the block sits on the first row of the field.
- Falling blocks are randomly selected from a predetermined list of shapes.
- The score and level are displayed on the LCD, till the game ends.
- When the game ends, a sad face appears on the led matrix and you can see the final score displayed on LCD.

## Challenges
The most annoying thing when developing on arduino is memory management. This made me think how to store the matrix and the blocks without using much memory. For example, in the first phase I was trying to store every component and its rotations in a matrix, making the project to use too much global memory. Only after that I realised that I have to change something and now the rotations are made dinamically, no more need to store them.

## Used components
```
1. Arduino Uno - 1
2. LCD 1602 - 1
3. Buzzer - 1
4. 7219 matrix driver - 1
5. 8x8 matrix led - 1
6. Joystick - 1
7. 220Ω resistor - 2
8. 10μF electrolytic capacitor - 1
9. 104pF ceramic capacitor - 1
```

Picture of the setup:<br>
<img src="https://github.com/Coakaze/Matrix-Game/blob/main/firstPhase.jpg" width="300">
