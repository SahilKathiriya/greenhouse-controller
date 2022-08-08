/** @brief Defines the entry point for the console application.
 *  @file ghc.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "ghcontrol.h"
/**  @brief call Gh Controller all functions
 *   @version 10APRIL2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return int
*/
int main(void)
{
    int logged;
	reading_s creadings = {0};
    control_s ctrl = {0};
	setpoint_s sets = {0};
	alarmlimit_s alimits = {0};

    alarm_s * arecord;

   arecord = (alarm_s *) calloc(1,sizeof(alarm_s));
   if(arecord == NULL)
   {
        printf("\nCannot allocate memory\n");
        return EXIT_FAILURE;
   }

    GhControllerInit();
    sets = GhSetTargets();
    alimits = GhSetAlarmLimits();

	while(1)
	{
		creadings = GhGetReadings();
        logged = GhLogData("ghdata.txt", creadings);
        ctrl = GhSetControls(sets, creadings);
        GhSetAlarms(arecord, alimits, creadings);
		GhDisplayReadings(creadings);
		GhDisplayTargets(sets);
		GhDisplayControls(ctrl);
		GhDisplayAlarms(arecord);
		GhDisplayAll(creadings, sets);
		GhDelay(GHUPDATE);
	}

	return EXIT_SUCCESS;
}


