#include "MSN.h"
#include "Scene/LevelScene.h"
#include "../../Utils/Renderer.h"


// Object virtual methods
void MSN_VM_Destructor(void* self);

// GameObject virtual methods
void MSN_VM_DrawGizmos(void* self);
void MSN_VM_FixedUpdate(void* self);
void MSN_VM_OnRespawn(void* self);
void MSN_VM_Render(void* self);
void MSN_VM_Start(void* self);
void MSN_VM_Update(void* self);

// Enemy virtual methods
void MSN_VM_Damage(void* self, void* damager);


// Callbacks de collisions
void MSN_OnCollisionStay(PE_Collision* collision);

static MSNClass _Class_MSN = { 0 };
const void* const Class_MSN = &_Class_MSN;

void Class_InitMSN()
{
    if (!Class_IsInitialized(Class_MSN))
    {
        Class_InitEnemy();

        void* self = (void*)Class_MSN;
        ClassCtorParams params = {
            .self = self,
            .super = Class_Enemy,
            .name = "MSN",
            .instanceSize = sizeof(MSN),
            .classSize = sizeof(MSNClass)
        };
        Class_Constructor(params, MSN_VM_Destructor);
        ((GameObjectClass*)self)->DrawGizmos = MSN_VM_DrawGizmos;
        ((GameObjectClass*)self)->FixedUpdate = MSN_VM_FixedUpdate;
        ((GameObjectClass*)self)->OnRespawn = MSN_VM_OnRespawn;
        ((GameObjectClass*)self)->Render = MSN_VM_Render;
        ((GameObjectClass*)self)->Start = MSN_VM_Start;
        ((GameObjectClass*)self)->Update = MSN_VM_Update;
        ((EnemyClass*)self)->Damage = MSN_VM_Damage;
    }
}
void MSN_CreateAnimator(MSN* MSN, void* scene)
{
    AssetManager* assets = Scene_GetAssetManager(scene);
    RE_Atlas* atlas = AssetManager_GetMSNAtlas(assets);
    void* anim = NULL;

    // Cr�e l'animateur
    RE_Animator* animator = RE_Animator_New();
    AssertNew(animator);

    RE_AtlasPart* part = NULL;

    MSN->m_animator = animator;

    // Animation "Idle"
    part = RE_Atlas_GetPart(atlas, "MSNIdle");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Idle", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);

    // Animation "Spinning"
    part = RE_Atlas_GetPart(atlas, "MSNSpinning");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Spinning", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, -1);
    RE_Animation_SetCycleTime(anim, 0.2f);

    // Animation "MSNDying"
    part = RE_Atlas_GetPart(atlas, "MSNDying");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Dying", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, -1);
    RE_Animation_SetCycleTime(anim, 0.2f);
}

void MSN_Constructor(void* self, void* scene, PE_Vec2 startPos)
{
    Enemy_Constructor(self, scene, startPos, 1);
    Object_SetClass(self, Class_MSN);

    MSN* MSN = Object_Cast(self, Class_MSN);
    MSN->m_state = MSN_IDLE;

    MSN_CreateAnimator(MSN, scene);
    Scene_SetToRespawn(scene, self, true);
}

void MSN_VM_Start(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    Scene* scene = GameObject_GetScene(MSN);
    PE_World* world = Scene_GetWorld(scene);
    PE_Body* body = NULL;
    PE_BodyDef bodyDef = { 0 };
    PE_ColliderDef colliderDef = { 0 };
    PE_Collider* collider = NULL;

    // Cr�e le corps
    PE_BodyDef_SetDefault(&bodyDef);
    bodyDef.type = PE_DYNAMIC_BODY;
    bodyDef.position = GameBody_GetStartPosition(MSN);
    bodyDef.name = "MSN";
    bodyDef.xDamping = 0.0f;
    bodyDef.yDamping = 0.0f;
    bodyDef.mass = 1.0f;

    body = PE_World_CreateBody(world, &bodyDef);
    AssertNew(body);

    PE_ColliderDef_SetDefault(&colliderDef);
    colliderDef.friction = 0.002f;
    colliderDef.filter.categoryBits = FILTER_ENEMY;
    colliderDef.filter.maskBits = FILTER_TERRAIN | FILTER_PLAYER | FILTER_ENEMY;
    PE_Shape_SetAsCircle(
        &colliderDef.shape,
        PE_Vec2_Set(0.0f, 0.45f), 0.5f
    );

    collider = PE_Body_CreateCollider(body, &colliderDef);
    AssertNew(collider);

    PE_Collider_SetOnCollisionStay(collider, MSN_OnCollisionStay);

    GameBody_SetBody(self, body);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simul�s
    PE_Body_SetAwake(body, false);

    // Joue l'animation par d�faut
    RE_Animator_PlayAnimation(MSN->m_animator, "Idle");
}

void MSN_VM_Damage(void* self, void* damager)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    Scene* scene = GameObject_GetScene(MSN);

    if (Object_IsA(damager, Class_Player))
    {
        Player_Bounce(damager);
    }
    MSN->m_state = MSN_DYING;
    RE_Animator_PlayAnimation(MSN->m_animator, "Dying");
    //  Scene_DisableObject(scene, MSN);
}

void MSN_VM_Destructor(void* self)
{
    // Destructeur de la classe m�re
    Object_SuperDestroy(self, Class_MSN);
}

void MSN_OnCollisionStay(PE_Collision* collision)
{
    PE_Manifold manifold = PE_Collision_GetManifold(collision);
    PE_Body* thisBody = PE_Collision_GetBody(collision);
    PE_Body* otherBody = PE_Collision_GetOtherBody(collision);
    PE_Collider* otherCollider = PE_Collision_GetOtherCollider(collision);

    GameBody* thisGameBody = GameBody_GetFromBody(thisBody);
    GameBody* otherGameBody = GameBody_GetFromBody(otherBody);
    MSN* MSN = Object_Cast(thisGameBody, Class_MSN);

    // Collision avec le joueur
    if (PE_Collider_CheckCategory(otherCollider, FILTER_PLAYER))
    {
        Player* player = Object_Cast(otherGameBody, Class_Player);

        float angle = PE_Vec2_AngleDeg(manifold.normal, PE_Vec2_Down);
        if (angle > 55.0f)
        {
           
            Player_Damage(player);
            MSN->m_state = MSN_JUMP;
            RE_Animator_PlayAnimation(MSN->m_animator, "Dying");
        }
        return;
    }
    if (PE_Collider_CheckCategory(otherCollider, FILTER_ENEMY))
    {


        thisBody->m_localVelocity.x = -(thisBody->m_localVelocity.x);

        return;
    }
    if (PE_Collider_CheckCategory(otherCollider, FILTER_TERRAIN))
    {
        float angle = PE_Vec2_AngleDeg(manifold.normal, PE_Vec2_Down);
        if (angle == 90.0f)
        {
            thisBody->m_localVelocity.x = -(thisBody->m_localVelocity.x);
        }

        return;
    }
}

void MSN_VM_DrawGizmos(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);

    PE_Vec2 position = GameBody_GetPosition(MSN);
    PE_Vec2 velocity = GameBody_GetVelocity(MSN);

    // Dessine en blanc le vecteur vitesse du joueur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    Renderer_DrawVector(renderer, camera, position, velocity);
}

void MSN_VM_FixedUpdate(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    PE_Body* body = GameBody_GetBody(self);
    PE_Vec2 position = PE_Body_GetPosition(body);
    PE_Vec2 velocity = PE_Body_GetLocalVelocity(body);

    if (PE_Body_IsAwake(body) == false)
    {
        // Ne met pas � jour la noisette si elle est endormie
        // Le joueur est loin d'elle et elle n'est plus visible par la cam�ra
        return;
    }

    // Tue la noisette si elle tombe dans un trou
    if (position.y < -2.0f)
    {
        Scene* scene = GameObject_GetScene(self);
        Scene_DisableObject(scene, self);
        return;
    }



    Scene* scene = GameObject_GetScene(self);
    Player* player = LevelScene_GetPlayer(scene);
    PE_Vec2 playerPos = GameBody_GetPosition(player);

    // Calcule la distance entre le joueur et la noisette
    float dist = PE_Vec2_Distance(position, playerPos);
    //Cours sur le joueur comme des Goomba
    
   
    if (dist > 24.0f)
    {
        // La distance entre de joueur et la noisette vient de d�passer 24 tuiles.
        // On endort la noisette pour ne plus la simuler dans le moteur physique.
        PE_Body_SetAwake(body, false);
        return;
        
    }

    if (velocity.x == 0 && dist <= 5.0f && (position.x < playerPos.x) && MSN->m_state == MSN_SPINNING)
    {
        // Le joueur est � moins de 5 tuiles de la noisette
        MSN->m_state = MSN_SPINNING;
        RE_Animator_PlayAnimation(MSN->m_animator, "Spinning");
        // LevelScene_SetStartPosition(scene, MSN->m_position);
        velocity = PE_Vec2_Set(1.5f, 0.0f);
    }
    if (velocity.x == 0 && dist <= 5.0f && (position.x > playerPos.x) && MSN->m_state == MSN_SPINNING)
    {
        // Le joueur est � moins de 5 tuiles de la noisette
       
        RE_Animator_PlayAnimation(MSN->m_animator, "Spinning");
        // LevelScene_SetStartPosition(scene, MSN->m_position);
        velocity = PE_Vec2_Set(-1.5f, 0.0f);
    }

    else if (dist <= 5.0f && (position.x > playerPos.x) && MSN->m_state == MSN_IDLE)
    {
        // Le joueur est � moins de 5 tuiles de la noisette
       
        RE_Animator_PlayAnimation(MSN->m_animator, "Spinning");
        // LevelScene_SetStartPosition(scene, MSN->m_position);
        velocity = PE_Vec2_Set(-6.f, 10.f);
    }
    else if (dist <= 5.0f && (position.x < playerPos.x) && MSN->m_state == MSN_IDLE)
    {
        // Le joueur est � moins de 5 tuiles de la noisette
        MSN->m_state = MSN_SPINNING;
        RE_Animator_PlayAnimation(MSN->m_animator, "Spinning");
        // LevelScene_SetStartPosition(scene, MSN->m_position);
        velocity = PE_Vec2_Set(6.f, 10.f);
    }
    else if (position.x > playerPos.x && dist > 5.0f)
    {
        velocity = PE_Vec2_Set(-1.5f, 0.0f);
    }
    else if (position.x < playerPos.x && dist > 5.0f)
    {
        velocity = PE_Vec2_Set(1.5f, 0.0f);
    }
  
       
    PE_Body_SetVelocity(body, velocity);
}

void MSN_VM_OnRespawn(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    MSN->m_state = MSN_IDLE;

    GameBody_EnableBody(self);

    PE_Vec2 startPos = GameBody_GetStartPosition(self);
    PE_Body* body = GameBody_GetBody(self);
    PE_Body_SetPosition(body, startPos);
    PE_Body_SetVelocity(body, PE_Vec2_Zero);
    PE_Body_ClearForces(body);

    RE_Animator_StopAnimations(MSN->m_animator);
    RE_Animator_PlayAnimation(MSN->m_animator, "Idle");
}

void MSN_VM_Render(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);

    PE_Body* body = GameBody_GetBody(self);
    PE_Vec2 position = GameBody_GetPosition(self);

    float scale = Camera_GetWorldToViewScale(camera);
    SDL_FRect rect = { 0 };
    rect.h = 1.15f * scale; // Le sprite fait 1 tuile de haut
    rect.w = 1.15f * scale; // Le sprite fait 1 tuile de large
    Camera_WorldToView(camera, position, &rect.x, &rect.y);

    RE_Animator_RenderCopyF(
        MSN->m_animator, renderer, &rect, RE_ANCHOR_CENTER | RE_ANCHOR_BOTTOM
    );
}

void MSN_VM_Update(void* self)
{
    MSN* MSN = Object_Cast(self, Class_MSN);
    Scene* scene = GameObject_GetScene(self);
    RE_Animator_Update(MSN->m_animator, g_time);
    


    if (MSN->m_state == MSN_DYING || MSN->m_state == MSN_JUMP)
    {
        MSN->timedeath += RE_Timer_GetDelta(g_time);

        if (MSN->timedeath > 1)
        {
            Scene_DisableObject(scene, MSN);
        }
    }
    

}