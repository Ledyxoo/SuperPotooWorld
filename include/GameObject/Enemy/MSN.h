#ifndef _MSN_H_
#define _MSN_H_

#include "Enemy.h"

//MSN = MEGA SUPER MSN !!!!

void Class_InitMSN();

/// @brief Description de la classe "MSN".
extern const void* const Class_MSN;

typedef struct MSNClass_s {
    const EnemyClass base;
} MSNClass;

typedef enum MSNState_e
{
    MSN_IDLE,
    MSN_SPINNING,
    MSN_DYING,
    MSN_JUMP
} MSNState;

typedef struct MSN_s
{
    struct {
        Enemy base;
    } m_super;

    RE_Animator* m_animator;
    float timedeath;
    int m_state;
    float timejump;

} MSN;

void MSN_Constructor(void* self, void* scene, PE_Vec2 startPos);

#endif
