#include <Firmata.h>
#include <FastLED.h>
//----- FAST LED ------
#define NUM_LEDS 60
#define DATA_PIN 6
#define LED_TYPE      WS2811 //2?
#define COLOR_ORDER   RGB
#define BRIGHTNESS  32
#define FRAMES_PER_SECOND  120
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
//----- MY COMMAND -----
#define MY_COMMAND 0x01
#define SET_PATTERN 0x01
#define SET_SOLID 0x02
#define UP_BRIGHTNESS 0x09
#define DOWN_BRIGHTNESS 0x0a
#define OFF_LEDS 0x0d

#define RUN 0x03
#define CONFIG 0x04
//---- ACCESSOR ----
#define ACCESSOR 0x05

#define SET 0x00
#define GET 0x01

#define VALUE 0x02
#define NUMBER 0x03
#define RED 0x04
#define GREEN 0x05
#define BLUE 0x06
#define STATICNUMBER 0x00
#define STATICDELTA 0x01
#define DYNAMICNUMBER 0x07
#define DYNAMICDELTA 0x08
#define COOLINGWATER 0x09
#define SPARKINGWATER 0x0a
#define COOLINGFIRE 0x0b
#define SPARKINGFIRE 0x0c
#define BLINKNUMBER 0x0d
#define CYLONTRAIL 0x0e
#define CYLONSPEED 0x0f
#define CYLONNUMBER 0x10
#define SPARKNUMBER 0x11
#define BPMNUMBER 0x12
#define BPMSPEED 0x13

uint8_t gHue = 0;
uint8_t speed = 30;
uint8_t number = 60;
CRGB solidColor = CRGB::Blue; 

//-------------------BRIGHTNESS---------------------
const uint8_t brightnessCount = 5;
uint8_t brightnessMap[brightnessCount] = { 16, 32, 64, 128, 255 };
uint8_t brightnessIndex = 0;
uint8_t brightness = 16;
//--------------------------------------------------


CRGB leds[NUM_LEDS]; 
CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
uint8_t currentPatternIndex = 0; // Index number of which paatern is current
uint8_t currentColorIndex = 0;

// DODAC SET GET dla parametrów

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100


// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.

uint8_t currentPaletteIndex = 0;

unsigned long actTime = 0;
unsigned long remTime = 0;
const unsigned long period = 800;
bool is_looping = false;
uint8_t r;
uint8_t g;
uint8_t b = 255;

//----- Rainbow Solid ----------
uint8_t staticNumber = 60;
uint8_t staticDelta = 7;
//-----------------------------
//----- Rainbow Dynamic --------
uint8_t dynamicNumber = 60;
uint8_t dynamicDelta = 1;
//-----------------------------
//----- Fire -------------------
uint8_t coolingFire = 49;
uint8_t sparkingFire = 60;
//-----------------------------
//---- Water -------------------
uint8_t coolingWater = 49;
uint8_t sparkingWater = 60;
//------------------------------
//---- Blink ------------------
uint8_t blinkNumber = 60;
//------------------------------
//---- Cylon -----------------
uint8_t cylonNumber = 60;
uint8_t cylontrail = 20;
uint8_t cylonspeed = 30;
//------------------------------
//---- Spark ------------------
uint8_t sparkNumber = 60;
//------------------------------
//---- BPM --------------------
uint8_t bpmspeed = 30;
uint8_t bpmNumber = 60;
//------------------------------

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

void showOneColor();
void BlinkPattern();
void offLeds();
void ConfettiPattern();
void WaterEffect();
void FireEffect();
void RainbowSolid();
void RainbowDynamic();
void fast();
void Cylon();
void bpm();
void chooseOneColor();

PatternAndNameList patterns = {
  { showOneColor,         "Solid Color"}, //0
  { BlinkPattern,         "Blink"},       //1
  { ConfettiPattern,         "Confetti"},       //2  
  { WaterEffect,          "Water"},       //3
  { FireEffect,           "Fire"},        //4
  { RainbowSolid,         "Rainbow Static"},//5
  { RainbowDynamic,       "Rainbow Dynamic"}, //6
  { Cylon,                 "Cylon"},        //7
  { offLeds,              "Off"},          //8
  { bpm,                  "bpm"},        //9
  { chooseOneColor,       "HexColor"}     //10
};
const uint8_t patternCount = 2;

typedef struct {
  CRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

const CRGBPalette16 palettes[] = {
  RainbowColors_p,
  RainbowStripeColors_p,
  CloudColors_p,
  LavaColors_p,
  OceanColors_p,
  ForestColors_p,
  PartyColors_p,
  HeatColors_p
};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

const String paletteNames[paletteCount] = {
  "Rainbow",
  "Rainbow Stripe",
  "Cloud",
  "Lava",
  "Ocean",
  "Forest",
  "Party",
  "Heat",
};

typedef void (*Color)();
typedef Color ColorList[];
typedef struct {
  Color color;
  String name;
} ColorAndName;
typedef ColorAndName ColorAndNameList[];

void showRed();
void showYellow();
void showGreen();
void showCyan();
void showBlue();
void showMagenta();
void showWhite();

ColorAndNameList colors = {
  { showRed,            "Solid Color Red"},     //0
  { showGreen,          "Solid Color Green"},   //1
  { showBlue,           "Solid Color Blue"},    //2
  { showYellow,         "Solid Color Yellow"},  //3     
  { showCyan,           "Solid Color Cyan"},    //4
  { showMagenta,        "Solid Color Magenta"}, //5
  { showWhite,          "Solid Color White"}    //6 
};

void showRed(){
 setOneColor(CRGB::Red);
}
void showYellow(){
 setOneColor(CRGB::Yellow);  
}
void showGreen(){
 setOneColor(CRGB::Green);
}
void showCyan(){
 setOneColor(CRGB::Cyan); 
}
void showBlue(){
  setOneColor(CRGB::Blue);
}
void showMagenta(){
  setOneColor(CRGB::Magenta);
}
void showWhite(){
  setOneColor(CRGB::White);
}

void setPalette(uint8_t value)
{
  if (value >= paletteCount)
    value = paletteCount - 1;

  currentPaletteIndex = value;
}

void Cylon()
{
  fadeToBlackBy( leds, cylonNumber, cylontrail);
  int pos = beatsin16(cylonspeed, 0, cylonNumber);
  static int prevpos = 0;
  solidColor = CRGB(r, g, b);
  //CRGB color = ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);
  if ( pos < prevpos ) {
    fill_solid( leds + pos, (prevpos - pos) + 1, solidColor);
  } else {
    fill_solid( leds + prevpos, (pos - prevpos) + 1, solidColor);
  }
  prevpos = pos;
}


void bpm()
{
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } 
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t beat = beatsin8( bpmspeed, 64, 255);
  CRGBPalette16 palette = palettes[currentPaletteIndex];
  for ( int i = 0; i < bpmNumber; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  //FastLED.show();
}

void setOneColor(CRGB color){
  setOneColor(color.r, color.g, color.b);
}
void chooseOneColor()
{
    if (r > 255)
    r = 255;
  else if (r < 0) r = 0;

  if (g > 255)
    g = 255;
  else if (g < 0) g = 0;

  if (b > 255)
    b = 255;
  else if (b < 0) b = 0;
  
  solidColor = CRGB(r, g, b);
  fill_solid(leds, number, solidColor);
}

void setOneColor(uint8_t r, uint8_t g, uint8_t b){
  solidColor = CRGB(r, g, b);
}
/*
void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }
*/
void ConfettiPattern()
{
  fadeToBlackBy( leds, sparkNumber, 10);
  int pos = random16(sparkNumber);
  // leds[pos] += CHSV( gHue + random8(64), 200, 255);
  //leds[pos] += ColorFromPalette(palettes[currentPaletteIndex], gHue + random8(64));
  leds[pos] += solidColor;
}
  
void showOneColor(){
  fill_solid(leds, number, solidColor);
  //FastLED.show();
}


void BlinkPattern()
{
  
  if(!is_looping) {
    solidColor = CRGB(r, g, b);
    fill_solid(leds, blinkNumber, solidColor);
  } else {
    fill_solid(leds, blinkNumber, CRGB::Black);
  }
}

void offLeds(){
  fill_solid(leds, 60, CRGB::Black);
  //FastLED.show();
}

void WaterEffect()
{  
    heatMapWater(IceColors_p, false);
   // FastLED.show();
}

void FireEffect()
{
  heatMapFire(HeatColors_p, true);
  //FastLED.show();
}


void heatMapFire(CRGBPalette16 palette, bool up)
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  random16_add_entropy(random(256));
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  byte colorindex;

  // Step 1.  Cool down every cell a little

  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((coolingFire * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( uint16_t k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparkingFire ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( uint16_t j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    colorindex = scale8(heat[j], 190);

    CRGB color = ColorFromPalette(palette, colorindex);

    if (up) {
      leds[j] = color;
    }
    else {
      leds[(NUM_LEDS - 1) - j] = color;
    }
  }
}

void heatMapWater(CRGBPalette16 palette, bool up)
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  random16_add_entropy(random(256));
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  byte colorindex;

  // Step 1.  Cool down every cell a little

  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((coolingWater * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( uint16_t k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparkingWater ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( uint16_t j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    colorindex = scale8(heat[j], 190);

    CRGB color = ColorFromPalette(palette, colorindex);

    if (up) {
      leds[j] = color;
    }
    else {
      leds[(NUM_LEDS - 1) - j] = color;
    }
  }
}
//--------STATIC------------------
void RainbowSolid()
{
  fill_rainbow( leds, staticNumber, gHue, staticDelta);

}

void RainbowDynamic()
{
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } 
  fill_rainbow( leds, dynamicNumber, gHue, dynamicDelta);
 // FastLED.show();
}

//--------BRIGHTNESSS-----------------------------
void changeBrightness(bool up)
  {
  if (up && brightnessIndex < brightnessCount - 1)
    brightnessIndex++;
  else if (!up && brightnessIndex > 0)
    brightnessIndex--;

  brightness = brightnessMap[brightnessIndex];

  FastLED.setBrightness(brightness);
  }
//------------------------------------------------

void setBrightness(uint8_t value)
{
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;

  brightness = value;

  FastLED.setBrightness(brightness);
}

void process_command1(byte argc, byte *argv){ 
    if (argc == 0) {
      return; 
    }
    switch (argv[0]) {
      case SET_PATTERN: //0x01
        currentPatternIndex = argv[1];
        break;
      case SET_SOLID:
        currentColorIndex = argv[1];
      break;
      case UP_BRIGHTNESS:
        changeBrightness(true);
      break;
      case DOWN_BRIGHTNESS:
        changeBrightness(false);
      break;
      case OFF_LEDS:
        offLeds();
      break;
  }     
}

void accessor_command(byte argc,byte *argv){
  if (argc < 2){
    return;
  }

  switch (argv[0]){
    case SET:
      switch (argv[1]){
        case NUMBER:
          number = argv[2];
          break;
        case RED:
          r = argv[2];
          break;
        case GREEN:
          g = argv[2];
          break;
        case BLUE:
          b = argv[2];
          break;
        case STATICNUMBER:
          staticNumber = argv[2];
          break;
        case STATICDELTA:
          staticDelta = argv[2];
          break;
        case DYNAMICNUMBER:
          dynamicNumber = argv[2];
          break;
        case DYNAMICDELTA:
          dynamicDelta = argv[2];
          break;
        case COOLINGWATER:
          coolingWater = argv[2];
          break;
        case SPARKINGWATER:
          sparkingWater = argv[2];
          break;
        case COOLINGFIRE:
          coolingFire = argv[2];
          break;
        case SPARKINGFIRE:
          sparkingFire = argv[2];
          break;
        case BLINKNUMBER:
          blinkNumber = argv[2];
          break;
        case CYLONTRAIL:
          cylontrail = argv[2];
          break;
        case CYLONSPEED:
          cylonspeed = argv[2];
          break;
        case CYLONNUMBER:
          cylonNumber = argv[2];
          break;
        case SPARKNUMBER:
          sparkNumber = argv[2];
          break;
        case BPMNUMBER:
          bpmNumber = argv[2];
          break;
        case BPMSPEED:
          bpmspeed = argv[2];
          break;
        }
        break;
    case GET:
      switch (argv[1]){
          case NUMBER: {
            uint8_t data[] = { VALUE, number};
            Firmata.sendSysex(ACCESSOR, 2, data);
          } break;
          case RED: {
            uint8_t data3[] = { VALUE, r};
            Firmata.sendSysex(ACCESSOR, 2, data3);
          } break; 
          case GREEN: {
            uint8_t data4[] = { VALUE, g};
            Firmata.sendSysex(ACCESSOR, 2, data4);
        } break;
        case BLUE: {
            uint8_t data5[] = { VALUE, b};
            Firmata.sendSysex(ACCESSOR, 2, data5);
        } break; 
        case STATICNUMBER: {
            uint8_t data6[] = { VALUE, staticNumber};
            Firmata.sendSysex(ACCESSOR, 2, data6);
        } break;
        case STATICDELTA: {
            uint8_t data7[] = { VALUE, staticDelta};
            Firmata.sendSysex(ACCESSOR, 2, data7);
        } break;
        case DYNAMICNUMBER: {
            uint8_t data8[] = { VALUE, dynamicNumber};
            Firmata.sendSysex(ACCESSOR, 2, data8);
        } break;
        case DYNAMICDELTA: {
            uint8_t data9[] = { VALUE, dynamicDelta};
            Firmata.sendSysex(ACCESSOR, 2, data9);
        } break;
        case COOLINGWATER: {
            uint8_t dataa[] = { VALUE, coolingWater};
            Firmata.sendSysex(ACCESSOR, 2, dataa);
        } break;
        case SPARKINGWATER: {
            uint8_t datab[] = { VALUE, sparkingWater};
            Firmata.sendSysex(ACCESSOR, 2, datab);
        } break;
        case COOLINGFIRE: {
            uint8_t datac[] = { VALUE, coolingFire};
            Firmata.sendSysex(ACCESSOR, 2, datac);
        } break;
        case SPARKINGFIRE: {
            uint8_t datad[] = { VALUE, sparkingFire};
            Firmata.sendSysex(ACCESSOR, 2, datad);
        } break;
        case BLINKNUMBER: {
            uint8_t datae[] = { VALUE, blinkNumber};
            Firmata.sendSysex(ACCESSOR, 2, datae);
        } break;
        case CYLONTRAIL: {
            uint8_t dataf[] = { VALUE, cylontrail};
            Firmata.sendSysex(ACCESSOR, 2, dataf);
        } break;
        case CYLONSPEED: {
            uint8_t datag[] = { VALUE, cylonspeed};
            Firmata.sendSysex(ACCESSOR, 2, datag);
        } break;
        case CYLONNUMBER: {
            uint8_t datah[] = { VALUE, cylonNumber};
            Firmata.sendSysex(ACCESSOR, 2, datah);
        } break;
        case SPARKNUMBER: {
            uint8_t datai[] = { VALUE, sparkNumber};
            Firmata.sendSysex(ACCESSOR, 2, datai);
        } break;
        case BPMNUMBER: {
            uint8_t dataj[] = { VALUE, bpmNumber};
            Firmata.sendSysex(ACCESSOR, 2, dataj);
        } break;
        case BPMSPEED: {
            uint8_t datak[] = { VALUE, bpmspeed};
            Firmata.sendSysex(ACCESSOR, 2, datak);
        } break;
    }
    break;
  }
}
/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

#ifdef FIRMATA_SERIAL_FEATURE
SerialFirmata serialFeature;
#endif

boolean isResetting = false;

// Forward declare a few functions to avoid compiler errors with older versions
// of the Arduino IDE.
void setPinModeCallback(byte, int);
void reportAnalogCallback(byte analogPin, int value);
void sysexCallback(byte, byte, byte*);

/*==============================================================================
 * SYSEX-BASED commands
 *============================================================================*/
bool enableLed = true;
void sysexCallback(byte command, byte argc, byte *argv) // command to id
{
  byte mode;
  byte stopTX;
  byte slaveAddress;
  byte data;
  int slaveRegister;
  unsigned int delayTime;

  switch (command) {
    case  MY_COMMAND: //0x01 - id
      Firmata.sendSysex(0x01, argc, argv);
      process_command1(argc, argv);
      break;
    case CONFIG:
       enableLed = false;
       break;
    case RUN:
      enableLed = true;
      break;
    case ACCESSOR:
    {
      uint8_t date[] = {argc};
      accessor_command(argc, argv);
      break;
    }
    case CAPABILITY_QUERY:
      Firmata.write(START_SYSEX);
      Firmata.write(CAPABILITY_RESPONSE);
      for (byte pin = 0; pin < TOTAL_PINS; pin++) {
#ifdef FIRMATA_SERIAL_FEATURE
        serialFeature.handleCapability(pin);
#endif
        Firmata.write(127);
      }
      
      Firmata.write(END_SYSEX);
      break;
    case ANALOG_MAPPING_QUERY:
      Firmata.write(START_SYSEX);
      Firmata.write(ANALOG_MAPPING_RESPONSE);
      for (byte pin = 0; pin < TOTAL_PINS; pin++) {
        Firmata.write(127);
      }
      Firmata.write(END_SYSEX);
      break;
    case SERIAL_MESSAGE:
#ifdef FIRMATA_SERIAL_FEATURE
      serialFeature.handleSysex(command, argc, argv);
#endif
      break;

  }
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void systemResetCallback()
{
  isResetting = true;

  // initialize a defalt state
  // TODO: option to load config from EEPROM instead of default

#ifdef FIRMATA_SERIAL_FEATURE
  serialFeature.reset();
#endif

  isResetting = false;
}
/*
void stringCallback(char *myString)
{
  char message[] = { 1, 2, 3, 4};
  Firmata.sendString(myString);
  
  Firmata.sendSysex(0x01, 4, message );
}
*/
void setup()
{
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);

  Firmata.attach(START_SYSEX, sysexCallback);
  Firmata.attach(SYSTEM_RESET, systemResetCallback);
//  Firmata.attach(STRING_DATA, stringCallback);

  Firmata.begin(57600);
  while (!Serial) {
    ; 
  }

  systemResetCallback(); 

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  //FastLED.show();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void updateFastLed()
{
  actTime = millis();
  if (actTime - remTime >= period) {
    //settingss[currentSettingsIndex].settings();
    //gHue++;
    remTime = actTime;
    is_looping = !is_looping; 
  }
  FastLED.show();
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}

void loop()
{
  
  //FastLED.show();  
  // insert a delay to keep the framerate modest
  //FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  
  
  /* STREAMREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while (Firmata.available())
    Firmata.processInput();

 patterns[currentPatternIndex].pattern();
 
 colors[currentColorIndex].color();
#ifdef FIRMATA_SERIAL_FEATURE
  serialFeature.update();
#endif
   if (enableLed) {
    updateFastLed();
   }
}
