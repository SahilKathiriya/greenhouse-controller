/** @brief Gh constants, structures, function prototypes
 *  @file ghcontrol.h
 */
#ifndef GHCONTROL_H
#define GHCONTROL_H

//Includes
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "sensehat.h"

// Constants
#define SEARCHSTR "serial\t\t:"
#define SYSINFOBUFSZ 512
#define GHUPDATE 2000
#define SENSORS 3
#define TEMPERATURE 0
#define HUMIDITY 1
#define PRESSURE 2
#define ON 1
#define OFF 0
#define CTIMESTRSZ 25
// Simulate Constants
#define SIMULATE 1
#define SIMTEMPERATURE 0
#define SIMHUMIDITY 0
#define SIMPRESSURE 0
#define USTEMP 50
#define LSTEMP -10
#define USHUMID 100
#define LSHUMID 0
#define USPRESS 1016
#define LSPRESS 975
// Targets Constants
#define STEMP 25.0
#define SHUMID 55.0
//sensehat Constants
#define SENSEHAT 1
#define NUMBARS 8
#define NUMPTS 8.0
#define TBAR 7
#define HBAR 5
#define PBAR 3
//Alarms Constants
#define NALARMS 7
#define LOWERATEMP 10
#define UPPERATEMP 30
#define LOWERAHUMID 25
#define UPPERAHUMID 70
#define LOWERAPRESS 985
#define UPPERAPRESS 1016
#define ALARMNMSZ 18

// Structers
typedef struct readings
{
	time_t rtime;
	float temperature;
	float humidity;
	float pressure;
}reading_s;

typedef struct setpoints
{
    float temperature;
    float humidity;
}setpoint_s;

typedef struct controls
{
    int heater;
    int humidifier;
}control_s;

typedef enum { NOALARM, HTEMP, LTEMP, HHUMID, LHUMID, HPRESS, LPRESS } alarm_e;

typedef struct alarmlimts
{
    float hight;
    float lowt;
    float highh;
    float lowh;
    float highp;
    float lowp;
}alarmlimit_s;

typedef struct alarms
{
    alarm_e code;
    time_t atime;
    float value;
    struct alarms * next;
}alarm_s;


// Function Prototypes
///@cond INTERNAL
void GhDisplayHeader(const char * sname);
reading_s GhGetReadings(void);
void GhDisplayReadings(reading_s rdata);
setpoint_s GhSetTargets(void);
void GhDisplayTargets(struct setpoints spts);
control_s GhSetControls(struct setpoints target, reading_s rdata);
float GhGetHumidity(void);
float GhGetPressure(void);
float GhGetTemperature(void);
void GhDisplayControls(control_s ctrl);
uint64_t GhGetSerial(void);
int GhGetRandom(int range);
void GhDelay(int milliseconds);
void GhControllerInit(void);
int GhLogData(char * fname, reading_s ghdata);
int GhSaveSetpoints(const char * fname, struct setpoints spts);
setpoint_s GhRetrieveSetpoints(const char * fname);
int ShSetVerticalBar(int bar,COLOR_SENSEHAT pxc, uint8_t value);
alarmlimit_s GhSetAlarmLimits(void);
void GhDisplayAlarms(alarm_s alrm[NALARMS]);
void GhDisplayAll(reading_s rd, struct setpoints sd);
alarm_s * GhSetAlarms(alarm_s * head, alarmlimit_s alarmpt, reading_s rdata);
void GhDisplayAlarms(alarm_s * head);
int GhSetOneAlarm(alarm_e code,time_t atime,double value,alarm_s * head);
alarm_s * GhClearOneAlarm(alarm_e code,alarm_s * head);


///@endcond

#endif
