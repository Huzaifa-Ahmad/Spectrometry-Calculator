#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

// Define Orbital Switches
#define ORBIT0 2
#define ORBIT1 3
#define ORBIT2 4
#define ORBIT3 5
#define ORBIT4 6
#define ORBIT5 7

// Define Spectrum Lights
#define RED_STRIP 8
#define CYAN_STRIP 9
#define BLUE_STRIP 10
#define VIOLET_STRIP 11

#define NUM_PIXELS 8

Adafruit_NeoPixel RED = Adafruit_NeoPixel(NUM_PIXELS, RED_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel CYAN = Adafruit_NeoPixel(NUM_PIXELS, CYAN_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel BLUE = Adafruit_NeoPixel(NUM_PIXELS, BLUE_STRIP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel VIOLET = Adafruit_NeoPixel(NUM_PIXELS, VIOLET_STRIP, NEO_GRB + NEO_KHZ800);

//Creating LCD 
LiquidCrystal_I2C LCD(0x27,20,4);

// Creating Custom Characters
uint8_t Delta[8] = {
  0x4,
  0x4,
  0xa,
  0xa,
  0x11,
  0x11,
  0x1f,
  0x0
};

uint8_t L[8] = {
  0x8,
  0xf4,
  0x4,
  0x4,
  0x4,
  0xe,
  0xf2,
  0xf3  
};

uint8_t x[8] = {
  0x0,
  0x0,
  0x0,
  0xa,
  0x4,
  0xa,
  0x0,
  0x0
};

uint8_t exponent[8] = {
  0x4,
  0xa,
  0x11,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0
};

// Defining Reset Button
#define RESET_SWITCH 12

// Declaring Variables
int delayval = 100;
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int initialOrbital, finalOrbital;
double lambda, E;


void setup()
{
  // Setting Data Rate
  Serial.begin(9600);

  // Setting pins
  pinMode(RED_STRIP, OUTPUT);
  pinMode(CYAN_STRIP, OUTPUT);
  pinMode(BLUE_STRIP, OUTPUT);
  pinMode(VIOLET_STRIP, OUTPUT);

  pinMode(ORBIT0, INPUT);
  pinMode(ORBIT1, INPUT);
  pinMode(ORBIT2, INPUT);
  pinMode(ORBIT3, INPUT);
  pinMode(ORBIT4, INPUT);
  pinMode(ORBIT5, INPUT);

  pinMode(RESET_SWITCH, INPUT);

  // Setting up Lights
  RED.begin();
  RED.show();

  CYAN.begin();
  CYAN.show();

  BLUE.begin();
  BLUE.show();

  VIOLET.begin();
  VIOLET.show();

  // Settting up LCD
  LCD.init();
  LCD.backlight();

  // Custom Character
  LCD.createChar(0,Delta);
  LCD.createChar(1,L);
  LCD.createChar(2,x);
  LCD.createChar(3,exponent);
  
}

void loop()
{
  startProgram();
  easterEgg();
}

void startProgram()
{
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Set Initial Orbital");
  initialOrbital = getOrbital();
  waitForAllClear();
  LCD.setCursor(0, 0);
  LCD.print("Set Final Orbital");
  finalOrbital = getOrbital();

  displayOrbitals(initialOrbital, finalOrbital);
  delay(1000);
  LCD.clear();

  lambda = calcwavelength(initialOrbital, finalOrbital);
  E = calcEnergy(initialOrbital, finalOrbital);

  displayAnalysis(initialOrbital, finalOrbital, lambda, E);

  if (finalOrbital == 2 && initialOrbital > finalOrbital)
    displayLight(initialOrbital);

  // reset();
}

int getOrbital()
{
  int currentOrbital = -1;

  while (currentOrbital < 0)
    currentOrbital = checkButtonPressed();

  return currentOrbital;
}

int checkButtonPressed()
{
  delay(2000);
  while (digitalRead(ORBIT0) == LOW)
    return 1;

  while (digitalRead(ORBIT1) == LOW)
    return 2;

  while (digitalRead(ORBIT2) == LOW)
    return 3;

  while (digitalRead(ORBIT3) == LOW)
    return 4;

  while (digitalRead(ORBIT4) == LOW)
    return 5;

  while (digitalRead(ORBIT5) == LOW)
    return 6;

  return (-1);
}

void waitForAllClear()
{
  LCD.setCursor(0, 1);
  LCD.print("Move the Electron");
  while (digitalRead(ORBIT0) == LOW || digitalRead(ORBIT1) == LOW || digitalRead(ORBIT2) == LOW ||
         digitalRead(ORBIT3) == LOW || digitalRead(ORBIT4) == LOW || digitalRead(ORBIT5) == LOW)
    delay(1000);

  LCD.clear();
  LCD.setCursor(0, 2);
}

void displayOrbitals(int initialOrbital, int finalOrbital)
{
  delay(1000);
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Orbital 1: " + String(initialOrbital));
  LCD.setCursor(0, 1);
  LCD.print("Orbital 2: " + String(finalOrbital));
}

double calcwavelength(int initialOrbital, int finalOrbital)
{
  double R = 10973731.6;
  double invni = initialOrbital * initialOrbital;
  double invnf = finalOrbital * finalOrbital;
  double nf = 1 / (invnf);
  double ni = 1 / (invni);
  double nDelta = nf - ni;
  nDelta = abs(nDelta);
  double invLambda = R * nDelta;
  double lambda = 1e9 / invLambda;

  return lambda;
}

double calcEnergy(int initialOrbital, int finalOrbital)
{
  double k = 2.1789601284e-18;
  double invni = initialOrbital * initialOrbital;
  double invnf = finalOrbital * finalOrbital;
  double nf = 1 / (invnf);
  double ni = 1 / (invni);
  double nDelta = nf - ni;
  nDelta = abs(nDelta);
  double E = k * nDelta;
  E = 1e19 * E;
  return E;
}

void displayAnalysis(int initialOrbital, int finalOrbital, double lambda, double E)
{
  if (initialOrbital != finalOrbital)
  {
    LCD.home();

    LCD.print("Orbits: ");
    LCD.setCursor(8,0);
    LCD.print(initialOrbital);
    LCD.print(" --> ");
    LCD.print(finalOrbital);

    LCD.setCursor(0,1);
    LCD.print(char(0));
    LCD.setCursor(1, 1);
    LCD.print("E = ");
    LCD.setCursor(5, 1);
    if (initialOrbital > finalOrbital)
      LCD.print(E);
    else if (initialOrbital < finalOrbital)
      LCD.print(-E);
    LCD.setCursor(9,1);
    LCD.print(char(2));
    LCD.setCursor(10,1);
    LCD.print("10");
    LCD.setCursor(12,1);
    LCD.print(char(3));
    LCD.print("-19 J");

    LCD.setCursor(0,2);
    LCD.print(char(1));
    LCD.print(" = ");
    LCD.print(lambda);
    LCD.setCursor(7,2);
    LCD.print(" nm");
    LCD.setCursor(11,2);
    
    if (finalOrbital == 5 || finalOrbital == 4 || finalOrbital == 3)
      LCD.print("(IR)");
    else if (finalOrbital == 2)
      LCD.print("(VISIBLE)");
    else if (finalOrbital == 1)
      LCD.print("(UV)"); 

    LCD.setCursor(0,3);
    LCD.print("Series:");
    LCD.setCursor(8,3);
    
    if (finalOrbital == 5)
      LCD.print("PFUND");
    else if (finalOrbital == 4)
      LCD.print("PASCHEN");
    else if (finalOrbital == 3)
      LCD.print("BRACKETT");
    else if (finalOrbital == 2)
      LCD.print("LYMAN");
  }
  else if (initialOrbital == finalOrbital)
  {
    LCD.home();
    LCD.print("No Energy Change!");
    
  }
}

void displayLight(int initialOrbital)
{
  uint32_t red_high = RED.Color(255, 0, 0);
  uint32_t cyan_high = CYAN.Color(0, 255, 255);
  uint32_t blue_high = BLUE.Color(13, 31, 224);
  uint32_t violet_high = VIOLET.Color(128, 17, 219);

  if (initialOrbital == 3)
  {
    digitalWrite(RED_STRIP, HIGH);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      RED.setPixelColor(i, red_high);
      RED.show();
    }
  }
  else if (initialOrbital == 5)
  {
    digitalWrite(BLUE_STRIP, HIGH);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      BLUE.setPixelColor(i, blue_high);
      BLUE.show();
    }
  }
  else if (initialOrbital == 4)
  {
    digitalWrite(CYAN_STRIP, HIGH);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      CYAN.setPixelColor(i, cyan_high);
      CYAN.show();
    }
  }
  else if (initialOrbital == 6)
  {
    digitalWrite(VIOLET_STRIP, HIGH);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      VIOLET.setPixelColor(i, violet_high);
      VIOLET.show();
    }
  }
}

void reset()
{
  boolean flag = checkReset();

  while (flag)
    flag = checkReset();
  
  // Resetting the lights & LCD
  LCD.home();

  uint32_t red_low = RED.Color(0, 0, 0);
  uint32_t cyan_low = CYAN.Color(0, 0, 0);
  uint32_t blue_low = BLUE.Color(0, 0, 0);
  uint32_t violet_low = VIOLET.Color(0, 0, 0);

  for (int i = 0; i < NUM_PIXELS; i++)
  {
    BLUE.setPixelColor(i, blue_low);
    RED.setPixelColor(i, red_low);
    CYAN.setPixelColor(i, cyan_low);
    VIOLET.setPixelColor(i, violet_low);

    RED.show();
    CYAN.show();
    BLUE.show();
    VIOLET.show();
  }
}

boolean checkReset()
{
  while (digitalRead(RESET_SWITCH) == LOW)
    return false;

  return true;
}

void easterEgg()
{

  boolean check = checkEasterEgg();
  while (check)
    check = checkEasterEgg();

  while (digitalRead(ORBIT0) == LOW && digitalRead(ORBIT1) == LOW && digitalRead(ORBIT2) == LOW &&
         digitalRead(ORBIT3) == LOW && digitalRead(ORBIT4) == LOW && digitalRead(ORBIT5) == LOW)
    setColor();

  for (int i = 0; i < NUM_PIXELS; i++)
  {
    RED.setPixelColor(i, RED.Color(redColor, greenColor, blueColor));
    CYAN.setPixelColor(i, CYAN.Color(redColor, greenColor, blueColor));
    BLUE.setPixelColor(i, BLUE.Color(redColor, greenColor, blueColor));
    VIOLET.setPixelColor(i, VIOLET.Color(redColor, greenColor, blueColor));

    RED.show();
    CYAN.show();
    BLUE.show();
    VIOLET.show();

    delay(delayval);

    if (i == NUM_PIXELS)
    {
      i = 0;
      setColor();
    }
  }
  LCD.clear();

  LCD.setCursor(7, 0);
  LCD.print("DESIGN");

  LCD.setCursor(8, 0);
  LCD.print("TEAM");

  LCD.setCursor(10, 2);
  LCD.print("1");

  LCD.setCursor(7, 3);
  LCD.print("BITCHES");

  reset();
}

boolean checkEasterEgg()
{
  while (digitalRead(ORBIT0) == LOW && digitalRead(ORBIT1) == LOW && digitalRead(ORBIT2) == LOW &&
         digitalRead(ORBIT3) == LOW && digitalRead(ORBIT4) == LOW && digitalRead(ORBIT5) == LOW)
    return false;

  return true;
}

void setColor()
{
  redColor = random(0, 255);
  greenColor = random(0, 255);
  blueColor = random(0, 255);
  Serial.print("red: ");
  Serial.println(redColor);
  Serial.print("green: ");
  Serial.println(greenColor);
  Serial.print("blue: ");
  Serial.println(blueColor);
}