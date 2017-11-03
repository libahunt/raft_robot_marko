/*Include this file if using 
 * ultrasound distance sensors and 
 * Atmega328 controller. */

/*** Pin connections ***/
#define TRIGGER_PIN_FIRST  3
#define ECHO_PIN_FIRST     4
#define MAX_DISTANCE_FIRST 300

#define TRIGGER_PIN_SECOND  7
#define ECHO_PIN_SECOND     8
#define MAX_DISTANCE_SECOND 300

const int radarServo = 11;

const int motor[] = {5, 6, 9, 10};

const int runToggleButton = 2;


/*** Library object initializations. ***/

NewPing firstSonar(TRIGGER_PIN_FIRST, ECHO_PIN_FIRST, MAX_DISTANCE_FIRST);
NewPing secondSonar(TRIGGER_PIN_SECOND, ECHO_PIN_SECOND, MAX_DISTANCE_SECOND);
