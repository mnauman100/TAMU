#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int joystickYPin = A1;  // Joystick Y-axis (up/down)
const int joystickXPin = A0;  // Joystick X-axis (left/right)
const int buttonPin = 8;      // Optional push button on joystick
const int RLED = 3;
const int GLED = 2;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C LCD setup

int selectedGame = 0;  // Tracks the currently selected game (0 = Dino Jump, 1 = Pong, 2 = Snake)

// Custom symbols for games
byte dinoSymbol[8] = {
  B01110,
  B10111,
  B11100,
  B11111,
  B11000,
  B11100,
  B11000,
  B11100
};

byte cactus[8] = {
  B00110,
  B10110,
  B10110,
  B11111,
  B00110,
  B00111,
  B01110,
  B00110
};

byte triangle[8] = {
  B11000,
  B11100,
  B11110,
  B11111,
  B11111,
  B11110,
  B11100,
  B11000
};
byte rightwolf[8] = {
  B10100,
  B11111,
  B10111,
  B11100,
  B11110,
  B11000,
  B11000,
  B11100
};

byte leftwolf[8] = {
  B00101,
  B11111,
  B11101,
  B00111,
  B01111,
  B00011,
  B00011,
  B00111
};

byte player[8] = {
  B11111,
  B10101,
  B11111,
  B01110,
  B11111,
  B01110,
  B01010,
  B01010
};

byte squid[8] = {
  B00100,
  B01110,
  B10101,
  B11111,
  B01110,
  B01110,
  B10101,
  B10101
};

byte card_symbol[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

byte select[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};


void setup() {
  lcd.init();             // Initialize the LCD
  lcd.backlight();        // Turn on the backlight
  pinMode(buttonPin, INPUT_PULLUP);  // Set button pin as input with pull-up resistor

  // Create custom characters
  lcd.createChar(0, dinoSymbol);
  lcd.createChar(1, cactus);
  lcd.createChar(2, triangle);
  lcd.createChar(3, rightwolf); // facing right (wolf)
  lcd.createChar(4, leftwolf); // Create custom character (wolf)
  lcd.createChar(5, player); // person for red light green light
  lcd.createChar(6, squid); // Squid
  lcd.createChar(7, card_symbol); // Back of cards

  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
}

void showMenu() {
  while (true) {
    int joystickX = analogRead(joystickXPin);

    

    // Display the menu
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Game:");

    // Move the selection cursor using joystick (left/right)
    if ((joystickX < 200) && (selectedGame != 2)) {  // Move right
      selectedGame = (selectedGame + 1);  // Wrap around after the last game
      delay(500);  // Debounce delay
    } 
    else if ((joystickX > 800) && (selectedGame != 0)) {  // Move left
      selectedGame = (selectedGame - 1);  // Wrap around to the last game
      delay(500);
    }

    // Display symbols on the bottom row
    lcd.setCursor(1, 1);
    lcd.print(char(0));  // Dino Jump
    lcd.setCursor(8,1);
    lcd.print(char(6));
    lcd.setCursor(14, 1);
    lcd.print(char(7));
    delay(400);

    // Highlight the selected game with a cursor
    if (selectedGame == 0) {
      lcd.setCursor(0, 1);
      lcd.print(char(2));  // Cursor below Dino Jump
      delay(400);

    } else if (selectedGame == 1) {
      lcd.setCursor(7, 1);
      lcd.print(char(2));  // Cursor below Dino Jump
      delay(400);
    } else if (selectedGame == 2) {
      lcd.setCursor(13, 1);
      lcd.print(char(2));  // Cursor below Dino Jump
      delay(400);
    
    }

    
    // Confirm selection with button press
    if (digitalRead(buttonPin) == LOW) {  // Button pressed
      delay(50);  // Debounce delay
      if (selectedGame == 0) {
        playDinoJump();  // Call the Dino Jump game
      }
      else if (selectedGame == 1){
        redLightGreen();
      }
      else if (selectedGame == 2){
        memoryGame();
        delay(500);
      }
    }
  }
}


void playDinoJump() {
  lcd.clear();
  lcd.print("Starting Dino...");
  delay(2000);

  int characterPosX = 0;    // Character X position
  int characterPosY = 1;    // Character Y position (0 or 1, simulating jump)
  bool isJumping = false;   // To check if character is jumping
  int obstaclePos = 15;     // Initial position of obstacle (LCD is 16 columns, 0-15)
  int count = 0;

  // Timing variables for the jump
  unsigned long jumpStartTime = 0;
  const unsigned long jumpDuration = 1500;  // Duration of the jump in milliseconds
  bool gameOver = false;

  int joystickY = analogRead(joystickYPin);
  int joystickX = analogRead(joystickXPin);
  int jumpThresholdLow = 900; 
  int rightthreshold = 900;
  int leftthreshold = 200;  // Threshold for detecting joystick upward movement

  while (!gameOver) {
    joystickY = analogRead(joystickYPin);
    joystickX = analogRead(joystickXPin);

    // Start the jump if joystick is pushed up
    if (joystickY >= jumpThresholdLow && !isJumping) {
      isJumping = true;
      jumpStartTime = millis();  // Record the time when the jump started
    }

    if (joystickX >= rightthreshold && characterPosX > 0) {
      --characterPosX;
    } else if (joystickX <= leftthreshold && characterPosX < 15) {
      ++characterPosX;
    }

    // If the character is jumping, move upwards
    if (isJumping) {
      unsigned long currentTime = millis();
      if (currentTime - jumpStartTime < jumpDuration) {
        characterPosY = 0;  // Move character up (simulate jump)
        digitalWrite(GLED, HIGH);
      } else {
        isJumping = false;
        characterPosY = 1;  // Character falls back to normal height (ground)
        digitalWrite(GLED, LOW);
      }
    }

    // Move the obstacle
    --obstaclePos;
    if (obstaclePos < 0) {
      obstaclePos = 15; // Reset obstacle position (LCD width is 16, indices 0-15)
      ++count;
    }

    // Check for collision
    if ((characterPosX == obstaclePos) && (characterPosY == 1)) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("GAME OVER!");
      digitalWrite(RLED, HIGH);
      delay(1000);
      lcd.setCursor(4, 1);
      lcd.print("Score : ");
      lcd.setCursor(12, 1);
      lcd.print(count);
      delay(2000); // Wait before restarting
      gameOver = true;
    }

    // Clear the LCD and update the display
    digitalWrite(RLED, LOW);
    lcd.clear();
    lcd.setCursor(13, 0);
    lcd.print(count);

    lcd.setCursor(characterPosX, characterPosY);  // Set position for the character
    lcd.print(char(0)); // Print character

    lcd.setCursor(obstaclePos, 1); // Set position for the obstacle
    lcd.print(char(1)); // Print obstacle

    delay(600);  // Control game speed (adjust as needed)
  }

  // // Restart the game after game over
  // lcd.clear();
  // lcd.print("Restarting...");
  // delay(2000);
  // playDinoJump();
}
void redLightGreen() {
  lcd.clear();
  lcd.print("SQUID GAME...");
  delay(1000); // Intro delay
  lcd.clear();

  int characterPosX = 0; // Character X position
  int characterPosY = 1; // Character Y position
  bool gameOver = false;

  int jumpThresholdLow = 900;
  int rightThreshold = 900;
  int leftThreshold = 200;

  // Print initial positions
  lcd.setCursor(0, 1);
  lcd.print(char(5)); // Print character
  lcd.setCursor(15, 1);
  lcd.print(char(4)); // Print wolf initially facing the wall

  delay(1000);

  // Random countdown for green light
  int countdown = random(2, 6) * 1000;
  unsigned long previousTime = millis();
  bool isGreenLight = false;
  digitalWrite(RLED, HIGH);
  digitalWrite(GLED, LOW);
  bool win = false;

  while (!gameOver) {
    unsigned long currentTime = millis();

    // Clear only specific sections of the LCD
    lcd.clear();
    lcd.setCursor(characterPosX, characterPosY);
    lcd.print(char(5)); // Update character position
    delay(500);
    lcd.setCursor(15, 1);
    lcd.print(isGreenLight ? char(3) : char(4)); // Toggle wolf face based on light
    delay(500);
    // Check joystick movement
    int joystickX = analogRead(joystickXPin);

    if (isGreenLight) {
      if (joystickX >= rightThreshold && characterPosX > 0) {
        --characterPosX; // Move left
      } 
      else if (joystickX <= leftThreshold && characterPosX < 15) {
        ++characterPosX; // Move right
      }
      if (characterPosX >= 14){
        gameOver = true;
        win = true;
        break;
      }
    }
    // Red light logic (game over check)
    else if (!isGreenLight) {
      if ((joystickX >= rightThreshold) || (joystickX <= leftThreshold)) {
        gameOver = true;
        win = false;
        break;
      }
    }

    // Green light timing logic    
    if (isGreenLight && (currentTime - previousTime >= countdown)) {
      isGreenLight = false;            // Switch to red light
      previousTime = millis();         // Reset the timer
      countdown = random(2, 6) * 1000; // Randomize the next countdown
      digitalWrite(GLED, LOW);
      digitalWrite(RLED, HIGH);
    }
    else if (!isGreenLight && (currentTime - previousTime >= countdown)) {
      isGreenLight = true;            // Switch to red light
      previousTime = millis();         // Reset the timer
      countdown = random(2, 6) * 1000; // Randomize the next countdown
      digitalWrite(GLED, HIGH);
      digitalWrite(RLED, LOW);
    }
    // If the light is green, keep the green LED on
    // if ((isGreenLight) && (currentTime - previousTime >= countdown)) {
    //   digitalWrite(GLED, HIGH);
    //   digitalWrite(RLED, LOW);
    // }
    // else if ((!isGreenLight) && (currentTime - previousTime >= countdown)) {
    //   digitalWrite(GLED, LOW);
    //   digitalWrite(RLED, HIGH);
    // }
  }
    lcd.clear();

  // Game over screen
  if (win == true){
    lcd.setCursor(1, 0);
    lcd.print("GAME OVER! WIN");
    delay(2000);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER! LOSER");
    delay(2000);
  }

  // lcd.clear();
  // lcd.setCursor(3, 0);
  digitalWrite(RLED, HIGH); // Red light stays on
  delay(2000); // Wait before restarting
  
}


void memoryGame() {
  unsigned long buttonPressStartTime = 0;
  const unsigned long LONG_PRESS_DURATION = 1000; //joystick manual ender


  bool memoryGame = true;
  lcd.clear();
  Serial.begin(9600);

  const int gridWidth = 8;   // Grid width (8 columns)
  const int gridHeight = 2;  // Grid height (2 rows)
  char grid[gridHeight][gridWidth];  // Stores card symbols
  char hiddenGrid[gridHeight][gridWidth];  // Hidden state for display ('*')

  int characterPosX = 0;    // Character X position
  int characterPosY = 0;    // Character Y position
  bool gameOver = false;

  const int rightThreshold = 900;
  const int leftThreshold = 200;
  const int upThreshold = 200;
  const int downThreshold = 900;

  // Initialize and shuffle the symbols
  char symbols[] = {'A', 'B', 'C', 'D', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'E', 'F', 'G', 'H'};
  for (int i = 0; i < 16; i++) {
    int r = random(0, 16);
    char temp = symbols[i];
    symbols[i] = symbols[r];
    symbols[r] = temp;
  }

  int index = 0;
  for (int row = 0; row < gridHeight; row++) {
    for (int col = 0; col < gridWidth; col++) {
      grid[row][col] = symbols[index++];
      hiddenGrid[row][col] = '*';  // Initially hide all symbols
    }
  }

  int firstRow = -1, firstCol = -1;  // First card selected
  unsigned long lastButtonPress = 0;
  const unsigned long debounceDelay = 200;  // Debounce time in milliseconds

  while (!gameOver) {
    // Handle joystick movement

    int joystickX = analogRead(joystickXPin);
    int joystickY = analogRead(joystickYPin);

    if (joystickX <= leftThreshold && characterPosX < gridWidth - 1) {
      ++characterPosX;  // Move right
    } else if (joystickX >= rightThreshold && characterPosX > 0) {
      --characterPosX;  // Move left
    }

    if (joystickY >= downThreshold && characterPosY > 0) {
      --characterPosY;  // Move up
    } else if (joystickY <= upThreshold && characterPosY < gridHeight - 1) {
      ++characterPosY;  // Move down
    }

    // Display the grid with spaces
    lcd.clear();
    for (int row = 0; row < gridHeight; row++) {
      lcd.setCursor(0, row);
      for (int col = 0; col < gridWidth; col++) {
        if (row == characterPosY && col == characterPosX) {
          lcd.print(char(7));  // Highlight the current position
        } else {
          lcd.print(hiddenGrid[row][col]);  // Display hidden or revealed symbol
        }
        if (col < gridWidth - 1) {
          lcd.print(' ');  // Add space between symbols
        }
      }
    }

    // Handle card selection with debounce
    if (digitalRead(buttonPin) == LOW && millis() - lastButtonPress > debounceDelay) {
      lastButtonPress = millis();
      
      if (firstRow == -1 && firstCol == -1) {
        // First card selected
        firstRow = characterPosY;
        firstCol = characterPosX;
        hiddenGrid[firstRow][firstCol] = grid[firstRow][firstCol];  // Reveal card
      } else if (firstRow != characterPosY || firstCol != characterPosX) {
        // Second card selected (different from the first)
        int secondRow = characterPosY;
        int secondCol = characterPosX;
        hiddenGrid[secondRow][secondCol] = grid[secondRow][secondCol];  // Reveal card

        // Update grid display with spaces
        lcd.clear();
        for (int row = 0; row < gridHeight; row++) {
          lcd.setCursor(0, row);
          for (int col = 0; col < gridWidth; col++) {
            lcd.print(hiddenGrid[row][col]);
            if (col < gridWidth - 1) {
              lcd.print(' ');  // Add space between symbols
            }
          }
        }

        // Check for a match
        if (grid[firstRow][firstCol] == grid[secondRow][secondCol]) {
          // Match found, leave cards revealed
          Serial.println("Match!");
          
          // Check if the game is over
          gameOver = true;
          for (int row = 0; row < gridHeight; row++) {
            for (int col = 0; col < gridWidth; col++) {
              if (hiddenGrid[row][col] == '*') {
                gameOver = false;
                break;
              }
            }
            if (!gameOver) break;
          }
        } else {
          // No match, hide cards again
          hiddenGrid[firstRow][firstCol] = '*';
          hiddenGrid[secondRow][secondCol] = '*';
        }

        // Reset selection
        firstRow = -1;
        firstCol = -1;
      }
    }

    delay(1250);  // Short delay for smooth joystick input
  }

  // Game over message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Win!");
  delay(2000);
}


    
  
void loop() {
  showMenu();  // Show the game menu
}
