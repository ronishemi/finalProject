/* Automation Studio Generated Header File, Format Version 1.00 */
/* do not change */
#ifndef PLASTLIB_H_
#define PLASTLIB_H_
#define _WEAK	__attribute__((__weak__))

#include <bur/plctypes.h>



/* Constants */


/* Datatypes */
typedef struct profcalc_loc_typ
{
	signed short accel;
	signed short decel;
	signed short enddecel;
	unsigned short noSteps;
	unsigned short delaytime;
	signed long Ss[10];
	signed short Sv[10];
	signed long Sspre[10];
	signed short Svpre[10];
	unsigned char z_mode;
	unsigned char e_mode;
	unsigned char dly_mode;
} profcalc_loc_typ;

typedef struct pidxhopt_loc_typ
{
	signed short act_val;
	signed short set_val;
	signed short y;
	signed short l_up;
	signed short l_low;
	signed char method;
	signed short set_step;
	signed short status;
	signed short limit;
	signed short xmax;
	signed short xmin;
	signed short Ttoth;
	signed short Ttotk;
	signed long vmaxh;
	signed long vmaxk;
	signed short timer;
	signed short count;
	signed short dt;
	signed short ist10;
	signed short limtat;
	signed long Xph;
	signed long Tnh;
	signed long Tvh;
	signed short ta;
	signed short ta_fix;
	signed short ta_ext;
	signed short tak;
	signed short tak_fix;
	signed short tak_ext;
	signed short Kph;
	signed short Kih;
	signed short Kdh;
	signed short Kpk;
	signed short Kik;
	signed short Kdk;
	signed short t1;
	signed short deltaT;
	signed short f_cool;
	signed short fastslow;
} pidxhopt_loc_typ;

typedef struct im_typ
{
	signed long minstroke;
	signed long maxstroke;
	signed long mininc;
	signed long maxinc;
	signed long ratemin;
	signed long ratemax;
	signed long shiftfactor;
	signed long no;
	signed long rateinc;
	signed long table;
} im_typ;

typedef struct pidxp_loc_typ
{
	signed short act;
	signed short set;
	signed short kp;
	signed short ki;
	signed short kd;
	signed short c0;
	signed short y_low;
	signed short y_up;
	signed long integr;
	signed long filter;
	signed short y;
} pidxp_loc_typ;

typedef struct pidxh_loc_typ
{
	signed short act;
	signed short set;
	signed short init;
	signed short Kp;
	signed short Ki;
	signed short Kd;
	signed short deltaT;
	signed short c0;
	signed short l_low;
	signed short l_up;
	signed long integr;
	signed long filter;
	signed short out;
} pidxh_loc_typ;

typedef struct tog_geometry_data_typ
{
	float A;
	float R;
	float L;
	float L1;
	float beta;
	float D;
	float H;
	float delta;
	float shifting;
} tog_geometry_data_typ;

typedef struct toglin_datamod_typ
{
	signed long minstroke;
	signed long maxstroke;
	signed long mininc;
	signed long maxinc;
	signed long ratemin;
	signed long ratemaxstroke;
	signed long shoftfactor;
	signed long no_of_table_entries;
	signed long rateinc;
	signed long tab1[10];
} toglin_datamod_typ;



/* Datatypes of function blocks */
typedef struct inc2mmcb
{
	/* VAR_INPUT (analogous) */
	unsigned long pointer;
	signed long inc;
	/* VAR_OUTPUT (analogous) */
	signed long mm;
} inc2mmcb_typ;

typedef struct skal
{
	/* VAR_INPUT (analogous) */
	signed short in;
	signed short inmin;
	signed short inmax;
	signed short outmin;
	signed short outmax;
	/* VAR_OUTPUT (analogous) */
	signed short out;
} skal_typ;

typedef struct ramp_q1
{
	/* VAR_INPUT (analogous) */
	signed short in;
	signed short target;
	unsigned short deltat;
	unsigned char firstinit;
	signed short accel;
	signed long memory;
	signed long qmemory;
	signed long dstep;
	signed long qaend;
	signed long qstep;
	/* VAR_OUTPUT (analogous) */
	signed short lin_out;
	signed short q_out;
	unsigned short status;
	/* VAR (analogous) */
	signed short targ_old;
} ramp_q1_typ;

typedef struct pid
{
	/* VAR_INPUT (analogous) */
	signed short act;
	signed short set;
	signed short kp;
	signed short ki;
	signed short kd;
	signed short c0;
	signed short kv;
	signed short kvolpd;
	signed short cloop;
	signed short zone_neg;
	signed short zone_pos;
	signed short y_low;
	signed short y_up;
	signed short abs_low;
	signed short abs_up;
	signed short ilimit;
	/* VAR_OUTPUT (analogous) */
	signed long integr;
	signed long filter;
	signed short y;
	signed short y_oloop;
} pid_typ;

typedef struct pidx
{
	/* VAR_INPUT (analogous) */
	signed short act;
	signed short set;
	signed short kp;
	signed short ki;
	signed short kd;
	signed short c0;
	signed short y_low;
	signed short y_up;
	/* VAR_OUTPUT (analogous) */
	signed long integr;
	signed long filter;
	signed short y;
} pidx_typ;

typedef struct pidxp
{
	/* VAR_INPUT (analogous) */
	unsigned long ptr;
} pidxp_typ;

typedef struct pidxh
{
	/* VAR_INPUT (analogous) */
	unsigned long ptr;
} pidxh_typ;

typedef struct pt1
{
	/* VAR_INPUT (analogous) */
	signed short in;
	signed short c0;
	/* VAR_OUTPUT (analogous) */
	signed short out;
	signed long filter;
} pt1_typ;

typedef struct ramp
{
	/* VAR_INPUT (analogous) */
	signed short setval;
	unsigned long accel;
	/* VAR_OUTPUT (analogous) */
	signed short out;
	signed long integr;
} ramp_typ;

typedef struct pidxhopt
{
	/* VAR_INPUT (analogous) */
	unsigned long ptr;
} pidxhopt_typ;

typedef struct lin
{
	/* VAR_INPUT (analogous) */
	signed long x;
	unsigned long x_tab;
	unsigned long y_tab;
	unsigned short no;
	/* VAR_OUTPUT (analogous) */
	signed short y;
	/* VAR_OUTPUT (digital) */
	plcbit exc_1st;
	plcbit exc_last;
} lin_typ;

typedef struct mm2inc
{
	/* VAR_INPUT (analogous) */
	unsigned long pointer;
	signed long mm;
	/* VAR_OUTPUT (analogous) */
	signed long inc;
} mm2inc_typ;

typedef struct inc2mm
{
	/* VAR_INPUT (analogous) */
	unsigned long pointer;
	signed long inc;
	/* VAR_OUTPUT (analogous) */
	signed long mm;
} inc2mm_typ;

typedef struct verst
{
	/* VAR_INPUT (analogous) */
	signed long inc;
	signed long offset;
	signed short v_set;
	unsigned long ptr;
	signed short factor;
	signed short divisor;
	signed long maxinc;
	signed char en_ip;
	/* VAR_OUTPUT (analogous) */
	signed short out;
} verst_typ;

typedef struct tog_init
{
	/* VAR_INPUT (analogous) */
	unsigned long tog_geometry;
	unsigned long DAid_p;
	unsigned long DAsts;
	unsigned long DAoff;
	unsigned long tog_init_status;
} tog_init_typ;

typedef struct vmeas
{
	/* VAR_INPUT (analogous) */
	unsigned char init;
	unsigned char reset;
	signed short Asdig;
	signed short SsMin;
	signed short SsMax;
	signed short AiMin;
	signed short AiMax;
	unsigned short CycleTime;
	unsigned short c0;
	/* VAR_OUTPUT (analogous) */
	signed short Av;
	signed long sOld;
	signed long filter;
	signed long sAmp;
	signed short cnt;
} vmeas_typ;

typedef struct convert
{
	/* VAR_INPUT (analogous) */
	signed short in;
	signed short num;
	signed short denom;
	/* VAR_OUTPUT (analogous) */
	signed short out;
} convert_typ;

typedef struct vprofms
{
	/* VAR_INPUT (analogous) */
	signed long s_act;
	signed long s_target;
	signed short v_set;
	unsigned short en_ms;
	signed short v_ms;
	signed long s_ms;
	signed short accel;
	signed short decel;
	signed short end_decel;
	signed short t_delay;
	unsigned short intervall;
	unsigned char init;
	unsigned char direction;
	/* VAR_OUTPUT (analogous) */
	unsigned char target;
	signed short v_act;
	/* VAR (analogous) */
	unsigned char target_old;
	signed long v_integr;
	signed long er_start;
	signed long ms_start;
	signed long v_ms2;
	signed long x20end_decel;
	signed long step_accel;
	signed long step_decel;
	signed long z_fact;
	signed long z_fact2;
	unsigned long ms_tmpsqrt;
} vprofms_typ;

typedef struct lin2point
{
	/* VAR_INPUT (analogous) */
	signed short in;
	signed short inmin;
	signed short inmax;
	signed short outmin;
	signed short outmax;
	/* VAR_OUTPUT (analogous) */
	signed short out;
} lin2point_typ;



/* Prototyping of functions and function blocks */
void inc2mmcb(inc2mmcb_typ* inst);
void skal(skal_typ* inst);
void ramp_q1(ramp_q1_typ* inst);
void pid(pid_typ* inst);
void pidx(pidx_typ* inst);
void pidxp(pidxp_typ* inst);
void pidxh(pidxh_typ* inst);
void pt1(pt1_typ* inst);
unsigned short qsqrt(unsigned long radiant);
void ramp(ramp_typ* inst);
void pidxhopt(pidxhopt_typ* inst);
void lin(lin_typ* inst);
void mm2inc(mm2inc_typ* inst);
void inc2mm(inc2mm_typ* inst);
void verst(verst_typ* inst);
void tog_init(tog_init_typ* inst);
void vmeas(vmeas_typ* inst);
void convert(convert_typ* inst);
signed short mV2digits(signed short mv);
signed short ms2c0(unsigned short tf, unsigned short ta);
void vprofms(vprofms_typ* inst);
void lin2point(lin2point_typ* inst);



#endif /* PLASTLIB_H_ */
