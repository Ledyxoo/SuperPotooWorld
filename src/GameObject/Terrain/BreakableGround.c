#include "BreakableGround.h"
#include "Scene/LevelScene.h"

// Object virtual methods
void BreakableGround_VM_Destructor(void* self);

// GameObject virtual methods
void BreakableGround_VM_Start(void* self);
void BreakableGround_VM_Render(void* self);
void BreakableGround_VM_OnRespawn(void* self);
void BreakableGround_VM_Update(void* self);


void BreakableGround_OnCollisionEnter(PE_Collision* collision);
static BreakableGroundClass _Class_BreakableGround = { 0 };
const void* const Class_BreakableGround = &_Class_BreakableGround;

void Class_InitBreakableGround()
{
    if (!Class_IsInitialized(Class_BreakableGround))
    {
        Class_InitGameBody();

        void* self = (void*)Class_BreakableGround;
        ClassCtorParams params = {
            .self = self,
            .super = Class_GameBody,
            .name = "BreakableGround",
            .instanceSize = sizeof(BreakableGround),
            .classSize = sizeof(BreakableGroundClass)
        };
        Class_Constructor(params, BreakableGround_VM_Destructor);
        ((GameObjectClass*)self)->Start = BreakableGround_VM_Start;
        ((GameObjectClass*)self)->Render = BreakableGround_VM_Render;
        ((GameObjectClass*)self)->Update = BreakableGround_VM_Update;
        ((GameObjectClass*)self)->OnRespawn = BreakableGround_VM_OnRespawn;

    }
}

void BreakableGround_VM_OnRespawn(void* self)
{
   
    BreakableGround* breakeableground = Object_Cast(self, Class_BreakableGround);
    Scene* scene = GameObject_GetScene(self);
    breakeableground->timetop = 0;
    breakeableground->ontop = BREAKABLE_IDLE;
    Scene_EnableObject(scene, breakeableground);

    RE_Animator_StopAnimations(breakeableground->m_animator);
    RE_Animator_PlayAnimation(breakeableground->m_animator, "Breakable1");
}

void BreakableGround_CreateAnimator(BreakableGround* breakeableground, void* scene)
{
    AssetManager* assets = Scene_GetAssetManager(scene);
    RE_Atlas* atlas = AssetManager_GetTerrainAtlas(assets);
    RE_AtlasPart* part = NULL;
    void* anim = NULL;

    RE_Animator* animator = RE_Animator_New();
    AssertNew(animator);

   breakeableground->m_animator = animator;
    // Animation "BreakableGround1"
    part = RE_Atlas_GetPart(atlas, "BreakableGround1");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Breakable1", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);

    // Animation "BreakableGround2"
    part = RE_Atlas_GetPart(atlas, "BreakableGround2");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Breakable2", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);

    // Animation "BreakableGround3"
    part = RE_Atlas_GetPart(atlas, "BreakableGround3");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Breakable3", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);

    
}

void BreakableGround_Constructor(void* self, void* scene, PE_Vec2 startPos)
{

    GameBody_Constructor(self, scene, LAYER_TERRAIN);
    Object_SetClass(self, Class_BreakableGround);

    BreakableGround* breakableground = Object_Cast(self, Class_BreakableGround);
    
    breakableground->m_animator = NULL;

    GameBody_SaveStartPosition(breakableground, startPos);
    BreakableGround_CreateAnimator(breakableground, scene);
}

void BreakableGround_OnCollisionEnter(PE_Collision* collision)
{
    PE_Manifold manifold = PE_CollisionPair_GetManifold(PE_Collision_GetCollisionPair(collision));
    PE_Body* thisBody = PE_Collision_GetBody(collision);
    PE_Collider* otherCollider = PE_Collision_GetOtherCollider(collision);
    BreakableGround* breakableground = (BreakableGround*)GameBody_GetFromBody(thisBody);

    if (PE_Collider_CheckCategory(otherCollider, FILTER_PLAYER))
    {
        
        LevelScene* scene = Object_Cast(GameObject_GetScene(breakableground), Class_LevelScene);
        
        if ((thisBody->m_position.y < otherCollider->m_body->m_position.y))
        {
            breakableground->ontop = BREAKABLE_ONTOP;
            
             
            
                
        }
    }
}

void BreakableGround_VM_Start(void* self)
{
    BreakableGround* breakableground = Object_Cast(self, Class_BreakableGround);
    Scene* scene = GameObject_GetScene(breakableground);
    PE_World* world = Scene_GetWorld(scene);
    PE_Body* body = NULL;
    PE_BodyDef bodyDef = { 0 };
    PE_ColliderDef colliderDef = { 0 };
    PE_Collider* collider = NULL;
    Scene_EnableObject(scene, breakableground);
    
    // Cr�e le corps
    PE_BodyDef_SetDefault(&bodyDef);
    bodyDef.type = PE_STATIC_BODY;
    bodyDef.position = GameBody_GetStartPosition(breakableground);
    bodyDef.name = "BreakableGround1";

    body = PE_World_CreateBody(world, &bodyDef);
    AssertNew(body);

    PE_ColliderDef_SetDefault(&colliderDef);
    colliderDef.filter.categoryBits = FILTER_TERRAIN;
    colliderDef.isTrigger = false; //Action de la hitbox
    PE_Shape_SetAsBox(&colliderDef.shape, -0.5f, 0.0f, 0.5f, 1.0f);

    collider = PE_Body_CreateCollider(body, &colliderDef);
    AssertNew(collider);

    PE_Collider_SetOnCollisionEnter(collider, BreakableGround_OnCollisionEnter);

    GameBody_SetBody(self, body);
    Scene_SetToRespawn(scene, breakableground, true);

    // Joue l'animation par d�faut
    RE_Animator_PlayAnimation(breakableground->m_animator, "Breakable1");
}

void BreakableGround_VM_Destructor(void* self)
{
    BreakableGround* breakableground = Object_Cast(self, Class_BreakableGround);

    RE_Animator_Delete(breakableground->m_animator);

    // Destructeur de la classe m�re
    Object_SuperDestroy(self, Class_BreakableGround);
}

void BreakableGround_VM_Render(void* self)
{
    BreakableGround* breakableground = Object_Cast(self, Class_BreakableGround);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);
    PE_Vec2 position = GameBody_GetPosition(breakableground);
    

    SDL_FRect dst = { 0 };
    Camera_WorldToView(camera, position, &(dst.x), &(dst.y));
    float scale = Camera_GetWorldToViewScale(camera);
    dst.w = 1.0f * scale;
    dst.h = 1.0f * scale;

    RE_Animator_RenderCopyF(
        breakableground->m_animator, renderer, &dst, RE_ANCHOR_CENTER | RE_ANCHOR_BOTTOM
    );
}

void BreakableGround_VM_Update(void* self)
{
    BreakableGround* breakableground = Object_Cast(self, Class_BreakableGround);
    Scene* scene = GameObject_GetScene(self);
    RE_Animator_Update(breakableground->m_animator, g_time);
    if (breakableground->ontop == BREAKABLE_ONTOP)
    {
        breakableground->timetop += RE_Timer_GetDelta(g_time);
        
       
        if (breakableground->timetop > 0.5)
        {

           
            RE_Animator_PlayAnimation(breakableground->m_animator, "Breakable2");
        }
        if (breakableground->timetop > 1.5)
        {
            RE_Animator_PlayAnimation(breakableground->m_animator, "Breakable3");
            
        }
        if (breakableground->timetop > 2.5)
        {
            Scene_DisableObject(scene, breakableground);

        }
       

       
    }
    
    
    

    
}
