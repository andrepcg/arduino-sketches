#include <MANCHESTER.h>
#include <ATTinyWatchdog.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

/*
       ATTiny85:
                   u
         Reset (1)    (8) VCC
TxPwr  P3 (A3) (2)    (7) P2 (A1/INT0)  sensorPin
TxPin  P4 (A2) (3)    (6) P1 (PWM) battpin
           GND (4)    (5) P0 (PWM)

*/

#define sensorPin 2
#define battPin 1
#define TxPwr 3
#define TxPin 4

#define inputVoltagem 4.5

unsigned int led_counts = 0;

void setup(){
  analogReference(INTERNAL);
  
  pinMode(sensorPin, INPUT);
  pinMode(battPin, INPUT);
  // pino nao usado = input
  pinMode(0, INPUT);
  
  power_timer0_disable();
  power_usi_disable();

  MANCHESTER.SetTxPin(TxPin); 
  ATTINYWATCHDOG.setup(8);
  
  attachInterrupt(sensorPin,interruptFunc,FALLING);
  
  MCUCR |= _BV(ISC01); // falling edge
  GIMSK |= _BV(INT0);   // enable INT0 interrupt 
  sei();
}

void loop(){
  acordar();
  delay(20);
  
  enviar();
  delay(20);
  
  // verificar voltagem
  
  // dormir 10 minutos
  dormir(10);
}

void acordar(){
  pinMode(TxPin, OUTPUT);
  pinMode(TxPwr, OUTPUT);
}

void enviar(){
  // bloquear interrupts
  uint8_t oldSREG = SREG;
  cli();
  
  MANCHESTER.Transmit(led_counts);

  // restaurar interrupts  
  SREG = oldSREG;
}

void dormir(int minutos){
  pinMode(TxPin, INPUT);
  pinMode(TxPwr, INPUT);
  ATTINYWATCHDOG.sleep((minutos*60)/4);
  acordar();
}

void interruptFunc(){
  led_counts++;
}

ISR(PCINT0_vect){ 
  led_counts++;
}

float voltagem(){
  int sensor = analogRead(battPin);
  return float(sensor)/1023*float(inputVoltagem);
}
