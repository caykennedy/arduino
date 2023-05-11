#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define ROW1PIN 21
#define ROW2PIN 3
#define ROW3PIN 4
#define ROW4PIN 5
#define ROW5PIN 6

//declare global variables (pin vals, etc)
  //initialize pins
  const int knobPin = A0;
  const int screamPin = A5;
  const int pausePin = 17;


  int baseSound = 0;
  int knobNum = 0;
  int screamNum = 0;
  int pauseNum = 0;
  int cnum = 0;
  int rnum = 0;
  int r1c = 0;
  int count = 0;
  int count2 = 0;
  int power = 0;
  int interval = 1000;
  bool noAttack = true;
  bool isPause = true;
  int range = 0;
  int i = 0;

  //neopixel strip
  Adafruit_NeoPixel row1 = Adafruit_NeoPixel(5, ROW1PIN, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel row2 = Adafruit_NeoPixel(5, ROW2PIN, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel row3 = Adafruit_NeoPixel(5, ROW3PIN, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel row4 = Adafruit_NeoPixel(5, ROW4PIN, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel row5 = Adafruit_NeoPixel(5, ROW5PIN, NEO_GRB + NEO_KHZ800);


void setup() {

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pausePin, INPUT);

  //initialize neopixels
  row1.begin();
  row1.show();

  row2.begin();
  row2.show();

  row3.begin();
  row3.show();

  row4.begin();
  row4.show();

  row5.begin();
  row5.show();

}

void loop() {

  //read numbers from the pins
  knobNum = analogRead(knobPin);
  screamNum = analogRead(screamPin);
  pauseNum = digitalRead(pausePin);

  //take baseline if this is the beginning of the game
  if (count == 0)
  {
    baseSound = takeSound();
  }

  //pause or unpause if the pause button is pressed
  if (pauseNum == 1)
  {
    pause();
  }


//prints to the serial monitor for testing purposes
  // Serial.print(baseSound);
  // Serial.print(',');
  // Serial.print(screamNum);
  // Serial.print(',');
  // Serial.print(knobNum);
  // Serial.print(',');
  // Serial.println(pauseNum);

  //only run if the game is not paused
  if (isPause == false)
    {

      count++;
      
      //bottom LED strip will be the "ship," controlled by the potentiometer
      steerShip();


      //attackers: in random columns
      if (noAttack == true)
      {
        generateEnemy();
      }
      
      //attackers: descend at a certain time interval
      if (count%interval == 0)
      {
        descend();
      }

      //shoot: light up the column the ship is in, controlled by sound sensor, higher sound reading = higher row
      if (screamNum != baseSound)
      {
        shoot();
      }
      
      if (screamNum <= baseSound)
      {
        row5.setPixelColor(r1c,0,0,0);
        row4.setPixelColor(r1c,0,0,0);
        row3.setPixelColor(r1c,0,0,0);
        row2.setPixelColor(r1c,0,0,0);
        row2.show();
        row3.show();
        row4.show();
        row5.show();
        power = 0;
        displayEnemy();
      }

      //on hit: green screen
      if (r1c == cnum && power >= rnum)
      {
        hit();
      }

      //on fail: red screen
      if (rnum == 1)
      {
        fail();
      }
    }
}

/*
    hit - triggered when an attacker ship is hit, lights up the screen green and resets the attacker
*/
void hit()
{
      for (int i = 0; i<5; i++)
    {
      row1.setPixelColor(i,0,255,0);
      row2.setPixelColor(i,0,255,0);
      row3.setPixelColor(i,0,255,0);
      row4.setPixelColor(i,0,255,0);
      row5.setPixelColor(i,0,255,0);
    }

    row1.show();
    row2.show();
    row3.show();
    row4.show();
    row5.show();

    delay(500);

    row1.clear();
    row2.clear();
    row3.clear();
    row4.clear();
    row5.clear();

    row1.show();
    row2.show();
    row3.show();
    row4.show();
    row5.show();

    rnum = 5;
    noAttack = true;
}

/*
    fail - triggered when the attacker ship reaches the first row, lights up the screen red and resets the attacker
*/
void fail()
{
    for (int i = 0; i<5; i++)
    {
      row1.setPixelColor(i,255,0,0);
      row2.setPixelColor(i,255,0,0);
      row3.setPixelColor(i,255,0,0);
      row4.setPixelColor(i,255,0,0);
      row5.setPixelColor(i,255,0,0);
    }

    row1.show();
    row2.show();
    row3.show();
    row4.show();
    row5.show();

    delay(500);

    row1.clear();
    row2.clear();
    row3.clear();
    row4.clear();
    row5.clear();

    noAttack = true;
    rnum = 5;
}

/*
    shoot - reads current sound level and shoots
*/
void shoot()
{
  if (screamNum > (baseSound + i))
  {
    row2.setPixelColor(r1c, 0,0,250);
    row2.show();
    power = 2;
  }
  //up to row 3
  if (screamNum > (baseSound + (2*i)))
  {
    row3.setPixelColor(r1c,0,0,250);
    row3.show();
    power = 3;
  }
  //up to row 4
  if (screamNum > (baseSound + (3*i)))
  {
    row4.setPixelColor(r1c,0,0,250);
    row4.show();
    power = 4;
  }
  //up to row 5
  if (screamNum > (baseSound + (4*i)))
  {
    row5.setPixelColor(r1c,0,0,250);
    row5.show();
    power = 5;
  }
}

/*
    generateEnemy - creates an enemy at a random column on the top row
*/
void generateEnemy()
{
  cnum = rand()%5;
  row5.setPixelColor(cnum, 250, 0, 0);
  row5.show();
  noAttack = false;
}

/*
    steerShip - moves the ship across the bottom row dependent on potentiometer reading
*/
void steerShip()
{
  row1.clear();
  row2.clear();
  row3.clear();
  row4.clear();
  row5.clear();
  //column 1
  if (knobNum >= 0 && knobNum < 204)
  {
    row1.setPixelColor(0, 250,50,50);
    r1c=0;
  }
  //column 2
  else if (knobNum >= 204 && knobNum < 408)
  {
    row1.setPixelColor(1, 250,50,50);
    r1c=1;
  }
  //column 3
  else if (knobNum >= 408 && knobNum < 612)
  {
    row1.setPixelColor(2, 250,50,50);
    r1c=2;
  }
  //column 4
  else if (knobNum >= 612 && knobNum < 816)
  {
    row1.setPixelColor(3, 250,50,50);
    r1c=3;
  }
  //column 5
  else
  {
    row1.setPixelColor(4, 250,50,50);
    r1c=4;
  }

  displayEnemy();
  row1.show();
  row2.show();
  row3.show();
  row4.show();
  row5.show();
}

/*
    descend - make enemy ship descend a row
*/
void descend()
{
    rnum--;
    cnum = rand()%5;
    //go down to row 4
    if (rnum == 4)
    {
      row5.clear();
      row4.setPixelColor(cnum, 255, 0, 0);
      row5.show();
      row4.show();
    }
    //go down to row 3
    else if (rnum == 3)
    {
      row4.clear();
      row3.setPixelColor(cnum, 255, 0, 0);
      row4.show();
      row3.show();
    }
    //go down to row 2 (closest to ship)
    else if (rnum == 2)
    {
      row3.clear();
      row2.setPixelColor(cnum, 255, 0, 0);
    }
}

/*
    pause - pause or unpause the game
*/
void pause(){
  if (isPause == true){
    isPause = false;
  }

  else
  {
    isPause = true;
  }

  count = 1;
}

/*
  displayEnemy - make sure enemy remains displayed when it is not defeated
*/
void displayEnemy(){
  if (rnum == 5){
      row5.setPixelColor(cnum, 255, 0, 0);
    row5.show();
  }
  else if (rnum == 4){
      row4.setPixelColor(cnum, 255, 0, 0);
    row4.show();
  }
  else if (rnum == 3){
      row3.setPixelColor(cnum, 255, 0, 0);
    row3.show();
  }
  else if (rnum == 2){
      row2.setPixelColor(cnum, 255, 0, 0);
    row2.show();
  }

}

/*
    takeSound - takes a baseline sound number and converts it to be a range read by the shoot function

    @return screamNum 
*/
int takeSound(){
  delay(1000);
  range = 1023 - screamNum;
  i = range /5;
  return screamNum;
}