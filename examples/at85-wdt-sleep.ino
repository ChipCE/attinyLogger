/*
 * Sketch for testing sleep mode with wake up on WDT.
 * Donal Morrissey - 2011.
 * ATTINY85
 */

#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
#include <LiquidCrystal_attiny.h>          // for LCD w/ GPIO MODIFIED for the ATtiny85


#include <avr/sleep.h> 
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

//uwaga na numer pinu! nie używać tego z resetem
#define OUT_PIN 3

#define GPIO_ADDR     0x27
LiquidCrystal_I2C lcd(GPIO_ADDR, 16, 2); // set address & 16 chars / 2 lines

volatile int f_wdt=1;


/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect){
  if(f_wdt == 0){
    f_wdt=1;

  }
  else{
//      lcd.clear();
//      lcd.print("WDT overrun");
//      delay(500);
  }
}


/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void enterSleep(void)
{
  lcd.clear();
  lcd.print("Asleep...");

  /* Enable the WD interrupt (note no reset). */
  // WDTCR |= (1<<WDIE); 

  set_sleep_mode( SLEEP_MODE_PWR_DOWN );   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();

  /* Now enter sleep mode. */
  sleep_mode();

  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */

  /* Re-enable the peripherals. */
  power_all_enable();

  /* Disable the WD interrupt */
  WDTCR &= ~_BV(WDIE);
}



/***************************************************
 *  Name:        setup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Setup for the serial comms and the
 *                Watch dog timeout. 
 *
 ***************************************************/
void setup()
{
  /*  Setup the LCD */
  lcd.init();                           // initialize the lcd
  lcd.backlight();                      // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Initializing...");
  delay(500);

  /*** Setup the WDT ***/

  /* Clear the reset flag of watchdog interrupts, so that they are available again. */
  MCUSR &= ~(1<<WDRF);

  /* Enable configuration changes
   */
  WDTCR |= (1<<WDCE) | (1<<WDE);

/*
WDP3 WDP2 WDP1 WDP0         Number of WDT     Typical Time-out at
                                             Oscillator Cycles     VCC = 5.0V

  0         0      0        0             2K (2048) cycles       16 ms
  0         0      0        1             4K (4096) cycles       32 ms
  0         0      1        0             8K (8192) cycles       64 ms
  0         0      1        1            16K (16384) cycles    0.125 s
  0         1      0        0            32K (32768) cycles    0.25 s
  0         1      0        1            64K (65536) cycles    0.5 s
  0         1      1        0            128K (131072) cycles 1.0 s
  0         1      1        1            256K (262144) cycles 2.0 s
  1         0      0        0            512K (524288) cycles 4.0 s
  1         0      0        1            1024K (1048576) cycles 8.0 s
*/
  /* set new watchdog timeout prescaler value */
  WDTCR =  0<<WDP3 | 1<<WDP2 | 0<<WDP1 |  1<<WDP0 ;

  /* Enable the WD interrupt (note no reset). */
  WDTCR |= (1<<WDIE); 

  lcd.clear();
  lcd.print("Initialisation complete.");
  delay(100); 
}



/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main application loop.
 *
 ***************************************************/
void loop()
{
  if(f_wdt == 1)
  {
//    lcd.clear();
//    lcd.print("Awake!");
    digitalWrite( OUT_PIN, HIGH );
    delay(1000);
    /* Don't forget to clear the flag. */
    f_wdt = 0;
    digitalWrite( OUT_PIN, LOW );
    /* Re-enter sleep mode. */
    enterSleep();
  }
  else
  {
    /* Do nothing. */
  }
}