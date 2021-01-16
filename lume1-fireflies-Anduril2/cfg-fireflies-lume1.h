
/* Lume-Fireflies Driver config options
 * - Anduril 2 Firmware by Selene Scriven
 * - 6A Single Cell Constant Current Buck + FET driver, with 2A USB-C Charging
 * - Developed at request of Fireflies / FireflyLite Flashlight Manufacturer
 * 
 * For more information: 
 * - www.loneoceans.com/labs/
 * - Manual: http://toykeeper.net/torches/fsm/anduril2/anduril-manual.txt
 * - 1634 Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8303-8-bit-AVR-Microcontroller-tinyAVR-ATtiny1634_Datasheet.pdf
 * - Code Repo: https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/files/head:/ToyKeeper/spaghetti-monster/anduril
 */

// TODO: determine if "04" (Fireflylite) is the right prefix,
//       or if there should be a brand ID for Loneoceans
// For Fireflies / FireflyLite Lume1
// To be used in several flashlights (e.g. E12R, E07X...
//       but Jack from Fireflies isn't making it clear to me which ones...)
#define MODEL_NUMBER "0443"	

#include "hwdef-fireflies-lume1.h"
// ATTINY: 1634


//***************************************
//**       THERMAL SETTINGS            **
//***************************************

/* THERMAL MODIFICATONS

   Some modifications were made to enable the following scenario in Fireflies E12R.
   The FET has 3.9kHz PWM, and thermal gradual stepdown causes a whine.
   Change behavior so that thermal stepdown is a step response directly to 6A.
   Levels 1 to 149 are CC, 150 is turbo at 100% no PWM. 
   In addition, since body and sensor temperatures have a lag, add TURBO_TEMP_EXTRA.
   So only when temperature >temp_ceil+TURBO_TEMP_EXTRA && ramp is at 150, then drop.
   Otherwise, thermal regulation remains at user set temp_ceil (either default or in EEprom).

   Modified fsm-adc.c at line 422
            // send a warning
        >>  if ((temperature < (therm_ceil + TURBO_TEMP_EXTRA))&&(actual_level==150)){;}
        >>  else{emit(EV_temperature_high, howmuch);}
            //emit(EV_temperature_high, howmuch);

	And, modified ramp-mode.c at line 298
	        #ifdef USE_SET_LEVEL_GRADUALLY
            //set_level_gradually(stepdown);
        >>  if (actual_level == 150) {set_level(149);}
        >>  else {set_level_gradually(stepdown);}
*/

// set this light for 50C thermal ceiling
#undef DEFAULT_THERM_CEIL
#define DEFAULT_THERM_CEIL 50
//#define MAX_THERM_CEIL 70 // default maximum value

// drop from turbo at therm_ceil+TURBO_TEMP_EXTRA
#define TURBO_TEMP_EXTRA 7 // in celsius

// do not recalibrate light temperature to 21C upon factory reset!
// Lume driver has a calibrated temperature sensor already
// Not implemented yet, manually commented out (TODO)
//#define NO_TEMP_CAL_DURING_RESET

// optional, makes initial turbo step-down faster so first peak isn't as hot
// FET mode can run very very hot, so be extra careful
//#define THERM_HARD_TURBO_DROP

// stop panicking at ~3A (not sure of this numbers yet since it depends on the host..)
#define THERM_FASTER_LEVEL 116
#define MIN_THERM_STEPDOWN 69         // 760mA
#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting (default 24)
#define THERM_RESPONSE_MAGNITUDE 96   // bigger adjustments (default 64)

// respond to thermal changes faster
//#define THERMAL_WARNING_SECONDS 3
//#define THERMAL_UPDATE_SPEED 1
//#define THERM_PREDICTION_STRENGTH 4

// easier access to thermal config mode, similar to Emisar, Noctigon
#define USE_TENCLICK_THERMAL_CONFIG



//***************************************
//**      RAMPS AND OPERATIONS         **
//***************************************

// Uncomment to get Max Turbo on 2C from Off
//#define USE_2C_MAX_TURBO

// set this light to use stepped ramp by default (instead of smooth)
#undef RAMP_STYLE
#define RAMP_STYLE 1

// ../../bin/level_calc.py cube 1 149 7135 0 0.5 1000, with 0 appended to the end.
// for FET PWM (PWM2), all values are 0, except for last value of 1023
// (with max_pwm set to 1023)

// For FET version (length 150)
#define RAMP_LENGTH 150
#define PWM1_LEVELS 1,2,3,3,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,56,58,61,64,67,70,74,77,80,84,88,91,95,99,103,108,112,116,121,125,130,135,140,145,150,156,161,167,173,178,184,191,197,203,210,217,223,230,238,245,252,260,268,275,283,292,300,308,317,326,335,344,353,363,372,382,392,402,413,423,434,445,456,467,478,490,502,514,526,538,551,563,576,589,603,616,630,644,658,672,687,702,717,732,747,763,778,794,811,827,844,861,878,895,913,931,949,967,985,1004,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1023
//For no FET version (length 149)
//#define RAMP_LENGTH 149
//#define PWM1_LEVELS 1,2,3,3,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,12,13,14,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,56,58,61,64,67,70,74,77,80,84,88,91,95,99,103,108,112,116,121,125,130,135,140,145,150,156,161,167,173,178,184,191,197,203,210,217,223,230,238,245,252,260,268,275,283,292,300,308,317,326,335,344,353,363,372,382,392,402,413,423,434,445,456,467,478,490,502,514,526,538,551,563,576,589,603,616,630,644,658,672,687,702,717,732,747,763,778,794,811,827,844,861,878,895,913,931,949,967,985,1004,1023
//#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

#define DEFAULT_LEVEL 44  // About ~250mA
#define MAX_1x7135 69     // About ~760mA, MIN_THERM_STEPDOWN defined as MAX_1x7135 in ramp-mode.h if not otherwise defined

// TODO CHECK WHAT THESE DO
// TODO: test if underclocking works on lume1
//#define HALFSPEED_LEVEL 14
//#define QUARTERSPEED_LEVEL 5

// the entire ramp is regulated; don't blink halfway up
#ifdef BLINK_AT_RAMP_MIDDLE
#undef BLINK_AT_RAMP_MIDDLE
#endif

// don't slow down at low levels; this isn't that sort of light
// (it needs to stay at full speed for the 10-bit PWM to work)
#ifdef USE_DYNAMIC_UNDERCLOCKING
#undef USE_DYNAMIC_UNDERCLOCKING
#endif

//#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_FLOOR 3   // requested by Fireflies
#define RAMP_SMOOTH_CEIL 149  // Level 150 is when Buck is off and FET is ON 100%

#define RAMP_DISCRETE_FLOOR 3
#define RAMP_DISCRETE_CEIL RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 5

#define SIMPLE_UI_FLOOR 3
#define SIMPLE_UI_CEIL 149  // 139 for 5A max
#define SIMPLE_UI_STEPS 5

// Note: floor begins at 3; user can use 7H config menu to change 
// floor to 1 or 2 (for value 1 or 2 respectively) - requested by Fireflies

// Turn on Buck from level 1 to 149, but not 150
// Level 150 is when Buck is off and FET is ON 100%
#define LED_ENABLE_PIN_LEVEL_MIN 1
#define LED_ENABLE_PIN_LEVEL_MAX 149



//***************************************
//**       AUX LEDs and MISC           **
//***************************************

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS

// it has no independent LED in the button unlike emisar d4 for example
//#define USE_BUTTON_LED

// the aux LEDs are front-facing, so turn them off while main LEDs are on
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif
#define RGB_LED_OFF_DEFAULT 0x18  // low, voltage
#define RGB_LED_LOCKOUT_DEFAULT 0x37  // blinking, rainbow

// enable blinking aux LEDs
#define TICK_DURING_STANDBY
#define STANDBY_TICK_SPEED 3  // every 0.128 s
//#define STANDBY_TICK_SPEED 4  // every 0.256 s
//#define STANDBY_TICK_SPEED 5  // every 0.512 s

#define USE_SOS_MODE
#define USE_SOS_MODE_IN_BLINKY_GROUP

// uncomment to use Beacon Tower mode (after alpine beacon in 3C blinky modes)
//#define USE_BEACONTOWER_MODE

// party strobe on-time
#define PARTY_STROBE_ONTIME 4

//#define THERM_CAL_OFFSET 0	// was 5 from Noctigon

// attiny1634 has enough space to smooth out voltage readings
#define USE_VOLTAGE_LOWPASS

// can't reset the normal way because power is connected before the button
#define USE_SOFT_FACTORY_RESET