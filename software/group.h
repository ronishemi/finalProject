#ifndef Group_H_
#define Group_H_

typedef struct 
{
	/* VAR_def  */
	UINT groupNum;						/*Group numbers:   0  barrel  
										                   1  mould heating
                 										   2  "cooling"*/
	UINT delayTime;						/*group delay time*/
	UINT actDelayTime;					/*group actual delay time*/
	UINT contactorOn;					/*time until contactor is on*/
	UINT actContactorOn;				/*actual Contactor connection time */

	/* VAR_general */
	INT minTemperature;				/*minimum temperature*/
	INT maxTemperature;				/*maximum temperature*/	
	UINT error[7];							/*group error*/
	UDINT setCycleTime;					/*group set cycle time*/
	UDINT actCycleTime;					/*group act cycle time*/
	UINT maxDiffTemp;					/*max difference between two zones*/
	UINT freezingTime;					/*freezing time for group*/
	UINT softStartTime;					/*soft start time*/
	UINT softStartMaxOutPut;			/*limit softstart output*/
	short softStartTemp;				/* soft start temrature*/
	short deltaTempratureStep;			/*rise all together by delta step*/
	short stepTempratureSetPoint;		/*step tempartue setpoint*/
	UDINT tuningRequestGroup;
	USINT outPutType;					/*0 = solid state; 1 = relay; 2 = analog*/
	/*VAR status*/
	BOOL stepReached;					/*all controllers reched temprature step*/ 
	BOOL aboveMinTemperature;			/*above minimum temperature*/
	BOOL softStart;						/*soft start option working on/off*/
	BOOL softStartReached;				/*all controler in the group reached soft start temperatur*/
	BOOL mainSwitch;					/*main physical switch */
	BOOL groupTempReady;				/*all controllers temp in the group are OK*/
	 
} Group;


#endif