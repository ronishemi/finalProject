#ifndef CONTROLER_H_
#define CONTROLER_H_

typedef struct 
{
	/* VAR_INPUT (digital) */
	BOOL enable;						/*enable controler*/
	BOOL startStop;						/* start controler = true, stop controler = false*/
	BOOL together;						/*heating with other controllers*/
	BOOL setPointReached;				/*reached setpoint value */
    
    BOOL manualControl;					/*working with open loop*/
    BOOL softStartReached;				/*soft start temprature point reached*/
    BOOL softStartTimeReached;			/*soft start time up change setpoint for target*/	
    BOOL checkFreezing;				    /*check if output freezed*/
	BOOL controlFreezed;				/*controler has freezed*/
	BOOL checkSensorDefect;					/* check if sensor defective*/
	/* times*/
	UINT softStartActTime;				/*controller softstart actual time*/
	UINT sensorErrorTimeOn;				/*delay sensor alarm*/
	UINT actFreezingTime;				/*actual freezing time*/
	UINT sensorErrorTimeReset;			/*sensor Error Time Reset*/
	/* VAR_general */
	
	INT lowDeviation;				/*low Deviation temprature*/
	INT highDeviation;				/*high Deviation temprature*/
	INT freezTime;						/*temperature not changing with 100% output for this time*/
	UINT error[5];							/*Controler error : 0  max. Tolerance exceede	
                 												1  min. Tolerance exceede	
                 												2  maximum allowed temperature
                 												3  short circuit (-32768)
                 												4  sensor error (if not open loop selected)
                 												5  output is not changing (frozen)*/

	/* VAR_INPUT (analog) */
	
	short setValue;						/*set temperature value*/
	short actValue;						/*act temperature value*/
	short maxTol;						/*controler maximum tolerance*/
	short minTol;						/*controler minimum tolerance*/
	short freezTemperature;				/* freezing temperature*/
	short sensorCorrection;				/*sensor correction up to 3 degrees, beyond change sensor*/
	
	UINT groupNum;						/*controler belong to group number*/
	UINT contType;						/*controler off = 0, controler pid = 1, softstart pid = 2, open loop = 3, controler in tune = 4,controler all rise = 5*/
	DINT integratorWindow;				/* prevent overshooting */
	/* VAR_OUTPUT (analog) */
	INT outPut;							/*controler output*/
	INT precentageOutPut;				/*controler output in 0-100% (precentage)*/
	INT manPrecentage;					/*manual precentage output values*/
	/* VAR (analog) */
	
	UINT status;						/*controler status*/
	UDINT baseTime;						/*controler base time*/
	USINT out_mode;						/*controler out modes  Controller */
	
	
	/*struct*/
	
	LCPID_typ controler;				/*pid controler*/
	LCPIDTune_typ autoTuneControler;	/*autotune controler*/
	LCPWM_typ	pwm;					/*pulse width modulator*/

} Controler;



#endif