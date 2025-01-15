#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// LCD object (adjust the address for 0x27 for real aduino)
LiquidCrystal_I2C lcd(0x27, 20, 4); // 16x4 LCD

byte customBackslash[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};

#define NOTE_G2  98
#define NOTE_A2  110
#define NOTE_AS2 117

#define NOTE_C3  131

#define NOTE_D3  139

#define NOTE_E3  165
#define NOTE_F3  175

#define NOTE_G3  196

#define NOTE_DS5 622

#define NOTE_F5  698

#define NOTE_G5  784

#define NOTE_A5  880

#define NOTE_C6  1047

//game start melody
int start_melody[] = {NOTE_G2, NOTE_A2, NOTE_AS2, NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3};
int start_noteDurations[] = {
  14, 14, 14, 14, 14, 14, 10
};

//hit mole/success melody
int success_melody[] = {NOTE_DS5, NOTE_F5, NOTE_A5};
int success_noteDurations[] = {
   16, 16, 16
};

//life lost (escaped mole/miss) melody
int lifelost_melody[] = {NOTE_F3, NOTE_AS2, NOTE_G2};
int lifelost_noteDurations[] = {
  16, 16, 10
};

//game over melody
int gameover_melody[] = {NOTE_G3, NOTE_E3, NOTE_D3, };
int gameover_noteDurations[] = {
   10, 10, 2
};


//new highscore melody
int highscore_melody[] = {NOTE_G5, NOTE_A5, NOTE_C6, NOTE_C6};
int highscore_noteDurations[] = {
   12, 12, 12, 8
};

// Declare some integers for the pin values
const int Speaker = 12;

const int LED1 = 0;
const int LED2 = 1;
const int LED3 = 2;
const int LED4 = 3;
const int LED5 = 4;

const int Button1 = 11;
const int Button2 = 10;
const int Button3 = 5;
const int Button4 = 8;
const int Button5 = 7;

// Button debounce parameters
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

// State tracking for buttons
int lastButtonState = LOW;
int buttonState = LOW;

// Total miss count
int totalMissCount = 0;

// Successful whack count
int whackedMolesCount = 0;

// Highest score
int highestScore = 0;

// Delay time for LED to stay on (default is 1000ms)
int ledOnTime = 1000;

// Function for debouncing button presses
bool isButtonPressed(int buttonPin) {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        lastButtonState = reading;
        return true;
      }
    }
  }

  lastButtonState = reading;
  return false;
}

// Function to load the highest score from EEPROM
void loadHighestScore() {
  highestScore = EEPROM.read(0); // Read the high score from EEPROM address 0
  if (highestScore == 255) {
    highestScore = 0; // Handle uninitialized EEPROM case
  }
}

// Function to save the highest score to EEPROM
void saveHighestScore() {
  if (whackedMolesCount > highestScore) {
    playGivenMelody(highscore_melody, highscore_noteDurations, 4);
    highestScore = whackedMolesCount;
    EEPROM.write(0, highestScore); // Save the new high score to EEPROM address 0
  }
}

//plays any given melody
void playGivenMelody(int melody[], int noteDurations[], int length) {
  for (int thisNote = 0; thisNote < length; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(Speaker, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(Speaker);
  }
}

// Sequentially light up LEDs at the beginning of the game
void startSequence() {
  const int LEDS[] = {LED1, LED2, LED3, LED4, LED5};
  for (int i = 0; i < 5; i++) {
    digitalWrite(LEDS[i], HIGH); // Turn on LED
    delay(300);                  // Wait for 300ms
    digitalWrite(LEDS[i], LOW);  // Turn off LED
    delay(100);                  // Wait before the next LED
  }

}

// Check if the game should end
bool checkGameEnd() {
  if (totalMissCount > 3) { // Maximum 3 misses allowed
    return true;
  }
  return false;
}

// Function to twinkle all LEDs for 2 seconds
void twinkleLEDs() {
  const int LEDS[] = {LED1, LED2, LED3, LED4, LED5};
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) { // Run for 2 seconds
    for (int i = 0; i < 5; i++) {
      digitalWrite(LEDS[i], HIGH);
    }
    delay(100);
    for (int i = 0; i < 5; i++) {
      digitalWrite(LEDS[i], LOW);
    }
    delay(100);
  }
}

// Reset the game state
void resetGame() {
  saveHighestScore(); // Save the high score before resetting
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highest Score:");
  lcd.setCursor(0, 1);
  lcd.print(highestScore);
  delay(2000); // Show highest score for 2 seconds
  totalMissCount = 0;     // Reset the total miss count
  whackedMolesCount = 0;  // Reset the successful whack count
  ledOnTime = 1000;       // Reset the LED speed
  lcd.clear();            // Clear LCD
  lcd.setCursor(0, 0);    // Display reset message
  lcd.print("Game Restarted!");
  delay(2000);            // Show for 2 seconds
  playGivenMelody(start_melody, start_noteDurations, 7); //plays start melody
  startSequence();        // Restart the initial LED sequence
  updateLCD();
}

// Update LED speed based on the whacked moles count
void updateSpeedLevel() {
  if (whackedMolesCount >= 7) {
    ledOnTime = 800; // Fastest speed
  } else if (whackedMolesCount >= 5) {
    ledOnTime = 1000; // Medium-fast speed
  } else if (whackedMolesCount >= 3) {
    ledOnTime = 1200; // Medium speed
  } else {
    ledOnTime = 1400; // Default speed
  }
}

// Update the LCD display
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0); // Set to the first line
  lcd.print("Moles Whacked:");
  lcd.setCursor(0, 1); // Set to the second line
  lcd.print(whackedMolesCount);
  lcd.setCursor(20, 0);
  lcd.print("Remaining Lives:");
  lcd.setCursor(20, 1);
  int remainingLives = 3 - totalMissCount;
  lcd.print(remainingLives);
  
}

// Display the highest score at the beginning
void showHighestScore() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highest Score:");
  lcd.setCursor(0, 1);
  lcd.print(highestScore);
  delay(2000); // Show for 2 seconds
}

// Set up the LEDs, buttons, and LCD
void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  pinMode(Button4, INPUT_PULLUP);
  pinMode(Button5, INPUT_PULLUP);

  // Initialize the LCD
  lcd.init();           // Initialize the LCD
  lcd.backlight();      // Turn on the backlight,otherwise can't be seen
  lcd.setCursor(0, 0);  // Display a welcome message
  lcd.print("Whack-a-Mole!");

  //first frame, mole hasn't showed up yet!
  lcd.setCursor(20,1);
  lcd.print("_ _ _ _");
  delay(500);

  //second frame, mole peeking out
  lcd.setCursor(20,0);
  lcd.print("  ___");
  lcd.setCursor(20,1);
  lcd.print("_/_Y_");
  lcd.write(byte(7));
  lcd.print("_");
  delay(500);

  //the following code draws a mole...
  lcd.createChar(7, customBackslash);
  lcd.setCursor(0,1);
  lcd.print("  ___");
  lcd.setCursor(20,0);
  lcd.print(" /'Y'");
  lcd.write(byte(7));
  lcd.setCursor(20,1);
  lcd.print("_");
  lcd.write(byte(7));
  lcd.print("\_^_/_");

  delay(2000);
  loadHighestScore();   // Load the highest score from EEPROM
  showHighestScore();   // Display the highest score
  lcd.clear();
  playGivenMelody(start_melody, start_noteDurations, 7); //plays start melody
  startSequence();
}

void loop() {
  while (true) {
    int randomLED = random(0, 5);
    int randomLED_PIN;
    int buttonPIN;

    switch (randomLED) {
      case 0: randomLED_PIN = LED1; buttonPIN = Button1; break;
      case 1: randomLED_PIN = LED2; buttonPIN = Button2; break;
      case 2: randomLED_PIN = LED3; buttonPIN = Button3; break;
      case 3: randomLED_PIN = LED4; buttonPIN = Button4; break;
      case 4: randomLED_PIN = LED5; buttonPIN = Button5; break;
    }

    // Light up the selected LED
    digitalWrite(randomLED_PIN, HIGH);

    // Wait for button press
    bool buttonPressed = false;
    unsigned long startTime = millis();

    while (millis() - startTime < ledOnTime) { // Wait based on current speed
      if (isButtonPressed(buttonPIN)) {
        playGivenMelody(success_melody, success_noteDurations, 3);
        buttonPressed = true;
        digitalWrite(randomLED_PIN, LOW); // Turn off the LED
        whackedMolesCount++;             // Increment successful whacks
        updateSpeedLevel();              // Update speed based on hits
        updateLCD();  
        //lcd.setCursor(20, 1); //for testing 
        //lcd.print(buttonPIN);
        break;
      }
    }

    // If button was not pressed, increment the total miss count
    if (!buttonPressed) {
      playGivenMelody(lifelost_melody, lifelost_noteDurations, 3); //plays life lost melody
      totalMissCount++;
      // Check if the game should end
      if (checkGameEnd()) {
        twinkleLEDs(); // Twinkle all LEDs
        lcd.clear();
        lcd.setCursor(0, 0);
        playGivenMelody(gameover_melody, gameover_noteDurations, 3);
        lcd.print("Game Over!");
        lcd.setCursor(0, 1);
        lcd.print("Final Score:");
        lcd.print(whackedMolesCount);
        delay(2000); // Wait for 2 seconds before resetting
        resetGame();
      }
      updateLCD();                     // Update LCD with the latest life lost
      digitalWrite(randomLED_PIN, LOW); // Turn off the LED
    }

    delay(300); // Short delay before lighting the next LED
  }
}
