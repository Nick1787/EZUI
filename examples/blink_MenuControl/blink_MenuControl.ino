#include<EZUI.h>
#include<TimerOne.h>

//- Hardware Setup --------------------------------------------------

//Instantiate The hardware - LCD
#define LCD_I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is 0x27
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define LCD_BACKLIGHT_PIN     3
LiquidCrystal_I2C LCD(  LCD_I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

//Instantiate The hardware - Encoder
#define ENC_A 32
#define ENC_B 31
#define ENC_SW 33
#define ENC_DEG_PER_NOTCH 4
EZUI_ClickEncoder ENCODER( ENC_A, ENC_B, ENC_SW, ENC_DEG_PER_NOTCH );
void serviceEncoders(){
  ENCODER.service();
}

//- UI Code --------------------------------------------------

//Instantiate a UI and the associated hardware
EZUI UI;

//UI Control Variables to Control Enabling program adn Blink Frequency
bool BlinkEnabled = true;                               //Whether or not to run the program.
AdjustableParam BlinkFrequency( 2.0 , 1.0 , 10.0, 1.0 );     //Create an Adjustable Parameter.  NominalValue: 1Hz, MinValue: 1Hz, MaxValue: 10Hz, Increment: 1Hz;

//Create a new LCDMenu and UI controls for (a) Enabling the program (b) Adjusting the blink frequency
EZUI_Menu Menu_BlinkProgram;
EZUI_Control_ToggleOption Tgl_EnableProgram("RunProgram:", &BlinkEnabled, "Run", "Stop");
EZUI_Control_AdjustParam Adj_BlinkFrequency("Frequency:", &BlinkFrequency);

//Group the Menu Controls into an array
EZUI_MenuItems Menu_BlinkProgram_Items[] {
    {&Tgl_EnableProgram},
    {&Adj_BlinkFrequency}
};
  
//- Setup Function --------------------------------------------------

void setup() {
  //Initialize Serial
  Serial.begin(9600);
  Serial.print("Setup...");
  
  //Initialize the LED
  pinMode(13, OUTPUT);
  
  //Initialize the LCD with Backlight on;
  LCD.setBacklightPin(LCD_BACKLIGHT_PIN,POSITIVE);
  LCD.setBacklight(HIGH);
  LCD.begin(20,4);
  
  // Attatch the hardware to the UI
  UI.attatchEncoder(&ENCODER);
  UI.attatchLCD(&LCD);

  //Initialize the Menu and attatch the menu items
  Menu_BlinkProgram.setItems(A(Menu_BlinkProgram_Items));

  //Set the Display to show the Menu
  UI.setDisplay(&Menu_BlinkProgram);

  //Attatch Timer to service the encoder
  Timer1.initialize(1000);
  Timer1.attachInterrupt(serviceEncoders);
 
  Serial.println("Complete");
}

//- Main-Program --------------------------------------------------
unsigned long lastBlinkTimeMs = 0;
bool LEDon = false;

void loop() {
  UI.display();      //Run Display

  unsigned long currentTimeMs = millis();
  float adjBlinkPeriodMs = 1000 * 1/BlinkFrequency.getValue();

  //Check if Program is Enabled (controlled by UI)
  if(BlinkEnabled){
    
    //Has enough time passed that a blink should happen?
    if( (currentTimeMs - lastBlinkTimeMs) > adjBlinkPeriodMs ){
      lastBlinkTimeMs = currentTimeMs;
  
      if(LEDon){
        Serial.print("Light On: ");
        Serial.print(currentTimeMs/1000);
        Serial.println(" seconds");
        
        //Turn off LED
        digitalWrite(13, LOW);
        LEDon = false;
      }else{
        Serial.print("Light Off: ");
        Serial.print(currentTimeMs/1000.0);
        Serial.println(" seconds");
        
        //Turn on LED
        digitalWrite(13, HIGH);
        LEDon = true;
      }
    }
  }
}
