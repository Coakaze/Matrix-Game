# Tetris

## Backstory
I chose to create this game because it is a fun old school game, that almost everyone has played and knows its rules. It was a great challenge developing my first embedded project, even if I had a hard time figuring out how to actually make it.

## The menu
When we first start the project, a greeting message is displayed for 4 seconds. Atfer that the menu appears and with the joystick you can navigate through it. The menu follows the lower schema:
  - Start Game: starting the game
  - Highscore: showing the first 3 best scores ever made.
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
- The score and level are displayed on the LCD, till the game ends

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
