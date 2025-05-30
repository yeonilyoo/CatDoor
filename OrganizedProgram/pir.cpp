/**
 * Program for PIRsensor. Detects movement and activates connected LED
 * for 2 seconds.
 * Connect on board pin number 07 to Sensor, 11 to LED 
 * - Refer RPiGPIOPin section of
 *   http://www.airspayce.com/mikem/bcm2835/group__constants.html
 */
#include "src/rpiServo.h"
#include "temp.h"
#include "facedetect.h"
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
using namespace std;



#define SENSOR RPI_GPIO_P1_11
//#define LED RPI_GPIO_P1_11
#define ACTIVE_VALUE 1

int main(int argc, char **argv)
{
	rpiServo servo;
	temp temperature;
	facedetect cat;
	if (!bcm2835_init()) {
	printf("Please run this with sudo\n");
	return EXIT_FAILURE;
	}

	bcm2835_gpio_fsel(SENSOR, BCM2835_GPIO_FSEL_INPT);//SENSOR as input
	//bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);	   //LED as output
	bcm2835_gpio_set_pud(SENSOR, BCM2835_GPIO_PUD_UP);
	uint8_t state = ACTIVE_VALUE; //Init state to HIGH
	while (1) {
		state = bcm2835_gpio_lev(SENSOR); //HIGH or LOW?
		//printf("State = %d\n", state);
		if(state != ACTIVE_VALUE)
		{
			//Sensor not active

		}
		else
		{
			printf("Temp: %f\n",(float)temperature.getTemp());
			printf("Movement at %d\n",(int)time(NULL));
			//bcm2835_gpio_set(LED);
		        system("raspistill -t 250 -vf -w 640 -h 480 -o counter -q 25");
			servo.setAngle(0); // set servo to centre
    			delay(500);
    			servo.setAngle(180);
    			delay(500);
			printf("End of cycle\n\n");
			delay(200);
	 		cat.catDetect();
		}
	}
	bcm2835_close();
	return EXIT_SUCCESS;
}
#undef SENSOR
//#undef LED
#undef ACTIVE_VALUE
