#include <Servo.h>

Servo damper; //creates servo object that will call servo functions

String readString; //incoming string
char inData[7]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0; // Index into array; where to store the character


//HEAT LEVELS
enum HeatLevel 
{
  heat0=0,//no heat
  heat1=1,//1 heating coil
  heat2=2//2 heating coils
};


//FAN LEVELS
enum FanLevel
{
  fan0=0, //fan off
  fan1=95, //moving air
  fan2 =130, //strong breeze
  fan3 =170, //creates wind flow
  fan4 =200, //used for large flow of wind
  fan5 =245 //used for powerful explosions and strong gusts of wind
};


//ARDUINO PINS USED
enum Pins
{
  FanPin = 11,//fan motor speed control 0-255
  damperPin = 5,//damper, Servo that controlls hot or cold flow, if 50 will allow ambient flow, if 110 will flow to heat
  
  limSwitchPin = 12,// 12new limit switch in drive train. Starts pressed HIGH, released to LOW, HIGH again once full burst gear turn
  burstPin = 3,//burst motor speed pin 0-255 corresponds to 100RPM to 10,000RPM; not used to control on/off

  heatPin1 = 7,//control on/off of heaters. If pin is low, will turn heater on
  heatPin2 = 8,//control on/off of heaters. If pin is low, will turn heater on

  burstControlPin = 4, // sets off burst

  redPin = 9,
  greenPin = 6,
  bluePin = 10,

  //testLEDPin = 13
};

// DEAFULT LEVELS
enum DefaultParameters
{
  defaultHeat=0, //normal off state
  defaultFan=90, // normal wind and heat amounts
  damperAmbient = 133,// 50 damper position allows heat
  damperHeat = 155 //106 damper position allows ambient
};




// GLOBAL VARIABLES
int fval = 0; int prevFval = 0;
int dval = damperAmbient; int prevDval = damperAmbient;
int hval = 0; int prevHval = 0;
int CurrentCoil = 1;// used to keep track of which coils have been recently used. Alternate to reduce overheat

int limitSwitchCounter = 0;
int limitSwitchState=0;
int lastlimitSwitchState=-1;

int count = 0;
int actualcount = 0;

int n_rev = 0;

boolean HeatPinsInitialized = false;

int Red =0;
int Green = 0;
int Blue = 0;

boolean IsHeatOn = false;

//


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);


  TCCR2B = (TCCR2B & 0b11111000) | 0x01; //Increase PWM frequency to eliminate motor buzzing
  damper.attach(damperPin); //must go after pwm frequency is adjusted, connects servo object to its pin

  damper.write(damperAmbient);
}

void InitializeHeatPins()
{
  pinMode(heatPin1, INPUT_PULLUP);//prevent pins from firing when resetting arduino
  pinMode(heatPin2, INPUT_PULLUP);
 
  pinMode(heatPin1, OUTPUT); //set pins as outputs or inputs
  pinMode(heatPin2, OUTPUT);
 
  pinMode(burstControlPin,OUTPUT );

  pinMode(limSwitchPin, INPUT); // set your switch pin as an input so that it can be read by the arduino
  pinMode(burstPin, OUTPUT);

  HeatPinsInitialized = true;

  //StartDevice();
  
}

void StartDevice()
{

   analogWrite(greenPin, 15);
   analogWrite(greenPin, 40);
   analogWrite(greenPin, 90);
   analogWrite(greenPin, 130);
   analogWrite(greenPin, 190);
   analogWrite(greenPin, 255);


   delay(500);
   analogWrite(greenPin, 255);
   analogWrite(greenPin, 190);
   analogWrite(greenPin, 130);
   analogWrite(greenPin, 90);
   analogWrite(greenPin, 40);
   analogWrite(greenPin, 0);
   
   delay(500);
   analogWrite(greenPin, 15);
   analogWrite(greenPin, 40);
   analogWrite(greenPin, 90);
   analogWrite(greenPin, 130);
   analogWrite(greenPin, 190);
   analogWrite(greenPin, 255);

   delay(500);
   analogWrite(greenPin, 255);
   analogWrite(greenPin, 190);
   analogWrite(greenPin, 130);
   analogWrite(greenPin, 90);
   analogWrite(greenPin, 40);
   analogWrite(greenPin, 15);
   analogWrite(greenPin, 0);

   delay(500);
   analogWrite(greenPin, 15);
   analogWrite(greenPin, 40);
   analogWrite(greenPin, 90);
   analogWrite(greenPin, 130);
   analogWrite(greenPin, 190);
   analogWrite(greenPin, 255);
  
  
}


void StopDevice()
{
  setHeaters(0);
  delay(5);
  SetFan(0);
  delay(5);
  Red=0;Green=0;Blue=0;
  SetColorRing();

 
}

void serialRead() 
{ 
  while (Serial.available()) 
  {
  delay(5);  
  if (Serial.available() >0) 
  {
    char c = Serial.read();
    readString += c;}
  }
}



void setHeaters(int hlevel) 
{
    hval = hlevel;

    if(hval==0)
    {
      //digitalWrite(testLEDPin, LOW);
      IsHeatOn = false;

      if(fval>0)
      {
        Red = 0;
        Green = 0;
        Blue = 255;
      }
      else
      {
        Red = 0;
        Green = 255;
        Blue = 0;
      }
      SetColorRing();
    }
    else if(hval==1)
    {
       //digitalWrite(testLEDPin, HIGH);
      IsHeatOn = true;
      
      Red = 255;
      Green = 0;
      Blue = 0;
      SetColorRing();
    }
    else
    {
      
    }


    if (hval == 1) 
    {//alternates coils used
      
      if (CurrentCoil == 1) 
      {
        //arduino.digitalWrite((int)Pins.heatPin1, Arduino.LOW);//if pin is set to Arduino.LOW, will cause heater to turn on. We can change this next time. Wired incorrectly
        //arduino.digitalWrite((int)Pins.heatPin2, Arduino.HIGH);
        //CurrentCoil++;

        digitalWrite(heatPin1, HIGH);//pin 7
        digitalWrite(heatPin2, LOW);//pin 8
      }
      else if (CurrentCoil == 2) 
      {
        digitalWrite(heatPin1, HIGH);
        digitalWrite(heatPin2, HIGH);
        CurrentCoil = 1;
      }
    }
    else if (hval == 2) 
    {
      
      digitalWrite(heatPin1, LOW);
      digitalWrite(heatPin2, LOW);

    }

    else 
    {
      
      digitalWrite(heatPin1, HIGH);
      digitalWrite(heatPin2, HIGH);

    }


}

void loop() 
{
  if(HeatPinsInitialized==false)
  {
    InitializeHeatPins();
  }

  if (Serial.available() > 0)
  {
    
   readString = "";
   serialRead();
   //delay(5);

   if (readString == "ON")
    {
      StartDevice();
    }
   else if (readString == "OFF")
    {
      StopDevice();
    }
    
   String first = readString.substring(0,1);
   String lastThree =  readString.substring(1,4);
   int paraValue = lastThree.toInt();

      
    if (first == "F")
    {
      SetFan(paraValue);
    }
    else if (first == "D")
    {
      SetDamper(paraValue); 
    }
    else if (first == "B")
    {
      Burst(paraValue);
    }
    else if (first == "H")
    {
      setHeaters(paraValue);
    }
   
  }

  
  delay(5);
}


void SetFan(int val)
{
  fval = val;

  if(val>=0 && val<=255)
  {
    
    if(IsHeatOn==false)
    {
      Red = 0;
      Green = 0;
      Blue = 255;
      SetColorRing();
    }
    
    analogWrite(FanPin,fval);
  }
  
}

void SetDamper(int val)
{
  dval = val;
  if(val>=damperAmbient && val<=damperHeat)
  {
    
     damper.write(val);
  }
}


void Burst(int num)
{
  n_rev = num;
  
  if(n_rev<=0)
  {
    return;
  }
   
   while (n_rev > actualcount) 
   {
                
        //this part of the code keeps count and compares to the count
        analogWrite(burstPin,150);
        
        
        int switch1 = digitalRead(limSwitchPin); 
       
          if (switch1 == LOW)
          {
              count = count+1;
              delay(50);
              
          } 
          else if(count >0 && switch1 == HIGH)
          {
              count = 0;
              actualcount = actualcount + 1;
  
          }    
    }
  
  
  analogWrite(burstPin, 0);
  
  
  actualcount = 0;
  n_rev=0;

}


void SetRedLED(int val)
{
  Red = val;
  if(val>=0 && val<=255)
  {
    
  }
}

void SetGreenLED(int val)
{
  Green = val;
  if(val>=0 && val<=255)
  {
    
  }
}

void SetBlueLED(int val)
{
  Blue = val;
  if(val>=0 && val<=255)
  {
    
  }
}


void SetColorRing()
{
  analogWrite(redPin, Red);
  analogWrite(greenPin, Green);
  analogWrite(bluePin, Blue);
}

void moveDamper(int heat) 
{ 
  //checks to see if the current heating level has any heat going on, if it does, will move damper to the hot side and let air through
  if (heat == heat0) 
  {
    damper.write(damperAmbient);
  }
  else 
  {
    damper.write(damperHeat);
  }
}

void until(int duration) 
{
  //if the duration value is 0, will run continuously at current heat/fan
  if (duration == 0) 
  { 
  }
  else 
  {
    delay(duration);
    SetFan(0);
    setHeaters(0);
    moveDamper(0);
    
  }
}


