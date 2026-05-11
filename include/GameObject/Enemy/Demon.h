#ifndef _DEMON_H_
#define _DEMON_H_

#include "Enemy.h"

void Class_InitDemon();

/// @brief Description de la classe "Demon".
extern const void* const Class_Demon;

typedef struct DemonClass_s {
    const EnemyClass base;
} DemonClass;

typedef enum DemonState_e
{
    DEMON_IDLE,
    DEMON_WALK,
    DEMON_CLEAVE,
    DEMON_TAKEHIT,
    DEMON_DEATH,
    DEMON_DYING
} DEMONState;

typedef struct Demon_s
{
    struct {
        Enemy base;
    } m_super;

    RE_Animator* m_animator;
    
    float m_hDirection;
    bool m_facingRight;
        int m_state;
        float timedeath;

} Demon;

void Demon_Constructor(void* self, void* scene, PE_Vec2 startPos);

#endif
