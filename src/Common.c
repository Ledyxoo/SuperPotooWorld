#include "Common.h"

char *SPW_ResolveAssetPath(const char *relativePath)
{
    assert(relativePath);

    char *basePath = SDL_GetBasePath();
    const char *prefix = basePath ? basePath : "./";
    size_t prefixLen = strlen(prefix);
    size_t suffixLen = strlen(relativePath);

    char *resolvedPath = calloc(prefixLen + suffixLen + 1, sizeof(char));
    AssertNew(resolvedPath);

    memcpy(resolvedPath, prefix, prefixLen);
    memcpy(resolvedPath + prefixLen, relativePath, suffixLen);

    SDL_free(basePath);
    return resolvedPath;
}

RE_Timer *g_time = NULL;
Progress g_progress = { 0 };
LevelData g_levelData[LEVEL_COUNT] = { 0 };

void Progress_Init()
{
    g_progress.fireflyCount = 0;
    g_progress.lifeCount = 5;

    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        g_progress.levels[i].sucessful = false;
    }
}

void LevelData_Init()
{
    int i = 0;
    g_levelData[i].title = u8"Montagnes pointues";
    g_levelData[i].theme = THEME_MOUNTAINS;
    g_levelData[i].path = SPW_ResolveAssetPath("../Assets/Level/Level01.txt");
    i++;

    g_levelData[i].title = u8"Lac Bleu";
    g_levelData[i].theme = THEME_LAKE;
    g_levelData[i].path = SPW_ResolveAssetPath("../Assets/Level/Level02.txt");
    i++;

    g_levelData[i].title = u8"Bateaux volants";
    g_levelData[i].theme = THEME_SKY;
    g_levelData[i].path = SPW_ResolveAssetPath("../Assets/Level/Level03.txt");
    i++;

    g_levelData[i].title = u8"Volcan de la Mort";
    g_levelData[i].theme = THEME_VOLCAN;
    g_levelData[i].path = SPW_ResolveAssetPath("../Assets/Level/Volcan.txt");
    i++;

    g_levelData[i].title = u8"Démo";
    g_levelData[i].theme = THEME_LAKE;
    g_levelData[i].path = SPW_ResolveAssetPath("../Assets/Level/LevelDemo.txt");
    i++;

   

    
}
