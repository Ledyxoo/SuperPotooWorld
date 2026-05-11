#include "Demon.h"
#include "Scene/LevelScene.h"
#include "../../Utils/Renderer.h"
#include "SDL_mixer.h"



// Object virtual methods
void Demon_VM_Destructor(void* self);

// GameObject virtual methods
void Demon_VM_DrawGizmos(void* self);
void Demon_VM_FixedUpdate(void* self);
void Demon_VM_OnRespawn(void* self);
void Demon_VM_Render(void* self);
void Demon_VM_Start(void* self);
void Demon_VM_Update(void* self);

// Enemy virtual methods
void Demon_VM_Damage(void* self, void* damager);


// Callbacks de collisions
void Demon_OnCollisionStay(PE_Collision* collision);

static DemonClass _Class_Demon = { 0 };
const void* const Class_Demon = &_Class_Demon;

int i = 0;

void Class_InitDemon()
{
    if (!Class_IsInitialized(Class_Demon))
    {
        
        Class_InitEnemy();

        void* self = (void*)Class_Demon;
        ClassCtorParams params = {
            .self = self,
            .super = Class_Enemy,
            .name = "Demon",
            .instanceSize = sizeof(Demon),
            .classSize = sizeof(DemonClass)
        };
        Class_Constructor(params, Demon_VM_Destructor);
        ((GameObjectClass*)self)->DrawGizmos = Demon_VM_DrawGizmos;
        ((GameObjectClass*)self)->FixedUpdate = Demon_VM_FixedUpdate;
        ((GameObjectClass*)self)->OnRespawn = Demon_VM_OnRespawn;
        ((GameObjectClass*)self)->Render = Demon_VM_Render;
        ((GameObjectClass*)self)->Start = Demon_VM_Start;
        ((GameObjectClass*)self)->Update = Demon_VM_Update;
        ((EnemyClass*)self)->Damage = Demon_VM_Damage;
    }
}
void Demon_CreateAnimator(Demon* demon, void* scene)
{

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
    {
        printf("%s", Mix_GetError());
    }
    Mix_Music* musique; //Cr�ation du pointeur de type Mix_Music
    musique = Mix_LoadMUS("Boss.mp3"); //Chargement de la musique
    Mix_PlayMusic(musique, -1); //Jouer infiniment la musique

    AssetManager* assets = Scene_GetAssetManager(scene);
    RE_Atlas* atlas = AssetManager_GetDemonAtlas(assets);
    void* anim = NULL;

    // Cr�e l'animateur
    RE_Animator* animator = RE_Animator_New();
    AssertNew(animator);
    RE_AtlasPart* part = NULL;
    demon->m_animator = animator;

    // Animation "Idle"
    part = RE_Atlas_GetPart(atlas, "DemonIdle");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Idle", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, -1);
   

    // Animation "Walk"
    part = RE_Atlas_GetPart(atlas, "DemonWalk");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Walk", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, -1);

    // Animation "Cleave"
    part = RE_Atlas_GetPart(atlas, "DemonCleave");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Cleave", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, -1);

    // Animation "TakeHit"
    part = RE_Atlas_GetPart(atlas, "DemonTakeHit");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "TakeHit", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 1);

    // Animation "DemonDying"
    part = RE_Atlas_GetPart(atlas, "DemonDeath");
    AssertNew(part);

    anim = RE_Animator_CreateTextureAnim(animator, "Death", part);
    AssertNew(anim);
    RE_Animation_SetCycleCount(anim, 1);
   
}

void Demon_Constructor(void* self, void* scene, PE_Vec2 startPos)
{
    Enemy_Constructor(self, scene, startPos, 1);
    Object_SetClass(self, Class_Demon);

    i = 0;

    Demon* demon = Object_Cast(self, Class_Demon);

    demon->m_hDirection = -1.0f;
    demon->m_facingRight = false;

    
    demon->m_state = DEMON_IDLE;

    Demon_CreateAnimator(demon, scene);
    Scene_SetToRespawn(scene, self, true);
}
void Demon_VM_Start(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    Scene* scene = GameObject_GetScene(demon);
    PE_World* world = Scene_GetWorld(scene);
    PE_Body* body = NULL;
    PE_BodyDef bodyDef = { 0 };
    PE_ColliderDef colliderDef = { 0 };
    PE_Collider* collider = NULL;

    // Cr�e le corps
    PE_BodyDef_SetDefault(&bodyDef);
    bodyDef.type = PE_DYNAMIC_BODY;
    bodyDef.position = GameBody_GetStartPosition(demon);
    bodyDef.name = "Demon";
    bodyDef.xDamping = 0.0f;
    bodyDef.yDamping = 0.0f;
    bodyDef.mass = 1.0f;

    body = PE_World_CreateBody(world, &bodyDef);
    AssertNew(body);

    PE_ColliderDef_SetDefault(&colliderDef);
    colliderDef.friction = 0.005f;
    colliderDef.filter.categoryBits = FILTER_ENEMY;
    colliderDef.filter.maskBits = FILTER_TERRAIN | FILTER_PLAYER | FILTER_ENEMY;
    //Hitbox
    PE_Shape_SetAsBox(&colliderDef.shape, -1.0f, 0.0f, 1.0f, 3.0f);
    
   
    collider = PE_Body_CreateCollider(body, &colliderDef);
    AssertNew(collider);

    PE_Collider_SetOnCollisionStay(collider, Demon_OnCollisionStay);

    GameBody_SetBody(self, body);

    // Endort le corps
    // Permet d'optimiser le calcul de la physique,
    // seuls les corps proches du joueur sont simul�s
    PE_Body_SetAwake(body, false);

    // Joue l'animation par d�faut
    RE_Animator_PlayAnimation(demon->m_animator, "Idle");
}

void Demon_VM_Damage(void* self, void* damager)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    Scene* scene = GameObject_GetScene(demon);
    if (demon->m_state != DEMON_DEATH)
    {
        i++;
        
        demon->m_state = DEMON_TAKEHIT;
        RE_Animator_PlayAnimation(demon->m_animator, "TakeHit");

        if (Object_IsA(damager, Class_Player))
        {
            Player_Bounce(damager);
        }
        if (i == 10)
        {
            Mix_HaltMusic(); //Arr�te la musique
            demon->m_state = DEMON_DEATH;
            RE_Animator_PlayAnimation(demon->m_animator, "Death");
            printf("Bravo, vous avez termine Super Potoo World !");
        }
    }
    
    
    
   
}

void Demon_VM_Destructor(void* self)
{
    // Destructeur de la classe m�re
    Object_SuperDestroy(self, Class_Demon);
}

void Demon_OnCollisionStay(PE_Collision* collision)
{
    PE_Manifold manifold = PE_Collision_GetManifold(collision);
    PE_Body* thisBody = PE_Collision_GetBody(collision);
    PE_Body* otherBody = PE_Collision_GetOtherBody(collision);
    PE_Collider* otherCollider = PE_Collision_GetOtherCollider(collision);

    GameBody* thisGameBody = GameBody_GetFromBody(thisBody);
    GameBody* otherGameBody = GameBody_GetFromBody(otherBody);
    Demon* demon = Object_Cast(thisGameBody, Class_Demon);

    

    // Collision avec le joueur
    if (PE_Collider_CheckCategory(otherCollider, FILTER_PLAYER))
    {
        if (demon->m_state == DEMON_DEATH)
        {
            PE_Collision_SetEnabled(collision, false);
        }
       
        Player* player = Object_Cast(otherGameBody, Class_Player);
        PE_Vec2 velocity = GameBody_GetVelocity(demon);

        float angle = PE_Vec2_AngleDeg(manifold.normal, PE_Vec2_Down);
        if (angle > 55.0f && (demon->m_state == DEMON_IDLE || demon->m_state == DEMON_WALK || demon->m_state == DEMON_CLEAVE) && demon->m_state != DEMON_DEATH)
        {   
            Player_Damage(player);
            velocity = PE_Vec2_Set(1.0f, 0.f);
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

void Demon_VM_DrawGizmos(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);

    PE_Vec2 position = GameBody_GetPosition(demon);
    PE_Vec2 velocity = GameBody_GetVelocity(demon);

    // Dessine en blanc le vecteur vitesse du joueur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    Renderer_DrawVector(renderer, camera, position, velocity);
}

void Demon_VM_FixedUpdate(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    PE_Body* body = GameBody_GetBody(self);
    PE_Vec2 position = PE_Body_GetPosition(body);
    PE_Vec2 velocity = PE_Body_GetLocalVelocity(body);
   
   

   

    if (PE_Body_IsAwake(body) == false)
    {

        // Ne met pas � jour le boss si elle est endormie
        // Le joueur est loin d'elle et elle n'est plus visible par la cam�ra
        return;
    }

    // Tue le d�mon si elle tombe dans un trou
    if (position.y < -2.0f)
    {
        Scene* scene = GameObject_GetScene(self);
        Scene_DisableObject(scene, self);
        return;
    }

    Scene* scene = GameObject_GetScene(self);
    Player* player = LevelScene_GetPlayer(scene);
    PE_Vec2 playerPos = GameBody_GetPosition(player);

    // Calcule la distance entre le joueur et le boss
    float dist = PE_Vec2_Distance(position, playerPos);
   
    if (demon->m_hDirection == -1.0f)
    {

        demon->m_facingRight = true;
       
    }
    else if (demon->m_hDirection == 1.0f)
    {
        demon->m_facingRight = false;
       

    }

    if (dist > 35.0f)
    {
        // La distance entre de joueur et la noisette vient de d�passer 24 tuiles.
        // On endort la noisette pour ne plus la simuler dans le moteur physique.
       
        PE_Body_SetAwake(body, false);
        return;
    }
   
    if (i < 7)
    {
     if ((demon->m_state == DEMON_CLEAVE || demon->m_state == DEMON_WALK || demon->m_state == DEMON_TAKEHIT && demon->m_state != DEMON_DEATH)  && velocity.x == 0)
    {
        demon->m_state = DEMON_IDLE;
        RE_Animator_PlayAnimation(demon->m_animator, "Idle");
        
    }
    else if (dist <= 15.0f && demon->m_state == DEMON_IDLE && demon->m_state != DEMON_WALK && demon->m_state != DEMON_TAKEHIT && demon->m_state != DEMON_DEATH)
    {   
        if (playerPos.x < position.x)
        {

            demon->m_facingRight = true;
            demon->m_state = DEMON_WALK;
            RE_Animator_PlayAnimation(demon->m_animator, "Walk");

            velocity = PE_Vec2_Set(-5.0f, 0.0f);
        }
        else if (playerPos.x > position.x)
        {
            demon->m_facingRight = false;
            demon->m_state = DEMON_WALK;
            RE_Animator_PlayAnimation(demon->m_animator, "Walk");

            velocity = PE_Vec2_Set(5.0f, 0.0f);

        }

    }
    
    else if (dist <= 3.0f && (demon->m_state == DEMON_IDLE || demon->m_state == DEMON_WALK && demon->m_state != DEMON_CLEAVE && demon->m_state != DEMON_TAKEHIT && demon->m_state != DEMON_DEATH))
    {

        demon->m_state = DEMON_CLEAVE;
        RE_Animator_PlayAnimation(demon->m_animator, "Cleave");
    }

    }
    else if (i >= 7)
    {
        if ((demon->m_state == DEMON_CLEAVE || demon->m_state == DEMON_WALK || demon->m_state == DEMON_TAKEHIT && demon->m_state != DEMON_DEATH) && velocity.x == 0)
        {
            demon->m_state = DEMON_IDLE;
            RE_Animator_PlayAnimation(demon->m_animator, "Idle");

        }
        else if (dist <= 30.0f && demon->m_state == DEMON_IDLE && demon->m_state != DEMON_WALK && demon->m_state != DEMON_TAKEHIT && demon->m_state != DEMON_DEATH)
        {
            if (playerPos.x < position.x)
            {

                demon->m_facingRight = true;
                demon->m_state = DEMON_WALK;
                RE_Animator_PlayAnimation(demon->m_animator, "Walk");

                velocity = PE_Vec2_Set(-15.0f, 0.0f);
            }
            else if (playerPos.x > position.x)
            {
                demon->m_facingRight = false;
                demon->m_state = DEMON_WALK;
                RE_Animator_PlayAnimation(demon->m_animator, "Walk");

                velocity = PE_Vec2_Set(15.0f, 0.0f);

            }

        }

        else if (dist <= 3.0f && (demon->m_state == DEMON_IDLE || demon->m_state == DEMON_WALK && demon->m_state != DEMON_CLEAVE && demon->m_state != DEMON_TAKEHIT && demon->m_state != DEMON_DEATH))
        {

            demon->m_state = DEMON_CLEAVE;
            RE_Animator_PlayAnimation(demon->m_animator, "Cleave");
        }

    }
   

    PE_Body_SetVelocity(body, velocity);
}

void Demon_VM_OnRespawn(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    demon->m_state = DEMON_IDLE;
    i = 0;
    demon->timedeath = 0;
    GameBody_EnableBody(self);

    PE_Vec2 startPos = GameBody_GetStartPosition(self);
    PE_Body* body = GameBody_GetBody(self);
    PE_Body_SetPosition(body, startPos);
    PE_Body_SetVelocity(body, PE_Vec2_Zero);
    PE_Body_ClearForces(body);

    demon->m_state = PLAYER_IDLE;
    demon->m_hDirection = -1.0f;
    demon->m_facingRight = false;

    RE_Animator_StopAnimations(demon->m_animator);
    RE_Animator_PlayAnimation(demon->m_animator, "Idle");
}

void Demon_VM_Render(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    Scene* scene = GameObject_GetScene(self);
    SDL_Renderer* renderer = Scene_GetRenderer(scene);
    Camera* camera = Scene_GetActiveCamera(scene);

    int flip = demon->m_facingRight ? 0 : SDL_FLIP_HORIZONTAL;

    PE_Body* body = GameBody_GetBody(self);
    PE_Vec2 position = GameBody_GetPosition(self);

    float scale = Camera_GetWorldToViewScale(camera);
    SDL_FRect rect = { 0 };
    rect.h = 4.5f * scale; // Le sprite fait 1 tuile de haut
    rect.w = 2.5f * scale; // Le sprite fait 1 tuile de large
    Camera_WorldToView(camera, position, &rect.x, &rect.y);

    Vec2 center = Vec2_Set(0.5f, 0.5f);
    RE_Animator_RenderCopyExF(
        demon->m_animator, renderer, &rect, RE_ANCHOR_CENTER | RE_ANCHOR_BOTTOM,
        0.0f, center, flip
    );
}

void Demon_VM_Update(void* self)
{
    Demon* demon = Object_Cast(self, Class_Demon);
    PE_Body* body = GameBody_GetBody(self);
    PE_Vec2 position = PE_Body_GetPosition(body);
    PE_Vec2 velocity = PE_Body_GetLocalVelocity(body);
    Scene* scene = GameObject_GetScene(self);
    RE_Animator_Update(demon->m_animator, g_time);

   
    if (demon->m_state == DEMON_DEATH)
    {
        demon->timedeath += RE_Timer_GetDelta(g_time);
       
        if (demon->timedeath > 0.9)
        {
            LevelScene *lvlScene = Object_Cast(scene, Class_LevelScene);
            int levelIdx = LevelScene_GetLevelIndex(lvlScene);

            Scene_DisableObject(scene, demon);
            g_progress.levels[levelIdx].sucessful = true;

            Scene_Quit(scene);
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
            {
                printf("%s", Mix_GetError());
            }
            Mix_Music* musique; //Cr�ation du pointeur de type Mix_Music
            musique = Mix_LoadMUS("MainTheme.mp3"); //Chargement de la musique
            Mix_PlayMusic(musique, -1); //Jouer infiniment la musique
        }
    }
    if (velocity.x > 0)
    {
        demon->m_hDirection = 1.0f;
    }
    else if (velocity.x < 0)
    {
        demon->m_hDirection = -1.0f;
    }
   
    
}

