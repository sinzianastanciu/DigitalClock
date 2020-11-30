#include <util/delay.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

LiquidCrystal lcd(12, NULL, 11, 9,8,7,6);

//ora si data calendaristica
int yyyy, m, d, c, leap, yy, cTable, mTable, SummedDate, DoW;
volatile int secunda = 40;
volatile int minut = 59;
volatile int ora = 23;
volatile int zi = 1;
volatile int luna = 12;
volatile int an = 2020;
int ziua_sapt;
unsigned t;
volatile int hr_12 = 0;
volatile int zile_luna [] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char *zile_sapt[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat","Sun"};
char *ziua_curenta = zile_sapt[1];
volatile int nr_zi_sapt = 1;
const int backlight = 13;
volatile boolean backlight_on;
volatile byte set_mode ;

//temperatura
const int pinTemp = 0;
float temperatura = 0;

//buzzer
const int speakerPin = 5;

//neopixel
const int pinLed = 10;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4, pinLed, NEO_GRB + NEO_KHZ800);

void setup() {
  

  TIMSK1 = (1 << TOIE1); // activare timer overflow
  //TCNT1=0x0BDC;
  TCCR1A = 0; 
  TCCR1B = (1 << CS12); // timer start

  attachInterrupt(0,ISR_b1,FALLING);
  attachInterrupt(1,ISR_b2,FALLING);
  //activare PCI2 (buton 3 - PCINT20 - PCI2)

  PCICR  |= (1 << PCIE2);
  PCMSK2 |= (1<<PCINT20);
  
  //la pornire lumina de fundal a afisajului este oprita
 	 pinMode(backlight, OUTPUT);
  	 digitalWrite(backlight, LOW);
 	 backlight_on = false;
  
  //setare mod functionare normala
  set_mode = 0;
  
  
  pinMode(speakerPin, OUTPUT);
  pixels.begin();
  
  //configurare LCD
  lcd.begin(16, 2);
  lcd.noCursor();
}

void loop(){

   lcd.setCursor(0, 0);
   if (zi<10) lcd.print("0");
   lcd.print(zi);
   lcd.setCursor(2,0);
   lcd.print("/");
   if (luna<10) lcd.print("0");
   lcd.print(luna);
   lcd.setCursor(5,0);
   lcd.print("/");
   lcd.print(an);
  
   if(set_mode == 0){
   lcd.setCursor(0, 1);
   if (ora<10) lcd.print("0");
   lcd.print(ora);
   lcd.setCursor(2,1);
   lcd.print(":");
	}
   lcd.setCursor(2,1);
   lcd.print(":");
   if (minut<10) lcd.print("0");
   lcd.print(minut);
   lcd.setCursor(5,1);
   lcd.print(":");
   if (secunda<10) lcd.print("0");
   lcd.print(secunda); 
   lcd.setCursor(11,1);
   lcd.print(ziua_curenta);

  	//selectare modificare 
  if(set_mode != 0){
     lcd.setCursor(15, 1);
     switch (set_mode) {
       case 1:
         lcd.print("A"); //mod fara am/pm
         break;
       case 2:
         lcd.print("B"); //mod cu am/pm
         break;
      
     } 
   }
  
  //Afisare AM/PM
  
  if(set_mode !=0)
  {
    switch(set_mode){
      case 1:
    	lcd.setCursor(0, 1);
   		if (ora<10) lcd.print("0");
   		lcd.print(ora);
    	break;
      case 2:
        if(ora == 0){
      hr_12 = 12;
      lcd.setCursor(0,1);
      lcd.print(hr_12);
      lcd.setCursor(8,1);
      lcd.print("AM");
    }
    else if (ora > 12){
	  hr_12 = ora-12;
      lcd.setCursor(0,1);  
      if (hr_12<10)
      lcd.print("0");
      lcd.print(hr_12);
      lcd.setCursor(8,1);
      lcd.print("PM");
    }
    else if(ora == 12) {
      lcd.setCursor(8,1);
      lcd.print("PM");
    }
    else{
      lcd.setCursor(8,1);
      lcd.print("AM");
    }
    break;
  
  }
  
    	
 }
  else if(set_mode==0)
  {
    
    lcd.setCursor(8,1);
    lcd.print("  ");
    lcd.setCursor(15,1);
    lcd.print(" ");
  }
 
  //Afisarea Temperaturii
  lcd.print(" ");
  lcd.setCursor(11, 0);
   temperatura = GetTemp();
   lcd.print(temperatura);

  delay(900);
}


ISR(TIMER1_OVF_vect) {
	TCNT1=0x0BDC;
 	secunda++;
 if (secunda == 60)
 { 
   secunda = 0;
   minut++;
 	if (minut>=60)
   {
     minut = 0;
     ora ++;
     if (ora >= 24)
   {  
     ora = 0;
     zi++;
     nr_zi_sapt++;
       if(nr_zi_sapt>=7)
       	 nr_zi_sapt = 0;
        
     ziua_curenta = zile_sapt[nr_zi_sapt];
     if (zi > zile_luna[luna-1])
     {
         zi = 1;
         luna++;
       if (luna > 12)
       {
         luna = 1;
         an++;
         if (an%4 == 0) zile_luna[1] = 29;
         else zile_luna[1] = 28;
 		}
 		}
 	}
 	}
 }
  
}

void ISR_b1() {
  TIMSK1 = (1 << TOIE1); // activare timer overflow
  TCCR1A = 0; 
  TCCR1B = (1 << CS12); // timer start
	pixels.clear();
  _delay_ms(600);
}

void ISR_b2() 
{

  set_mode++;
  if (set_mode==3) set_mode=0;

  _delay_ms(600);
}

ISR(PCINT2_vect) {

    TIMSK1 = 0;
    TCCR1A = 0; 
    TCCR1B = 0; // timer stop
  	tone(speakerPin,700,500);
  	delay(200);
  	digitalWrite(speakerPin,LOW);
  	for(int i=0;i<4;i++){
          pixels.setPixelColor(i, pixels.Color(200, 100, 200));
          pixels.show(); 
          
        }
  _delay_ms(600);
 }



float GetTemp(void)
{
  float rezultat;
  
  rezultat = (double)analogRead(A0)/1024;
  Serial.print(rezultat);
  rezultat = (rezultat*5-0.5)*100;
  return rezultat;

}