/** @brief Gh control functions
 *  @file ghcontrol.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "ghcontrol.h"
#include "sensehat.h"

SenseHat Sh;

// Alarm Message Array
const char alarmnames[NALARMS][ALARMNMSZ] =
        {"No Alarms", "HighTemperature", "LowTemperature", "High Humidity",
        "Low Humidity", "HighPressure", "Low Pressure"};


/**  @brief Prints Gh Controller Title
 *   @version 24JANUARY2022
 *   @author Sahil Kathiriya
 *   @param const char * sname
 *   @return void
*/

void GhDisplayHeader(const char * sname)
{
	fprintf(stdout,"%s's CENG153 Greenhouse Controller\n",sname);
}

/**  @brief get serial number
 *   @version 24JANUARY2022
 *   @param void
 *   @return unit64_t
*/
uint64_t GhGetSerial(void)
{
	static uint64_t serial = 0;
	FILE * fp;
	char buf[SYSINFOBUFSZ];
	char searchstring[] = SEARCHSTR;
	fp = fopen ("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
		while (fgets(buf, sizeof(buf), fp) != NULL)
		{
			if (!strncasecmp(searchstring, buf, strlen(searchstring)))
			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
     if(serial==0)
     {
         system("uname -a");
         system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
         fp = fopen ("stamp.txt", "r");
         if (fp != NULL)
         {
             while (fgets(buf, sizeof(buf), fp) != NULL)
             {
                sscanf(buf, "%Lx", &serial);
             }
             fclose(fp);
        }
     }
	return serial;
}

/**  @brief genrate random number
 *   @version 24JANUARY2022
 *   @author Sahil Kathiriya
 *   @param int range
 *   @return int
*/

int GhGetRandom(int range)
{
	return rand() % range;
}

/**  @brief delay function for delay time between running loop
 *   @version 24JANUARY2022
 *   @author Sahil Kathiriya
 *   @param int milliseconds
 *   @return void
*/

void GhDelay(int milliseconds)
{
	long wait;
	clock_t now,start;

	wait = milliseconds*(CLOCKS_PER_SEC/1000);
	start = clock();
	now = start;
	while( (now-start) < wait )
	{
		now = clock();
	}
}

/**  @brief run GhDisplayHeader and GhSetTargets function inside it
 *   @version 24JANUARY2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return void
*/

void GhControllerInit(void)
{

	srand((unsigned) time(NULL));
	GhDisplayHeader("Sahil Kathiriya");
	GhSetTargets();
}

/**  @brief Provide Gh Controller array data for temperature, humidity, pressure
 *   @version 18FEBRUARY2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return reading_s
*/

reading_s GhGetReadings(void)
{
    reading_s now = {0};

    now.rtime = time(NULL);
	now.temperature = GhGetTemperature();
	now.humidity = GhGetHumidity();
	now.pressure = GhGetPressure();
	return now;
}

/**  @brief Displays Gh Controller temperarure, humidity, pressure readings
 *   @version 18FEBRUARY2022
 *   @author Sahil Kathiriya
 *   @param reading_s rdata
 *   @return void
*/

void GhDisplayReadings(reading_s rdata)
{
	fprintf(stdout,"\n%sReadings\tT:%5.1fC\tH:%5.1f%\tP:%6.1fmb",
		ctime(&rdata.rtime),
		rdata.temperature, rdata.humidity,
        rdata.pressure);
}

/**  @brief Set Gh Controller heater and humidifier conditions
 *   @version 18FEBRUARY2022
 *   @author Sahil Kathiriya
 *   @param setpoint_s target
 *   @param reading_s rdata
 *   @return control_s
*/

control_s GhSetControls(setpoint_s target, reading_s rdata)
{
    control_s cset;

	if (rdata.temperature < target.temperature)
	{
		cset.heater = ON;
	}
	else
	{
		cset.heater = OFF;
	}
	if (rdata.humidity < target.humidity)
	{
		cset.humidifier = ON;
	}
	else
	{
		cset.humidifier = OFF;
	}
	return cset;
}

/**  @brief set STEMP and SHUMID
 *   @version 14MARCH2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return setpoint_s
*/

setpoint_s GhSetTargets(void)
{
    setpoint_s cpoints = {0};

    cpoints = GhRetrieveSetpoints("setpoints.dat");

    if(cpoints.temperature == 0)
    {
        cpoints.temperature = STEMP;
        cpoints.humidity = SHUMID;
        GhSaveSetpoints("setpoints.dat", cpoints);
    }

    return cpoints;
}

/**  @brief Displays Gh Controller target data
 *   @version 14MARCH2022
 *   @author Sahil Kathiriya
 *   @param setpoint_s spts
 *   @return void
*/

void GhDisplayTargets(setpoint_s spts)
{
	fprintf(stdout,"\nSetpoints\tT:%5.1fc\tH:%5.1f%\n", spts.temperature, spts.humidity);
}


/**  @brief Displays Gh Controller heater and humidifier
 *   @version 18FEBRUARY2022
 *   @author Sahil Kathiriya
 *   @param control_s ctrl
 *   @return void
*/

void GhDisplayControls(control_s ctrl)
{
	fprintf(stdout,"Controls\tHeater: %ld\tHumidifier: %ld\n",ctrl.heater, ctrl.humidifier);
}

/**  @brief Provide Gh Controller Humidity
 *   @version 25MARCH2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return float
*/

float GhGetHumidity(void)
{
#if SIMHUMIDITY
	return GhGetRandom(USHUMID-LSHUMID)+LSHUMID;
#else
	return Sh.GetHumidity();
#endif
}

/**  @brief Provide Gh Controller Pressure
 *   @version 25MARCH2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return float
*/

float GhGetPressure(void)
{
#if SIMPRESSURE
	return GhGetRandom(USPRESS-LSPRESS)+LSPRESS;
#else
	return Sh.GetPressure();
#endif
}

/**  @brief Provide Gh Controller Temperature
 *   @version 25MARCH2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return float
*/

float GhGetTemperature(void)
{
#if SIMTEMPERATURE
	return GhGetRandom(USTEMP-LSTEMP)+LSTEMP;
#else
	return Sh.GetTemperature();
#endif
}

/**  @brief Provide Gh Logdata
 *   @version 14March2022
 *   @author Sahil Kathiriya
 *   @param char * fname
 *   @param reading_s ghdata
 *   @return int
*/

int GhLogData(char * fname, reading_s ghdata)
{
    FILE *fp;
    char ltime[CTIMESTRSZ];

    fp = fopen(fname,"a");
    if(fp == NULL)
    {
        fprintf(stdout,"\nCan't open file, data not retrieved!\n");
        return 0;
    }
        strcpy(ltime, ctime(&ghdata.rtime));
        ltime[3]=',';
        ltime[7]=',';
        ltime[10]=',';
        ltime[19]=',';

        fprintf(fp,"\n%.24s,%5.1f,%5.1f,%6.1f",
        ltime,ghdata.temperature,ghdata.humidity,
        ghdata.pressure);

    fclose(fp);
    return 1;
}

/**  @brief Save setpoints data
 *   @version 14March2022
 *   @author Sahil Kathiriya
 *   @param char * fname
 *   @param setpoint_s spts
 *   @return int
*/

int GhSaveSetpoints(const char * fname, setpoint_s spts)
{
    FILE *fp;

    fp = fopen(fname,"w");
    if(fp == NULL)
    {
        fprintf(stdout,"\nCan't open file, data not retrieved!\n");
        return 0;
    }

    fwrite(&spts, sizeof(setpoints), 1 , fp);
    fclose(fp);

    return 1;
}


/**  @brief Retrieve Setpoints data
 *   @version 14March2022
 *   @author Sahil Kathiriya
 *   @param char * fname
 *   @return setpoint_s
*/

setpoint_s GhRetrieveSetpoints(const char * fname)
{
    setpoint_s spts = {0.0};

    FILE *fp;
    fp = fopen(fname, "r");
    if(fp == NULL)
    {
        return spts;
    }
    fread(&spts, sizeof(setpoints), 1, fp);
    fclose(fp);

    return spts;
}
/**  @brief lightup vartical bar
 *   @version 21March2022
 *   @author Sahil Kathiriya
 *   @param int bar
 *   @param COLOR_SENSEHAT pxc
 *   @param uint8_t value
 *   @return int
*/

int ShSetVerticalBar(int bar,COLOR_SENSEHAT pxc, uint8_t value)
{
    if(value > 7)
    {
        value = 7;
    }
    if(bar > 7)
    {
        bar = 7;
    }
    if(bar > 7 || value > 7)
    {
        return EXIT_FAILURE;
    }

    for(int i = 0; i < value; i++)
    {
        Sh.LightPixel(i, bar, pxc);
    }

    for(int i = value+1; i <= 7; i++)
    {
        Sh.LightPixel(i, bar, BLACK);
    }

    return EXIT_SUCCESS;

}

/**  @brief display the environmental values with a combination of bar position and point colour
 *   @version 20March2022
 *   @author Sahil Kathiriya
 *   @param reading_s rd
 *   @param setpoint_s sd
 *   @return void
*/

void GhDisplayAll(reading_s rd, setpoint_s sd)
{
    int rv = 0, sv = 0, avh = 0, avl = 0;
    COLOR_SENSEHAT pxc;
    Sh.WipeScreen(BLACK);

    rv = (8.0*(((rd.temperature-LSTEMP)/(USTEMP-LSTEMP))+0.05))-1.0;
    ShSetVerticalBar(TBAR, GREEN, rv);

    rv = (8.0*(((rd.humidity-LSHUMID)/(USHUMID-LSHUMID))+0.05))-1.0;
    ShSetVerticalBar(HBAR, GREEN, rv);

    rv = (8.0*(((rd.pressure-LSPRESS)/(USPRESS-LSPRESS))+0.05))-1.0;
    ShSetVerticalBar(PBAR, GREEN, rv);

    sv = (8.0*(((sd.temperature-LSTEMP)/(USTEMP-LSTEMP))+0.05))-1.0;
    Sh.LightPixel(sv, TBAR, MAGENTA);

    sv = (8.0*(((sd.humidity-LSHUMID)/(USHUMID-LSHUMID))+0.05))-1.0;
    Sh.LightPixel(sv, HBAR, MAGENTA);
}
/**  @brief set alarm alert limits
 *   @version 2APRIL2022
 *   @author Sahil Kathiriya
 *   @param void
 *   @return alarmlmit_s
*/

alarmlimit_s GhSetAlarmLimits(void)
{
    alarmlimit_s calarm = {0};

    calarm.hight = UPPERATEMP;
    calarm.lowt = LOWERATEMP;
    calarm.highh = UPPERAHUMID;
    calarm.lowh = LOWERAHUMID;
    calarm.highp = UPPERAPRESS;
    calarm.lowp = LOWERAPRESS;

    return calarm;
}
/**  @brief compare the readings with set data
 *   @version 10APRIL2022
 *   @author Sahil Kathiriya
 *   @param alarm_s * head
 *   @param alarmlimit_s alarmpt
 *   @param reading_s rdata
 *   @return alarm_s
*/
alarm_s * GhSetAlarms(alarm_s * head, alarmlimit_s alarmpt, reading_s rdata)
{

    if(rdata.temperature >= alarmpt.hight)
    {
        GhSetOneAlarm(HTEMP,rdata.rtime,rdata.temperature,head);
    }
    else
    {
        head = GhClearOneAlarm(HTEMP,head);
    }

    if(rdata.temperature <= alarmpt.lowt)
    {
        GhSetOneAlarm(LTEMP,rdata.rtime,rdata.temperature,head);
    }
    else
    {
        head = GhClearOneAlarm(LTEMP,head);
    }

    if(rdata.humidity >= alarmpt.highh)
    {
        GhSetOneAlarm(HHUMID,rdata.rtime,rdata.humidity,head);
    }
    else
    {
        head = GhClearOneAlarm(HHUMID,head);
    }

    if(rdata.humidity <= alarmpt.lowh)
    {
        GhSetOneAlarm(LHUMID,rdata.rtime,rdata.humidity,head);
    }
    else
    {
        head = GhClearOneAlarm(LHUMID, head);
    }

    if(rdata.pressure >= alarmpt.highp)
    {
        GhSetOneAlarm(HPRESS,rdata.rtime,rdata.pressure,head);
    }
    else
    {
        head = GhClearOneAlarm(HPRESS,head);
    }

    if(rdata.pressure <= alarmpt.lowp)
    {
        GhSetOneAlarm(LPRESS,rdata.rtime,rdata.pressure,head);
    }
    else
    {
        head = GhClearOneAlarm(HTEMP,head);
    }
    return head;
}

/**  @brief Display alarms alert
 *   @version 10APRIL2022
 *   @author Sahil Kathiriya
 *   @param alarm_s * head
 *   @return void
*/

void GhDisplayAlarms(alarm_s * head)
{
    alarm_s * cur;
    cur = head;

    fprintf(stdout,"\nAlarms\n");
    while(cur != NULL)
    {
        if(cur->code != NOALARM)
        {
            fprintf(stdout,"%s alarm %s", alarmnames[cur->code],ctime(&(cur->atime)));

        }
        cur = cur->next;
    }
}
/**  @brief Set alarm alert
 *   @version 10APRIL2022
 *   @author Sahil Kathiriya
 *   @param alarm_e code
 *   @param time_t atime
 *   @param double value
 *   @param alarm_s * head
 *   @return int
*/
int GhSetOneAlarm(alarm_e code, time_t atime, double value, alarm_s * head)
{
    alarm_s * last;
    alarm_s * cur;

    cur = head;
    if(cur->code != 0)
    {
        while(cur !=  NULL)
        {
            if(cur->code == code)
            {
                return 0;
            }
            last = cur;
            cur = cur->next;
        }
        cur = (alarm_s *)calloc(1, sizeof(alarm_s));

        if(cur == NULL)
        {
            return 0;
        }
        last->next = cur;
    }
    cur->code = code;
    cur->atime = atime;
    cur->value = value;
    cur->next = NULL;
    return 1;
}
/**  @brief clear alarms in list
 *   @version 10APRIL2022
 *   @author Sahil Kathiriya
 *   @param alarm_e code
 *   @param alarm_s * head
 *   @return alarm_s
*/
alarm_s * GhClearOneAlarm(alarm_e code, alarm_s * head)
{
    alarm_s * cur;
    alarm_s * last;
    cur = head;
    last = head;

    if(cur->code == code && cur->next == NULL)
    {
        cur->code = NOALARM;
        return head;
    }
    if(cur->code == code && cur->next != NULL)
    {
        head = cur->next;
        free(cur);
        return head;
    }

    while(cur != NULL)
    {
        if(cur->code == code)
        {
            last->next = cur->next;
            free(cur);
            return head;
        }
        last  = cur;
        cur = cur->next;
    }
    return head;
}


