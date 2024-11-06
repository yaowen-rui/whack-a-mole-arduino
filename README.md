# whack-a-mole-arduino
The game consists of: 
1) Eight LED Buttons : Representing 8 moles, each button lights up to signal a "mole" appearance.
2) Random Mole Selection: A "mole" appears as a randomly chosen button with its LED light on.
3) Scoring System: Screen tracks correct hits as players successfully "whack" each mole on LED display.
4) Speaker for sound effects: Adds feedback for game start/over.
5) Power switch


Game logic:
After turning on the power switch, the game begins with the buttons twinkling in sequence to indicate the start. After this, one of the eight buttons will light up at random. The player must press the lit button before the next one lights up. As the game progresses, the moles appear more frequently, increasing the challenge up to a set limit. The speed level change is triggered by the amount of moles that have been whacked by the player (Switch at 10, 25, 35 whacked moles, making up for a total of 4 levels: Beginner, Amateur, Expert, Legendary). Players are allowed up to three missed moles; if they exceed this, a sound effect is triggered, signaling the end of the game. The new game automatically begins if the player leaves the power switch on.
The game ideally can be played both by pressing buttons by hand or with a toy hammer.

Hardware requirement: 
Arduino UNO
8x 220 Ohm Resistor (blue) for Buttons
Cables
Motherboard
Screen
8x LED-push button switch, 60mm, 12V 
4 Ohm 3 Watt Speaker for Arduino
Tip120 Transistor 3 pieces


