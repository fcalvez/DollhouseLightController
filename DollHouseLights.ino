/* Illumination system for Dollhouse
 *  
 *  Using Adafruit Flora 
 *  - Small piezzo buzzer on D12(A11) as an input button
 *  - Leds with 220Hom resistor on pins D10, D9, D6, D1, D0, D2, D3
 *  
 *  Detect touch on the buzzer, and count the number of consecutive touch without pausing more than 500ms
 *  Turn the corresponding LED on/off
 *  
 *  Every led is on at startup
*/

#include <Adafruit_NeoPixel.h>

//#define DEBUGON

#define ALED0      PORTE &= ~(1<<6)
#define ALED1      PORTE |= (1<<6)

#define PIXELPIN 10
#define VECTLEN 200
#define LEDCOUNT 7
#define SENSIBILITY 100

int ledpins[] = {10,9,6,1,0,2,3};
int ledon[LEDCOUNT];

// input button samples rotating buffer
int vect[VECTLEN];
int vectx = 0; // index in buffer

void setup() {
  pinMode(A11, INPUT);
  
  #ifdef DEBUGON
    Serial.begin(115200);
  #endif

  for(int i=0; i<VECTLEN; i++) {
    vect[i] = 0;
  }

  for(int i=0; i<LEDCOUNT; i++)
  {
    pinMode(ledpins[i],OUTPUT);  
    ledon[i] = HIGH;
  }

  updateLeds();
}

int moy = 0;
int lastmoy = 0;
int delta = 0;
int lastdelta = 0;
int spd = 0;
int absspd = 0;

unsigned long t0;
int nbClick = 0;
bool btDown = false;
bool btClick = false;
unsigned long timeClick;
unsigned long timeReset=500;

  // TXLED1;
  // RXLED1;
  // ALED1;

void updateLeds()
{
    // update leds
    for(int i=0; i<LEDCOUNT; i++)
    {
      digitalWrite(ledpins[i], ledon[i]);
    }
}

void loop() {

  // fill half the data buffer size
  for(int i=VECTLEN/2; i >=0; i--) {
    // read button input
    unsigned int v = analogRead(A11);
    vect[vectx] = v;
    vectx = (vectx + 1) % VECTLEN;
  }

  // compute average of entire buffer
  long tot = 0;
  for(int i=0; i<VECTLEN; i++) {
    tot+=vect[i];
  }
  lastmoy = moy;    
  moy = tot / VECTLEN;

  // compute acceleration
  lastdelta = delta;
  delta = lastmoy-moy;
  spd = lastdelta-delta;
  absspd = spd;
  if(spd < 0) absspd = -spd;

  // detect click
  if(spd < 0 && absspd > SENSIBILITY) {
    btDown = true;
  }
  if(spd > 0 && absspd > SENSIBILITY) {
    if(btDown)
      btClick = true;
    btDown = false;
  }

  // count clicks
  if(btClick) {
    TXLED1;
    nbClick++;

#ifdef DEBUGON
    Serial.print("!! CLICK "); 
    Serial.println(nbClick);
#endif

    timeClick = millis();
    btClick = false;
  } else {
    TXLED0;
  }

  if(nbClick > 0)
    ALED1;

  // reset click count after a small time without click
  if(nbClick > 0 && millis() - timeClick > timeReset)
  {
    int ledNum = nbClick-1;
    // switch led on/off status
    if(ledNum < LEDCOUNT){
      
#ifdef DEBUGON
      Serial.print("> switching LED ");
      Serial.println(ledNum);
#endif

      if(ledon[ledNum] == HIGH)
        ledon[ledNum] = LOW;
      else
        ledon[ledNum] = HIGH;
    }
    
#ifdef DEBUGON
    Serial.println("..reset.."); 
#endif

    nbClick = 0;  
    btClick = false;
    ALED0;    

    updateLeds();
  }
    
    //pixels.setPixelColor(0, color, color ,color );
    //pixels.show();   // Send the updated pixel colors to the hardware.
  
}
