// See LICENSE for license details.

#ifndef _SIFIVE_PWM_H
#define _SIFIVE_PWM_H

#include "../platform.h"

/* Register offsets */

#define PWM_CFG   0x00
#define PWM_COUNT 0x08
#define PWM_S     0x10
#define PWM_CMP0  0x20
#define PWM_CMP1  0x24
#define PWM_CMP2  0x28
#define PWM_CMP3  0x2C

/* Constants */

#define PWM_CFG_SCALE       0x0000000F
#define PWM_CFG_STICKY      0x00000100
#define PWM_CFG_ZEROCMP     0x00000200
#define PWM_CFG_DEGLITCH    0x00000400
#define PWM_CFG_ENALWAYS    0x00001000
#define PWM_CFG_ONESHOT     0x00002000
#define PWM_CFG_CMP0CENTER  0x00010000
#define PWM_CFG_CMP1CENTER  0x00020000
#define PWM_CFG_CMP2CENTER  0x00040000
#define PWM_CFG_CMP3CENTER  0x00080000
#define PWM_CFG_CMP0GANG    0x01000000
#define PWM_CFG_CMP1GANG    0x02000000
#define PWM_CFG_CMP2GANG    0x04000000
#define PWM_CFG_CMP3GANG    0x08000000
#define PWM_CFG_CMP0IP      0x10000000
#define PWM_CFG_CMP1IP      0x20000000
#define PWM_CFG_CMP2IP      0x40000000
#define PWM_CFG_CMP3IP      0x80000000

#define PWM_OK              0
#define PWM_ERR_NV          1
#define PWM_ERR_TAKEN       2
#define PWM_ERR_SCALE       3
#define PWM_ERR_PERIOD      4
#define PWM_ERR_PULSE       5
#define PWM_ERR_DUTY        6

#define PWM_MAX_PIN         3    
#define PWM_MAX             4     

struct pwm {

    unsigned int  pwm_num  = PWM_MAX;
    unsigned char scale    = 0b0000;
    unsigned int  period   = 0;
    unsigned int  pulse[3] = {0, 0, 0};

};

typedef enum
{
    PWM_EN_ALWAYS  = 0,
    PWM_EN_ONESHOT = 1,
} pwm_mode_t;

typedef enum
{
    PWM_ZEROCMP_EN  = 1,
    PWM_ZEROCMP_DIS = 0,
} pwm_zerocmp_t;

typedef enum
{
    PWM_PHCORR_EN = 1,
    PWM_PHCORR_DIS = 0,
} pwm_phcorr_t;

int pwm_init(struct pwm p, unsigned int pin, unsigned char scale);

int pwm_set_period(struct pwm p, unsigned int period, pwm_zerocmp_t zerocmp, pwm_mode_t mode);

int pwm_set_pulsewidth(struct pwm p, unsigned int pin, unsigned int pulse);

int pwm_set_duty(struct pwm p, unsigned int pin, unsigned int duty);

#endif /* _SIFIVE_PWM_H */
