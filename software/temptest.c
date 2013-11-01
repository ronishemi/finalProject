
/** INCLUDE Files **/
#include <bur/plctypes.h> /** IEC datatypes **/
#include "loopcont.h"     /** LoopCont Library **/
#include "loopconr.h"     /** LoopConR Library **/
#include "sys_lib.h"     /** system lib **/
#include <plastlib.h>		  /*plastic lib*/
#include "asstring.h"     /** string lib **/
#include "Controler.h"     /** test**/
#include "Group.h"     /** test**/

#define MAX_CONTROLLERS 17
#define MAX_GROUPS 3
/* ===TIMER  variables declaration === */
    
/** Variable declarations **/
_LOCAL  skal_typ 					 skal_;
_LOCAL  LCCounter_typ                baseLCCounter;
_LOCAL  LCRSimModExt_typ             *extruderLCRSimModExt[MAX_CONTROLLERS],extruderLCRSimModExt1;
_LOCAL  Group 						 group[MAX_GROUPS];
_LOCAL  Controler 					 *controlers[MAX_CONTROLLERS],*Ctr,controler[MAX_CONTROLLERS];
_LOCAL  DINT 						 deltaT;
_LOCAL  INT							 pidPar_p[MAX_CONTROLLERS],pidPar_i[MAX_CONTROLLERS],pidPar_d[MAX_CONTROLLERS];	
_LOCAL  UDINT                        tuningRequest,timer1_set,timer1_act;
_LOCAL int 							 statusAlarm;
_LOCAL short						 testTEMP;
_GLOBAL short 						actTemp[MAX_CONTROLLERS];
_LOCAL BOOL 						 tuneStarted,tuneDone,firstScane,secondScane,timer_output;
 
_GLOBAL	short                        setValues[MAX_CONTROLLERS],minimumTemprature[MAX_GROUPS],deltaTempratureStep[MAX_GROUPS],maxTemperature[MAX_GROUPS],minTemperature[MAX_GROUPS],barrelSoftTemp,mouldSoftTemp,coolSoftTemp;
_GLOBAL UINT  						 barrelSoftTime,mouldSoftTime,coolSoftTime,softStartPrecentage[MAX_GROUPS];
_GLOBAL UINT 						 maxDiffTemp_0,maxDiffTemp_1,maxDiffTemp_2,sensorErrorTimeOn,sensorErrorTimeReset,grp,groups[MAX_CONTROLLERS],integratorWindow[MAX_CONTROLLERS];
_GLOBAL INT 						 setValue_0,actValue_0,barrelDelayTime,mouldDelayTime,coolDelayTime;
_GLOBAL INT 						 manualPrecentage[MAX_CONTROLLERS],sensorCorrection[MAX_CONTROLLERS],highDeviation[MAX_CONTROLLERS],lowDeviation[MAX_CONTROLLERS];
_GLOBAL BOOL  						 barrelSoftStart_On,mouldSoftStart_On,coolSoftStart_On,startTune[MAX_CONTROLLERS],enable[MAX_CONTROLLERS],together[MAX_CONTROLLERS];
_GLOBAL INT 						 zoneNumber,old_zoneNumber,new_zoneNumber,i,j,k,outPutPrecentage[MAX_CONTROLLERS];
_GLOBAL UDINT 						 status;
_GLOBAL BOOL 					     cylinder_contactor_In,mould_contactor_In,start_button,switch_on_cylinder,switch_on_mould,groupColdStart[MAX_GROUPS],manual[MAX_CONTROLLERS],start1,coldStart,test,test2;
_GLOBAL BOOL						 cylinder_contactor_Out,mould_contactor_Out,endTwoSec,start_tune;	
_GLOBAL USINT 						 outPutType[MAX_GROUPS],freezingTime[MAX_GROUPS],groupNumToTune;
_GLOBAL REAL  						 P_Kp[MAX_CONTROLLERS],I_Tn[MAX_CONTROLLERS],D_Tv[MAX_CONTROLLERS],Tf[MAX_CONTROLLERS],Kw[MAX_CONTROLLERS];
_GLOBAL REAL 						 Kp_Tn_1[MAX_CONTROLLERS],Tv_Tf_1[MAX_CONTROLLERS],Tf_reciproc[MAX_CONTROLLERS];

/* ====== INIT Part ======= */
void _INIT temp1INIT(void)
{
	firstScane = secondScane = 1;
	statusAlarm =  0;
	 /* set delay time */
		timer1_set = 400;
	
	    if(zoneNumber > MAX_CONTROLLERS)
	    	zoneNumber = MAX_CONTROLLERS;	 	    	 
    	
		new_zoneNumber = old_zoneNumber = zoneNumber;
		addController(0);
        timer1_act = 0;
       
       

}

void _CYCLIC temp1CYCLIC(void)
{
/* Time base counter */
  LCCounter(&baseLCCounter);
start_button=1;
  coldStart=1;
  if(timer1_set > timer1_act){
  timer_output=0;
 	++timer1_act;
 	}else
  		timer_output=1;
 /*=========================================================================================
 *						Alarms
 *==========================================================================================/
  /*aliminate low temp */
	if(start_button)
		group[0].error[1] = group[1].error[1] = group[2].error[1] = 0;
 
  if((old_zoneNumber < zoneNumber)&&!start_tune){		
  	k = old_zoneNumber;
   	old_zoneNumber = zoneNumber;
   	addController(k);
   	new_zoneNumber = zoneNumber;
  }
	
/***************************************/

  if((old_zoneNumber > zoneNumber)&&!start_tune){		
  	k = old_zoneNumber;
  	delController(k);
  	new_zoneNumber = old_zoneNumber = zoneNumber;
  }
	 
  
/*============================================================================
                HEATING ORGANIZATION
  ============================================================================

Function:  Heating Organisation
           Sensor Checking
           Tolerance checking
           Soft start
           Controller selection and optimization

Group numbers:   0  barrel  
                 1  mould heating
                 2  "cooling"
                
controlers[].enable:  	on/off
controlers[].contType 	0  switch off
						1:  heating PID
                 		2:  heating PID for hotrunner (with soft start allowed)
                		3:  open loop
                 		4: AutoTuning
                 		5: Together
                 		
group[].error[]:    	 0  max. Tolerance exceede
   						 1  min. Tolerance exceede	
                 		 2  maximum allowed temperature
                 		 3  short circuit (-32768)
                 		 4  sensor error (if not open loop selected)
                 		 5  output is not changing (frozen)
						 6 contactor malfunction
===================================================================*/
   
  /*******assign parameters*******************************/
	 /**alarm times*/
  sensorErrorTimeOn = 10;				/*20 sec delay befor sensor error*/
  sensorErrorTimeReset = 10;			/*10 sec delay befor sensor error reset*/
  
   /*max temperature for group*/
  group[0].maxTemperature = maxTemperature[0];		/*barrel*/
  group[1].maxTemperature = maxTemperature[1];		/*mould*/
  group[2].maxTemperature = maxTemperature[2];		/*cooling*/
   
   /*min temparature for group*/
 /*   group[0].minTemperature = minTemperature[0];		/*barrel*/
 /* 	group[1].minTemperature = minTemperature[1];		/*mould*/
 /* 	group[2].minTemperature = minTemperature[2];		/*cooling*/
	group[0].minTemperature = group[1].minTemperature = group[2].minTemperature = 10;
	/*cycle time for group*/
    group[0].setCycleTime = 10*baseLCCounter.mscnt;		/*barrel 10mSEC*/
  	group[1].setCycleTime = 10*baseLCCounter.mscnt;		/*mould 10mSEC*/
  	group[2].setCycleTime = 500*baseLCCounter.mscnt;	/*cooling 500mSEC*/
	
	/*delay time start count after all controllers are above low Deviation*/
    group[0].delayTime = barrelDelayTime;		/*barrel*/
  	group[1].delayTime = mouldDelayTime;		/*mould*/
  	group[2].delayTime = coolDelayTime;		/*cooling*/
	
	/*max difference between two zones*/
    group[0].maxDiffTemp = maxDiffTemp_0;		/*barrel*/
  	group[1].maxDiffTemp = maxDiffTemp_1;		/*mould*/
  	group[2].maxDiffTemp = maxDiffTemp_2;		/*cooling*/
  	
  	/* soft start on*/  	
  	if((minimumTemprature[0]<9999)&&(minimumTemprature[0] < barrelSoftTemp))
  		group[0].softStart = barrelSoftStart_On;
  	if((minimumTemprature[1]<9999)&&(minimumTemprature[1] < mouldSoftTemp))
  		group[1].softStart = mouldSoftStart_On;
  	if((minimumTemprature[2]<9999)&&(minimumTemprature[2] < coolSoftTemp))	
		group[2].softStart = coolSoftStart_On;
  	
  	/*soft start assign temprature parameters*/  	
	group[0].softStartTemp = barrelSoftTemp;
	group[1].softStartTemp = mouldSoftTemp;
	group[2].softStartTemp = coolSoftTemp;
	
	/*soft start assign time parameters*/  	
	group[0].softStartTime = barrelSoftTime;
	group[1].softStartTime = mouldSoftTime;
	group[2].softStartTime = coolSoftTime;
	
	/* delta temprature rise together*/
	group[0].deltaTempratureStep = deltaTempratureStep[0];
	group[1].deltaTempratureStep = deltaTempratureStep[1];
	group[2].deltaTempratureStep = deltaTempratureStep[2];
		 
	
  	/*above min temp unless otherwise*/
  	group[0].aboveMinTemperature = 1;
  	group[1].aboveMinTemperature = 1;
  	group[2].aboveMinTemperature = 1;
	
	/*all controllers are done unless otherwise*/
	group[0].softStartReached = 1;
	group[1].softStartReached = 1;
	group[2].softStartReached = 1;
	
	/*all step are ok unless otherwise*/
	group[0].stepReached = 1;
	group[1].stepReached = 1;
	group[2].stepReached = 1;
	
	/*output type */
	group[0].outPutType = outPutType[0];
	group[1].outPutType = outPutType[1];
	group[2].outPutType = outPutType[2];
	
	/*contactor delay time operation*/
	group[0].contactorOn = 30;		/*time in seconds befor alarm*/
	group[1].contactorOn = 30;		/*time in seconds befor alarm*/
  
  /*******assign parameters*******************************/
	
	tuningRequest = LCPID_TUNE_REQU_OSCILLATE + LCPID_TUNE_REQU_DIR_POS + LCPID_TUNE_REQU_PID + LCPID_TUNE_REQU_OSC_1 + LCPID_TUNE_REQU_PER_3;
	group[0].tuningRequestGroup = tuningRequest;
	group[1].tuningRequestGroup = tuningRequest;
	group[2].tuningRequestGroup = tuningRequest;
	
	
	/*delay for freez alarm*/
	group[0].freezingTime = freezingTime[0]; /*10 sec delay for freez alarm*/
 	group[1].freezingTime = freezingTime[1]; /*10 sec delay for freez alarm*/
 	group[2].freezingTime = freezingTime[2]; /*10 sec delay for freez alarm*/
 	
 	/*assign main switch to group*/
 	 group[0].mainSwitch = switch_on_cylinder;
 	 group[1].mainSwitch = switch_on_mould;
 	 
 	/*max softStart Precentage*/
 	  		/*group 0*/
 	  		skal_.in = softStartPrecentage[0];
            skal_.inmin = 0;
            skal_.inmax = 99;
            skal_.outmin = 0;
            skal_.outmax = 32767;
            skal(&skal_);
     		group[0].softStartMaxOutPut =  skal_.out; /* % output*/
			/*group 1*/
 	  		skal_.in = softStartPrecentage[1];
            skal_.inmin = 0;
            skal_.inmax = 99;
            skal_.outmin = 0;
            skal_.outmax = 32767;
            skal(&skal_);
     		group[1].softStartMaxOutPut =  skal_.out; /* % output*/
     		/*group 2*/
 	  		skal_.in = softStartPrecentage[2];
            skal_.inmin = 0;
            skal_.inmax = 99;
            skal_.outmin = 0;
            skal_.outmax = 32767;
            skal(&skal_);
     		group[2].softStartMaxOutPut =  skal_.out; /* % output*/
     		
   	if(start_tune&&!tuneStarted)
   		tuneStarted=1;
   	
   	
   	if(!start_tune&&tuneStarted){
		for(i=0;i<new_zoneNumber;++i)
			startTune[i]= 0; 
   		   	tuneStarted=0;
     }
    
	if(!new_zoneNumber)
       return;

	minimumTemprature[0] = minimumTemprature[1] = minimumTemprature[2] = 9999;		
	i=0;   
	do
	{
	controlers[i]->enable = enable[i];
	controlers[i]->together = together[i];
	controlers[i]->setValue = setValues[i];
	controlers[i]->groupNum = groups[i];   
    controlers[i]->minTol = lowDeviation[i];
	controlers[i]->maxTol = highDeviation[i];
	controlers[i]->sensorCorrection = sensorCorrection[i];
	controlers[i]->contType = 1;
	controlers[i]->manualControl = manual[i];
	if(!timer_output)
	controlers[i]-> setPointReached =0;
		/* change input to precentages*/
   			skal_.in = manualPrecentage[i];
            skal_.inmin = 0;
            skal_.inmax = 99;
            skal_.outmin = 0;
            skal_.outmax = 32767;
            skal(&skal_);
     controlers[i]->manPrecentage = skal_.out; /* % output*/
	
	if(controlers[i] -> autoTuneControler.state)
		controlers[i] -> contType = 4;
	else{
	 	if(controlers[i] -> manualControl)
			controlers[i] -> contType = 3;
		else if(group[controlers[i] -> groupNum].softStart)
				controlers[i] -> contType = 2;
		else if(controlers[i] -> together)
				controlers[i] -> contType = 5;			
	}	
controler[i]=*controlers[i]; 	
 Ctr = (controlers[i]);
  if (Ctr ->controler.enable) 
   { 
 
   	/* change output to precentages*/
   			skal_.in = controlers[i] -> outPut;
            skal_.inmin = 0;
            skal_.inmax = 32767;
            skal_.outmin = 0;
            skal_.outmax = 100;
            skal(&skal_);
            controlers[i] ->precentageOutPut = skal_.out; /*;% output*/
            outPutPrecentage[i] = Ctr ->precentageOutPut;
      /*sensor correction*/      
		Ctr -> actValue = Ctr -> actValue + Ctr -> sensorCorrection;

   grp = Ctr -> groupNum;     
	
                    		

     /*;=====================================================================
     ;SENSOR ERROR CHECKING:
     ;=====================================================================*/
    if(testTEMP > 7500)
    	Ctr -> actValue = testTEMP; 
     
     if (Ctr -> contType != 3)                /* Set Error - Bit if sensor error, not in open loop */
     {
        if (Ctr -> actValue > 7500)             /* Check for Sensor Error*/
        {
          Ctr -> actValue = 7500;                    /* Temperature value for VIS*/
         if(!Ctr -> checkSensorDefect)
         	Ctr -> sensorErrorTimeOn = baseLCCounter.seccnt + sensorErrorTimeOn;
          Ctr -> checkSensorDefect = 1;
        }
        else                                   /* No sensor error*/
        {
        	if(Ctr -> checkSensorDefect && !Ctr -> sensorErrorTimeReset)
        		Ctr -> sensorErrorTimeReset = baseLCCounter.seccnt + sensorErrorTimeReset;
        	if (Ctr -> sensorErrorTimeReset < baseLCCounter.seccnt){	 
         		 Ctr -> sensorErrorTimeReset = Ctr -> checkSensorDefect = 0;
         		 Ctr -> controler.X = Ctr -> actValue;               /* New temperature */
          	}
        };

        /*;===========================================================
        ; the counter has reached limit, the group is on and the zone*
        ; is on ----> set sensor error 
        ;===========================================================*/
        if (Ctr -> checkSensorDefect == 1) 
        {
            if (Ctr -> sensorErrorTimeOn <= baseLCCounter.seccnt) 
           {
              group[grp].error[4] = i+1;          	/* Sensor brak bit of group*/
			  Ctr ->error[4] =	i+1;		
           };
        }
        else
        {
           Ctr -> sensorErrorTimeOn = 0;        
           
        };
       
     };
                                           
		
     /*;==================================
     ;Check for max. allowed Temperature 
     ;==================================*/
     if (Ctr -> actValue > group[grp].maxTemperature)    /*;*** Switch off whole grp*/
     {
      group[grp].error[2] = i+1;    /*;*** zone number --> additional info*//*shemi*/
      Ctr ->error[2] =	i+1;
     };
     
   
	 /*;===================================================
     ;Check for max tolerance
     ;===================================================*/
		if (!start_tune&&(Ctr -> actValue > (Ctr ->maxTol + Ctr -> setValue)))
		{
			group[grp].error[0] = i+1;
			 Ctr ->error[0] =	i+1;
		}
	/*;===================================================
     ;Check for min tolerance
     ;===================================================*/
		if (Ctr -> actValue < (Ctr -> setValue - Ctr ->minTol))
		{
			group[grp].error[1] = i+1;
			 Ctr ->error[1] =	i+1;
		}
		

     /*;===================================================
     ;Check for "freezing" of actual value during heating
     ;===================================================*/
     switch( Ctr -> contType )  
     {       
                
        case 1:
        case 2:   /*;control with act. temp. signal is active*/
        {
        	
           if (Ctr -> precentageOutPut == 100)                  /*;100 % ED*/
              Ctr -> checkFreezing = 1;
           else
              Ctr -> checkFreezing = Ctr ->freezTime = 0;
              
       		if(Ctr -> checkFreezing&&!Ctr ->freezTime)
       			Ctr ->freezTime = baseLCCounter.seccnt + group[grp].freezingTime; /*add time to freezing*/ 
		
	      if(Ctr ->freezTime <= baseLCCounter.seccnt&&Ctr -> checkFreezing){
             if (Ctr -> controler.X <= Ctr -> freezTemperature)           /*;temperature falling or equal*/
              	Ctr -> controlFreezed = 1;
           	  else
              	Ctr -> controlFreezed = 0;
			}	         
            Ctr -> freezTemperature = Ctr -> actValue;
          
    /*========================================================================
	; 		soft start 
	;=========================================================================*/
		/*softStart  softStartTemp softStartTemp			*/
			if(group[grp].softStart){
				if(!Ctr -> softStartTimeReached)
					Ctr -> setValue = group[grp].softStartTemp;  

				if(Ctr -> actValue >= group[grp].softStartTemp)
						Ctr -> softStartReached = 1;
				if(Ctr -> softStartReached){
					if(!Ctr -> softStartActTime)
						Ctr -> softStartActTime = baseLCCounter.seccnt + group[grp].softStartTime;
				if(Ctr -> softStartActTime <= baseLCCounter.seccnt)
					Ctr -> softStartTimeReached = 1;
				}else
					group[grp].softStartReached = 0;
				if(Ctr -> actValue < (group[grp].softStartTemp - 100))
					Ctr -> softStartTimeReached = Ctr -> softStartReached = Ctr -> softStartActTime = 0;						 		
			}
        
        
                  
          }
         default:
         break;     
      }
      	   	if(Ctr -> controlFreezed)
                group[grp].error[5] = i+1;    /* freezing alarm bit of group on*/ 
        
         
  
        
          /*;======================================================
          ;on AT660, a temperature of -32768 indicates that:
          ;a) not valid sensor type adjusted
          ;b) the voltage from sensor < (voltage measurement range) or (range of sensor)
          ;c) comparator temperature out of range
          ;====================================================== */
          if (Ctr -> controler.X == -32768)            /*;short circuit detection from AT660*/
          {
            /* Ctr -> Pbon = 0;*/
             group[grp].error[3]  = i+1;      /*;additional info: channel no.*/
             
         }
	/*========================================================================
	; 		cold start delay check
	;=========================================================================*/
				if(secondScane||!group[grp].mainSwitch){
					
					if(Ctr -> actValue < (Ctr -> setValue - Ctr ->minTol)){
						group[grp].aboveMinTemperature = groupColdStart[grp] = 0;
						
						}
					}
					else{
						if((Ctr -> actValue < Ctr -> minTol))
							group[grp].aboveMinTemperature = 0;
						}
	/*=======================================================================
	;				all controllers rise together
	;========================================================================*/
		/* find the minimum temprature in the group*/
			if((minimumTemprature[grp] > Ctr -> actValue)&&!Ctr -> setPointReached)
				minimumTemprature[grp] = Ctr -> actValue;

		if((Ctr -> setValue - Ctr ->minTol) <= Ctr -> actValue)
			Ctr -> setPointReached = 1;
		else 
			Ctr -> setPointReached = 0;
			
			if(Ctr -> together && !Ctr -> setPointReached && !start_tune){
				Ctr -> setValue = group[grp].stepTempratureSetPoint;
			if(Ctr -> actValue <= Ctr -> setValue)
				group[grp].stepReached = 0;				
			}
   }
   i=i+1;
}
while( i < new_zoneNumber);                         /* ;all controllers checked*/

/* check groups befor activate controller*/

 grp=0;
 while(grp < 3){	
 	if(group[grp].aboveMinTemperature){
		
		if(!groupColdStart[grp]){
			
			if(!group[grp].actDelayTime)
				group[grp].actDelayTime = baseLCCounter.seccnt + group[grp].delayTime;
			if(group[grp].actDelayTime < baseLCCounter.seccnt){
				groupColdStart[grp] = 1;
				group[grp].actDelayTime = 0;
				}
		}else
			group[grp].groupTempReady = 1;	
				
	}else
		group[grp].groupTempReady = 0;
	if(!group[grp].softStartReached&&group[grp].softStart||start_tune)
		group[grp].groupTempReady = 0;
	
	if(group[grp].stepReached){
		if(minimumTemprature[grp]<9999)
			group[grp].stepTempratureSetPoint = minimumTemprature[grp] + group[grp].deltaTempratureStep;
		else
			group[grp].stepTempratureSetPoint = group[grp].deltaTempratureStep;
	}
	grp =grp+1;
}
       
  j=tuneDone=0;
do{
 	
  /*=========================================================================================	
   |  											PID tuning 									 |
   ===========================================================================================*/
  controlers[j]->autoTuneControler.enable   = 1;
  controlers[j]->autoTuneControler.okToStep = controlers[j]->autoTuneControler.rdyToStep;
  controlers[j]->autoTuneControler.basetime = group[controlers[j]->groupNum].setCycleTime;
  LCPIDTune(&controlers[j]->autoTuneControler);		/* LCPIDTune function block call */
if(startTune[j]&&start_tune&&(groupNumToTune == controlers[j]->groupNum)){
endTwoSec = 1;
controlers[j]->autoTuneControler.request  = group[controlers[j]->groupNum].tuningRequestGroup;
if (controlers[j]->autoTuneControler.state == LCPID_TUNE_STATE_FINISHED)/*LCPID_TUNE_STATE_FINISHED*/
  {
  	startTune[j]=0;
    controlers[j]->autoTuneControler.request  = LCPID_TUNE_REQU_OFF;
  	memcpy(&P_Kp[j],&controlers[j]->autoTuneControler.internal_data.Kp1,sizeof(DINT));
	memcpy(&Kp_Tn_1[j],&controlers[j]->autoTuneControler.internal_data.Kp_Tn_1,sizeof(DINT));
	memcpy(&Tv_Tf_1[j],&controlers[j]->autoTuneControler.internal_data.Tv_Tf_1,sizeof(DINT));
	memcpy(&Tf_reciproc[j],&controlers[j]->autoTuneControler.internal_data.Tf_reciproc_1,sizeof(DINT));
	memcpy(&Kw[j],&controlers[j]->autoTuneControler.internal_data.Kw1,sizeof(DINT));
	I_Tn[j] = !Kp_Tn_1[j] ? 0 : P_Kp[j]/Kp_Tn_1[j];
	D_Tv[j] = !Tf_reciproc[j] ? 0 : Tv_Tf_1[j]/Tf_reciproc[j];
	Tf[j] = !Tf_reciproc[j] ? 0 : 1/Tf_reciproc[j];

  }
	if(!tuneDone)  
  	tuneDone = startTune[j];
  /**check if all controllers finished tune*/
 
   
}
if(!start_tune){
	Kp_Tn_1[j] = !I_Tn[j] ? 0 : P_Kp[j]/I_Tn[j];
	Tv_Tf_1[j] = !Tf[j] ? 0 : D_Tv[j]/Tf[j];
	Tf_reciproc[j] = !Tf[j] ? 0 : 1/Tf[j];
	memcpy(&controlers[j]->autoTuneControler.internal_data.Kp1,&P_Kp[j],sizeof(DINT));
	memcpy(&controlers[j]->autoTuneControler.internal_data.Kp_Tn_1,&Kp_Tn_1[j],sizeof(DINT));
	memcpy(&controlers[j]->autoTuneControler.internal_data.Tv_Tf_1,&Tv_Tf_1[j],sizeof(DINT));
	memcpy(&controlers[j]->autoTuneControler.internal_data.Tf_reciproc_1,&Tf_reciproc[j],sizeof(DINT));
	memcpy(&controlers[j]->autoTuneControler.internal_data.Kw1,&Kw[j],sizeof(DINT));
	}

  /*=========================================================================================	
   |  					PID controller														 |
   ===========================================================================================*/
	
	
  if(timer_output&&group[controlers[j]->groupNum].mainSwitch)
   controlers[j]->controler.enable   = controlers[j]->enable;
  else
   controlers[j]->controler.enable   = controlers[j]->contType = 0;
  controlers[j]->controler.ident    = controlers[j]->autoTuneControler.ident; /* ident of PIDTune -> provides parameters (Kp, Tn, Tv, ...) */
  controlers[j]->controler.W        = controlers[j]->setValue;
  controlers[j]->integratorWindow = integratorWindow[j];
  controlers[j]->controler.X        = controlers[j]->actValue;
  controlers[j]->controler.basetime = group[controlers[j]->groupNum].setCycleTime;;
  controlers[j]->autoTuneControler.P_manualAdjust = pidPar_p[j];
  controlers[j]->autoTuneControler.I_manualAdjust = pidPar_i[j];
  controlers[j]->autoTuneControler.D_manualAdjust = pidPar_d[j];
	
	if(!controlers[j]->manualControl)
		controlers[j]->controler.out_mode = LCPID_OUT_MODE_AUTO;
	else{
		controlers[j]->controler.out_mode = LCPID_OUT_MODE_MAN_JOLTFREE;
		controlers[j]->controler.Y_man = controlers[j]->manPrecentage;
		}	
  LCPID(&controlers[j]->controler);			/* LCPID function block call */
  
  deltaT = controlers[j]->controler.W - controlers[j]->controler.X;
  deltaT = deltaT * deltaT; 
  
  if(!start_tune&&(deltaT > controlers[j]->integratorWindow))
  	controlers[j]->controler.hold_I =1;
  else
  	controlers[j]->controler.hold_I =0;
  
  /* controller output*/
  		/* if temp higher them maxTol shutdown group*/
		if(controlers[j]->controler.enable&&!controlers[j]->error[2]){		
			if(!group[controlers[j]->groupNum].softStart||controlers[j] -> softStartReached||(group[controlers[j]->groupNum].softStartMaxOutPut > controlers[j]->controler.Y ))		
 		   		controlers[j] -> outPut = controlers[j]->controler.Y;
 		   	else 	
 		   		controlers[j] -> outPut =	group[controlers[j]->groupNum].softStartMaxOutPut;
   		}else
   			outPutPrecentage[j] = controlers[j] -> outPut = 0;
 
  	/*aliminate alarms*/	
    if(!group[controlers[j]->groupNum].mainSwitch&&start_button)
    	controlers[j] -> error[0] = controlers[j] -> error[2] = controlers[j] -> error[3] = controlers[j] -> error[5] = controlers[j] -> error[6] = 0;
	/*=========================================================================================	
   |  					Pulse width modulator       										  |
   ===========================================================================================*/
		/*0 = solid state; 1 = relay; 2 = analog*/
		
     controlers[j] -> pwm.x = controlers[j] -> outPut;
     
  	if(group[controlers[j]->groupNum].outPutType == 1)
  		controlers[j] -> pwm.basetime = 500*baseLCCounter.mscnt;		/*relay*/
  	else if(!group[controlers[j]->groupNum].outPutType)
  		controlers[j] -> pwm.basetime = 10*baseLCCounter.mscnt;		/*solid state*/
  	if(group[controlers[j]->groupNum].outPutType != 2)
  		LCPWM(&controlers[j] -> pwm);		/* LCPWM function block call */


	/*=========================================================================================	
   |  					Extruder simulation model      										  |
   ===========================================================================================*/   
         
	extruderLCRSimModExt[j]->Alpha_h = controlers[j] -> outPut / 327.67;
    LCRSimModExt(extruderLCRSimModExt[j]);	/* LCRSimModExt function block call */
	controlers[j]->actValue = extruderLCRSimModExt[j] -> y + controlers[j]-> sensorCorrection;
	/*actTemp[j] = extruderLCRSimModExt[j] -> y;*/
	actTemp[j] = controlers[j]->actValue;
	++j;
	}while(j<new_zoneNumber);
	
	if(!tuneDone)
  		start_tune = tuneStarted = 0;
   

	
	
	
	/*==================================================================== 			
  ;					start contactor for grpup 0
  ;=====================================================================*/
        
    if(group[0].mainSwitch&&!group[0].error[2]&&!group[0].error[3]&&!group[0].error[5]){
    	 cylinder_contactor_Out = 1;
    	if(group[0].actContactorOn <= baseLCCounter.seccnt&&!cylinder_contactor_In)
    		group[0].error[6] = 1;	 
    }else{
    	cylinder_contactor_Out = 0;
    	group[0].actContactorOn = baseLCCounter.seccnt + group[0].contactorOn;
    	}
    /*aliminate alarms group 0*/	
    if(!group[0].mainSwitch&&start_button)
    	group[0].error[0] = group[0].error[2] = group[0].error[3] = group[0].error[5] = group[0].error[6] = 0;		 
 /*==================================================================== 			
  ;					start contactor for grpup 1
  ;=====================================================================*/
        
    if(group[1].mainSwitch&&!group[1].error[2]&&!group[1].error[3]&&!group[1].error[5]){
    	 mould_contactor_Out = 1;
    	if(group[1].actContactorOn <= baseLCCounter.seccnt&&!mould_contactor_In)
    		group[1].error[6] = 1;	 
    }else{
    	mould_contactor_Out = 0;
    	group[1].actContactorOn = baseLCCounter.seccnt + group[1].contactorOn;
    	}
    /*aliminate alarms group 1*/	
    if(!group[1].mainSwitch&&start_button)
    	group[1].error[0] = group[1].error[2] = group[1].error[3] = group[1].error[5] = group[1].error[6] = 0;		 

	if(!firstScane)
		secondScane = 0;
	if(timer_output)
		firstScane=0;
}
	/*=========================================================================================	
   **************************end of program start of functions*********************************			     										  |
   ===========================================================================================*/   

	/*=========================================================================================	
   |  				add controllers										  |
   ===========================================================================================*/   
/****************************************************/
void addController(int i){
       /* Allocate the runtime memory for the defined zones*/
 	
 		 for(;i<zoneNumber;++i)
       {
       
              status = TMP_alloc(sizeof(Controler), (void**)&controlers[i]);
              if(status != 0)
              {
                     controlers[i] = 0;
                     statusAlarm = 3;
                     return;
              }              
              memset(controlers[i], 0, sizeof(Controler));
              
					  /* Parameters for PID tuning */
					 controlers[i]->autoTuneControler.enable          = 0; 
					  controlers[i]->autoTuneControler.Y_min          = 0;
					  controlers[i]->autoTuneControler.Y_max          = 32767;
					  controlers[i]->autoTuneControler.Y0             = 6300;
					  controlers[i]->autoTuneControler.Y1             = 11300;
					  controlers[i]->autoTuneControler.X0             = 250;
					  controlers[i]->autoTuneControler.X_min          = -10000;
					  controlers[i]->autoTuneControler.X_max          = 10000;
					  controlers[i]->autoTuneControler.P_manualAdjust = 0;
					  controlers[i]->autoTuneControler.I_manualAdjust = 0;
					  controlers[i]->autoTuneControler.D_manualAdjust = 0;

              /* Parameters for PID controller */
  					controlers[i]->controler.A        = 0;
  					controlers[i]->controler.Y_man    = 0;
  					controlers[i]->controler.Y_fbk    = 0;
  					controlers[i]->controler.hold_I   = 0;
			  		controlers[i]->controler.out_mode = LCPID_OUT_MODE_AUTO;
					
						/*Parameters for PWM*/
					controlers[i]->pwm.enable = 1;
					controlers[i]->pwm.max_value = 32767;     		/*controller output*/
					controlers[i]->pwm.min_value   = 0;				
					controlers[i]->pwm.t_min_pulse = 10;				
					controlers[i]->pwm.t_period    = 100;	
					
				controlers[i]->autoTuneControler.request  = LCPID_TUNE_REQU_OFF;
	
			status = TMP_alloc(sizeof(LCRSimModExt_typ),(void**)&extruderLCRSimModExt[i]);
				if(status != 0)
              	{
              	statusAlarm = 4;
                 return;
              	}
       		   memset(extruderLCRSimModExt[i], 0, sizeof(LCRSimModExt_typ));	
		
							
				
 			 /* Parameters for extruder simulation model */
 			 extruderLCRSimModExt[i] -> enable   = 1;
 			 extruderLCRSimModExt[i] -> Tt_h     = 900000;	/*900000 microseconds */
 			 extruderLCRSimModExt[i] -> Tt_c     = 500000;	/*500000 microseconds */
 			 extruderLCRSimModExt[i] -> k_h      = 39;
 			 extruderLCRSimModExt[i] -> k_c      = 0.012;
 			 extruderLCRSimModExt[i] -> PT2_T1   = 5.0; /*5.0  (i+2)/2**/
 			 extruderLCRSimModExt[i] -> PT2_T2   = 12.5;/*(i+2)/2**/
 			 extruderLCRSimModExt[i] -> Temp_amb = 250;
 			 extruderLCRSimModExt[i] -> Temp_c   = 250;
 			 extruderLCRSimModExt[i] -> Alpha_c  = 0.0;
       
		}

}
   /*=========================================================================================	
   |  				delete controllers										  |
   ===========================================================================================*/   
void delController(int i){
       /* delete the runtime memory for the defined zones*/
        for(;i>zoneNumber;--i){
              status = TMP_free(sizeof(Controler), (void**)controlers[i]);
              if(status != 0)
              {        
              statusAlarm = 7;            
                     return;
              }
            controlers[i]=0;
       	
			status = TMP_free(sizeof(LCRSimModExt_typ),(void**)extruderLCRSimModExt[i]);
				if(status != 0)
              	{
              	statusAlarm = 8;
                 return;
              	}
             extruderLCRSimModExt[i]=0; 	
        
 		}
}


