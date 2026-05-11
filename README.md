# Super Potoo World

Un jeu de plateforme 2D rétro développé en C, utilisant SDL2. Ce projet a été réalisé dans le cadre d'un projet d'école d'ingénieur.

## Fonctionnalités

- **Gameplay Classique** : Incarnez Potoo dans un monde de plateformes exigeant.
- **Ennemis Variés** : Affrontez des noisettes magiques, des démons et le redoutable MSN (Méga Super Nut).
- **Mondes Multiples** : Explorez les Montagnes, le Lac, les Bateaux Volants et le Volcan.
- **Moteur Physique Custom** : Utilise le `PlatformerEngine` pour des collisions et des mouvements fluides.
- **Système d'Animation** : Rendu dynamique via le `RenderingEngine`.

## Prérequis

Le projet utilise **vcpkg** pour la gestion des dépendances sur Windows et les paquets système sur Linux.

### Linux (Debian/Ubuntu)
Installez les bibliothèques de développement SDL2 et OpenMP :
```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libomp-dev
```

### Windows
1. Installez [CMake](https://cmake.org/download/).
2. Installez [vcpkg](https://github.com/microsoft/vcpkg).
3. Le projet téléchargera automatiquement les dépendances (SDL2, etc.) lors de la configuration CMake.

## Compilation

### Linux
```bash
mkdir build && cd build
cmake ..
make
```

### Windows (Visual Studio)
```powershell
mkdir build; cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[PATH_TO_VCPKG]/scripts/buildsystems/vcpkg.cmake
# Ouvrez ensuite le fichier .sln généré dans Visual Studio et compilez le projet.
```

## Intégration Continue
Ce projet utilise **GitHub Actions** pour vérifier la compilation sur Linux et Windows à chaque commit.

## Structure du Projet
- `src/` : Code source (`.c`).
- `include/` : Fichiers d'en-tête (`.h`).
- `Assets/` : Ressources (images, niveaux, polices).
- `external/` : Bibliothèques et moteurs tiers.

## Auteurs
- Killian de Saint Jores
- Baptiste Barbier--Fradin
