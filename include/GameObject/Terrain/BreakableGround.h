#ifndef _BREAKABLEGROUND_H_
#define _BREAKABLEGROUND_H_

#include "../GameBody.h"

void Class_InitBreakableGround();

/// @brief Description de la classe "Brick".
extern const void* const Class_BreakableGround;

typedef struct BreakableGroundClass_s {
    const GameBodyClass base;
} BreakableGroundClass;

typedef enum BreakableState_e
{
    BREAKABLE_IDLE,
    BREAKABLE_ONTOP
    
} BreakableState;

typedef struct BreakableGround_s
{
    struct {
        GameBody base;
    } m_super;
    bool m_isActive;
    float timetop;
    int ontop;
    RE_Animator* m_animator;
} BreakableGround;

void BreakableGround_Constructor(void* self, void* scene, PE_Vec2 startPos);

#endif
