//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DSP.H #################################
//---------------------------------------------

#ifndef _DSP_H_
#define _DSP_H_

// Defines for Module Configuration --------------------------------------------
// #define USE_PID_CONTROLLERS
// #define USE_MA32_U8_CIRCULA
#define USE_MA8_U16_CIRCULAR
#define USE_MA16_U16_CIRCULAR
#define USE_MA32_U16_CIRCULAR


// Module Exported Types Constants & Macros ------------------------------------
typedef struct {
    unsigned short v_ma[8];
    unsigned short * p_ma;
    unsigned int total_ma;
} ma8_u16_data_obj_t;

typedef struct {
    unsigned short v_ma[16];
    unsigned short * p_ma;
    unsigned int total_ma;
} ma16_u16_data_obj_t;

typedef struct {
    unsigned char v_ma[32];
    unsigned char * p_ma;
    unsigned int total_ma;
} ma32_u8_data_obj_t;

typedef struct {
    unsigned short v_ma[32];
    unsigned short * p_ma;
    unsigned int total_ma;
} ma32_u16_data_obj_t;

typedef struct {
    short setpoint;
    short sample;
    short last_d;
    short error_z1;
    short error_z2;
    short ki_accumulator;    
    unsigned short kp;
    unsigned short ki;
    unsigned short kd;
} pid_data_obj_t;

typedef struct {
    unsigned short b_param_to_div_by_1000;
    unsigned short a_param_to_div_by_1000;
    unsigned int output_z1;
} IIR_first_order_data_obj_t;


// Module Exported Functions ---------------------------------------------------
unsigned short RandomGen (unsigned int);

#ifdef USE_PID_CONTROLLERS
short PID (pid_data_obj_t *);
void PID_Flush_Errors (pid_data_obj_t *);
short PID_Small_Ki (pid_data_obj_t *);
void PID_Small_Ki_Flush_Errors (pid_data_obj_t *);
#endif

#ifdef USE_MA8_U16_CIRCULAR
void MA8_U16Circular_Reset (ma8_u16_data_obj_t *);
unsigned short MA8_U16Circular (ma8_u16_data_obj_t *, unsigned short);
unsigned short MA8_U16Circular_Only_Calc (ma8_u16_data_obj_t *);
#endif

#ifdef USE_MA16_U16_CIRCULAR
void MA16_U16Circular_Reset (ma16_u16_data_obj_t *);
unsigned short MA16_U16Circular (ma16_u16_data_obj_t *, unsigned short);
unsigned short MA16_U16Circular_Only_Calc (ma16_u16_data_obj_t *);
#endif

#ifdef USE_MA32_U16_CIRCULAR
void MA32_U16Circular_Reset (ma32_u16_data_obj_t *);
unsigned short MA32_U16Circular (ma32_u16_data_obj_t *, unsigned short);
unsigned short MA32_U16Circular_Only_Calc (ma32_u16_data_obj_t *);
#endif


#ifdef USE_MA32_U8_CIRCULAR
void MA32_U8Circular_Reset (ma32_u8_data_obj_t *);
unsigned char MA32_U8Circular (ma32_u8_data_obj_t *, unsigned char);
unsigned char MA32_U8Circular_Only_Calc (ma32_u8_data_obj_t *);
#endif

unsigned short DSP_Vector_Get_Max_Value (unsigned short *, unsigned char);
unsigned short DSP_Vector_Get_Min_Value (unsigned short *, unsigned char);
void DSP_Vector_Calcule_Frequencies (unsigned short *,
                                     unsigned char ,
                                     unsigned short *,
                                     unsigned char ,
                                     unsigned char *);

unsigned short IIR_first_order (IIR_first_order_data_obj_t *, unsigned short);


#endif /* _DSP_H_ */

//--- end of file ---//
