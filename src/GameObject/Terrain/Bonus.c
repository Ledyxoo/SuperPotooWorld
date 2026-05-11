#include "Bonus.h"
#include "Scene/LevelScene.h"
#include "../Collectable/Heart.h"

// Object virtual methods
void Bonus_VM_Destructor(void *self);

// GameObject virtual methods
void Bonus_VM_Render(void *self);
void Bonus_VM_Start(void *self);
void Bonus_VM_Update(void *self);
void Bonus_VM_OnRespawn(void *self);

static BonusClass _Class_Bonus = { 0 };
const void *const Class_Bonus = &_Class_Bonus;

void Class_InitBonus()
{
    if (!Class_IsInitialized(Class_Bonus))
    {
        Class_InitGameBody();

        void *self = (void *)Class_Bonus;
        ClassCtorParams params = {
            .self = self,
            .super = Class_GameBody,
            .name = "Bonus",
            .instanceSize = sizeof(Bonus),
            .classSize = sizeof(BonusClass)
        };
        Class_Constructor(params, Bonus_VM_Destructor);
        ((GameObjectClass *)self)->OnRespawn = Bonus_VM_OnRespawn;
        ((GameObjectClass *)self)->Render = Bonus_VM_Render;
        ((GameObjectClass *)self)->Start = Bonus_VM_Start;
        ((GameObjectClass *)self)->Update = Bonus_VM_Update;
    }
}

void Bonus_VM_OnRespawn(void* self)
{
    Bonus* bonus = Object_Cast(self, Class_Bonus);
    Scene* scene = GameObject_GetScene(self);

    Scene_EnableObject(scene, bonus);

    RE_Animator_StopAnimations(bonus->m_animator);

    if (bonus->m_state == BONUS_ON)
    {
        RE_Animator_PlayAnimation(bonus->m_animator, "BonusFull");
    }

}

void Bonus_CreateAnimator(Bonus* bonus, void* scene)
{
    AssetManager* assets = Scene_GetAssetManager(scene);
    RE_Atlas* atlas = AssetManager_GetTerrainAtlas(assets);
    RE_AtlasPart* part = NULL;
    void* anim = NULL;

    RE_Animator* animator = RE_Animator_New();
    AssertNew(animator);

    bonus->m_animator = animator;
    // Animation "Bonus"
    part = RE_Atlas_GetPart(atlas, "BonusFull");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "BonusFull", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);

    // Animation "Bonus"
    part = RE_Atlas_GetPart(atlas, "BonusEmpty");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "BonusEmpty", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 0);


}

void Bonus_Constructor(void* self, void* scene, PE_Vec2 startPos)
{

    GameBody_Constructor(self, scene, LAYER_TERRAIN);
    Object_SetClass(self, Class_Bonus);

    Bonus* bonus = Object_Cast(self, Class_Bonus);
    // Bonus->m_isActive = false;
    bonus->m_animator = NULL;

    GameBody_SaveStartPosition(bonus, startPos);
    Bonus_CreateAnimator(bonus, scene);
}

void Bonus_OnCollisionEnter(PE_Collision* collision)
{
    PE_Manifold manifold = PE_CollisionPair_GetManifold(PE_Collision_GetCollisionPair(collision));
    PE_Body* thisBody = PE_Collision_GetBody(collision);
    PE_Collider* otherCollider = PE_Collision_GetOtherCollider(collision);
    Bonus* bonus= (Bonus*)GameBody_GetFromBody(thisBody);

    if (PE_Collider_CheckCategory(otherCollider, FILTER_PLAYER))
    {
        LevelScene* scene = Object_Cast(GameObject_GetScene(bonus), Class_LevelScene);
        int i = LevelScene_GetLevelIndex(scene);

        g_progress.levels[i].sucessful = true;
        if ((thisBody->m_position.y > otherCollider->m_body->m_position.y) && bonus->m_state == BONUS_ON)
        {
            RE_Animator_PlayAnimation(bonus->m_animator, "BonusEmpty");
            bonus->m_state = BONUS_OFF;


            int x = thisBody->m_position.x;
            int y = thisBody->m_position.y;

            Heart* heart = Scene_AllocateObject(scene, Class_Heart);
            AssertNew(heart);
            Heart_Constructor(heart, scene, PE_Vec2_Set((float)x+0.5, (float)y + 1.5f));


        }


    }
}

void Bonus_VM_Start(void* self)
{
    Bonus* bonus = Object_Cast(self, Class_Bonus);
    Scene* scene = GameObject_GetScene(bonus);
    PE_World* world = Scene_GetWorld(scene);
    PE_Body* body = NULL;
    PE_BodyDef bodyDef = { 0 };
    PE_ColliderDef colliderDef = { 0 };
    PE_Collider* collider = NULL;

    // Cr�e le corps
    PE_BodyDef_SetDefault(&bodyDef);
    bodyDef.type = PE_STATIC_BODY;
    bodyDef.position = GameBody_GetStartPosition(bonus);
    bodyDef.name = "BonusFull";

    body = PE_World_CreateBody(world, &bodyDef);
    AssertNew(body);

    PE_ColliderDef_SetDefault(&colliderDef);
    colliderDef.filter.categoryBits = FILTER_TERRAIN;
    colliderDef.isTrigger = false; //Action de la hitbox
    PE_Shape_SetAsBox(&colliderDef.shape, -0.5f, 0.0f, 0.5f, 1.0f);

    collider = PE_Body_CreateCollider(body, &colliderDef);
    AssertNew(collider);

    PE_Collider_SetOnCollisionEnter(collider, Bonus_OnCollisionEnter);

    GameBody_SetBody(self, body);
    Scene_SetToRespawn(scene, bonus, true);

    // Joue l'animation par d�faut
    RE_Animator_PlayAnimation(bonus->m_animator, "BonusFull");
}

void Bonus_VM_Destructor(void* self)
{
    Bonus *bonus = Object_Cast(self, Class_Bonus);

    RE_Animator_Delete(bonus->m_animator);

    // Destructeur de la classe m�re
    Object_SuperDestroy(self, Class_Bonus);
}

void Bonus_VM_Render(void* self)
{
    Bonus* bonus = Object_Cast(self, Class_Bonus);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);
    PE_Vec2 position = GameBody_GetPosition(bonus);

    SDL_FRect dst = { 0 };
    Camera_WorldToView(camera, position, &(dst.x), &(dst.y));
    float scale = Camera_GetWorldToViewScale(camera);
    dst.w = 1.0f * scale;
    dst.h = 1.0f * scale;

    RE_Animator_RenderCopyF(
        bonus->m_animator, renderer, &dst, RE_ANCHOR_CENTER | RE_ANCHOR_BOTTOM
    );
}

void Bonus_VM_Update(void* self)
{
    Bonus* bonus = Object_Cast(self, Class_Bonus);
    RE_Animator_Update(bonus->m_animator, g_time);
}