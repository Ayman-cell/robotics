# X-Ibition 2025 Robodog 🤖🐕

Projet de robot chien (Robodog) développé pour X-Ibition 2025. Ce projet comprend une interface de contrôle Python moderne avec détection de gestes, navigation autonome, et contrôle Arduino pour les servomoteurs.

## 📋 Table des matières

- [Description](#description)
- [Fonctionnalités](#fonctionnalités)
- [Structure du projet](#structure-du-projet)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Configuration](#configuration)
- [Utilisation](#utilisation)
- [Architecture](#architecture)
- [Contrôles](#contrôles)
- [Dépannage](#dépannage)
- [Auteurs](#auteurs)
- [Licence](#licence)

## 🎯 Description

Ce projet implémente un système complet de contrôle pour un robot chien quadrupède. L'interface graphique permet de contrôler le robot via plusieurs méthodes :
- Contrôle manuel via l'interface graphique
- Détection de gestes avec la main (MediaPipe)
- Navigation autonome avec évitement d'obstacles (YOLO)
- Contrôle vocal (optionnel)

Le robot est contrôlé via une carte Arduino qui pilote 8 servomoteurs (2 par patte) à travers un driver PCA9685.

## ✨ Fonctionnalités

### Interface Graphique
- **Design Cyberpunk** : Interface moderne avec thème Matrix/cyberpunk
- **Contrôle des moteurs** : Sliders individuels pour chaque moteur (épaule/genou de chaque patte)
- **Contrôle de mouvement** : Boutons directionnels pour avancer, reculer, tourner
- **Visualisation** : Animation en temps réel de l'état du robot
- **Caméra** : Affichage du flux vidéo avec détection de gestes
- **Communication série** : Configuration et connexion série/Bluetooth

### Détection de Gestes
- **MediaPipe Hands** : Détection en temps réel des gestes de la main
- **Gestes supportés** :
  - ✋ **Main ouverte** : Arrêt du robot
  - ✊ **Poing fermé** : Reculer
  - ✌️ **Peace (V)** : Avancer
  - 👈 **Index gauche** : Tourner à gauche
  - 👉 **Index droit** : Tourner à droite

### Navigation Autonome
- **YOLO v8** : Détection d'objets en temps réel
- **Évitement d'obstacles** : Détection et évitement automatique
- **Navigation intelligente** : Calcul de trajectoire optimale

### Contrôle Arduino
- **8 Servomoteurs** : Contrôle précis de chaque articulation
- **Mouvements prédéfinis** : Avancer, reculer, tourner, position de repos
- **Contrôle de hauteur** : Ajustement dynamique de la hauteur du corps
- **Communication Bluetooth** : Support Bluetooth et série USB

## 📁 Structure du projet

```
X-Ibition 2025 Robodog/
│
├── INTERFACE 2025 EAC/
│   ├── INTERFACE_DOG.py          # Interface graphique principale
│   ├── Camera.py                  # Gestion de la caméra
│   ├── Hand_Detection.py          # Détection de gestes (MediaPipe)
│   ├── Navigation.py              # Navigation et évitement d'obstacles (YOLO)
│   ├── Serial.py                  # Communication série/Bluetooth
│   ├── matrix_effect.py           # Effets visuels Matrix
│   ├── testing.py                 # Scripts de test
│   ├── storage.txt                # Fichier de stockage
│   ├── yolov8n.pt                 # Modèle YOLO pré-entraîné
│   └── E-TECH logo.png            # Logo du projet
│
├── RobotDog/
│   └── RobotDog.ino               # Code Arduino pour le contrôle des servos
│
├── .gitignore                     # Fichiers ignorés par Git
└── README.md                      # Ce fichier
```

## 🔧 Prérequis

### Logiciels
- **Python 3.8+**
- **Arduino IDE** (pour compiler et uploader le code Arduino)
- **Git** (pour cloner le dépôt)

### Matériel
- **Arduino** (Uno/Nano/Mega)
- **PCA9685** (Driver PWM pour servomoteurs)
- **8 Servomoteurs** (ex: SG90 ou MG996R)
- **Module Bluetooth** (optionnel, pour contrôle sans fil)
- **Caméra USB** ou caméra IP
- **Carte SD** (optionnel, pour stockage)

### Bibliothèques Python
```bash
pip install customtkinter
pip install opencv-python
pip install mediapipe
pip install ultralytics
pip install numpy
pip install matplotlib
pip install pillow
pip install pyserial
```

### Bibliothèques Arduino
Installer via le gestionnaire de bibliothèques Arduino IDE :
- **Adafruit PWM Servo Driver Library**
- **SoftwareSerial** (incluse par défaut)
- **Wire** (incluse par défaut)

## 📦 Installation

1. **Cloner le dépôt**
```bash
git clone https://github.com/Ayman-cell/projects.git
cd projects
git checkout robotics
```

2. **Installer les dépendances Python**
```bash
cd "INTERFACE 2025 EAC"
pip install -r requirements.txt
```

3. **Configurer Arduino**
   - Ouvrir `RobotDog/RobotDog.ino` dans Arduino IDE
   - Installer les bibliothèques nécessaires
   - Sélectionner la carte et le port COM
   - Compiler et uploader le code

4. **Configurer la caméra**
   - Modifier l'IP dans `Camera.py` si vous utilisez une caméra IP
   - Ou connecter une caméra USB (index 0 par défaut)

## ⚙️ Configuration

### Configuration série/Bluetooth

Dans `INTERFACE_DOG.py`, modifier :
```python
self.DEFAULT_COM = 6  # Numéro du port COM (None pour auto-détection)
self.SERIAL_BAUD = 9600  # 9600 pour Bluetooth, 115200 pour USB
```

### Configuration de la caméra

Dans `Camera.py` :
```python
ip = '10.24.20.222'  # IP de la caméra (si caméra IP)
url = f'http://{ip}:8080/video'
```

### Configuration des servos

Dans `RobotDog.ino`, ajuster les offsets selon votre configuration :
```cpp
const int FR_shoulder_offset = 98;
const int FR_knee_offset = 90;
// ... etc
```

## 🚀 Utilisation

### Démarrage de l'interface

```bash
cd "INTERFACE 2025 EAC"
python INTERFACE_DOG.py
```

### Contrôle manuel

1. **Connexion série** :
   - Sélectionner le port COM et le baud rate
   - Cliquer sur "Connect"

2. **Contrôle des moteurs** :
   - Utiliser les sliders pour ajuster chaque moteur
   - Cliquer sur "SEND" pour envoyer la commande

3. **Mouvement** :
   - Utiliser les flèches directionnelles pour avancer/reculer/tourner
   - Ajuster la vitesse avec le slider "SPEED"
   - Cliquer sur "Stand Position" pour revenir à la position de repos

4. **Élévation** :
   - Utiliser les boutons +/- pour ajuster la hauteur du corps

### Contrôle par gestes

1. **Démarrer la caméra** :
   - Cliquer sur "Start Camera"
   - Positionner votre main devant la caméra

2. **Gestes disponibles** :
   - ✋ Main ouverte → Arrêt
   - ✊ Poing fermé → Reculer
   - ✌️ Peace (V) → Avancer
   - 👈 Index gauche → Tourner gauche
   - 👉 Index droit → Tourner droite

### Navigation autonome

La navigation autonome s'active automatiquement lorsque la caméra est démarrée. Le robot détecte les obstacles et ajuste sa trajectoire.

## 🏗️ Architecture

### Interface Python
- **INTERFACE_DOG.py** : Interface principale avec CustomTkinter
- **Camera.py** : Capture et traitement vidéo
- **Hand_Detection.py** : Détection de gestes avec MediaPipe
- **Navigation.py** : Navigation et évitement d'obstacles avec YOLO
- **Serial.py** : Communication série/Bluetooth

### Code Arduino
- **RobotDog.ino** : Contrôle des servomoteurs
  - Gestion des 8 servos via PCA9685
  - Mouvements prédéfinis (avancer, reculer, tourner)
  - Contrôle de hauteur dynamique
  - Communication série/Bluetooth

### Protocole de communication

Commandes Arduino :
- `s<id> <angle>` : Déplacer le servo `<id>` à l'angle `<angle>`
- `r` : Position de repos
- `a` : Avancer
- `d` : Reculer
- `t` : Tourner à gauche
- `y` : Tourner à droite
- `h` : Augmenter la hauteur
- `l` : Diminuer la hauteur

## 🎮 Contrôles

### Clavier
- **Flèches** : Contrôle directionnel
- **Espace** : Position de repos
- **+/-** : Ajuster l'élévation
- **Échap** : Mode erreur

### Interface graphique
- **Sliders** : Contrôle individuel des moteurs
- **Boutons directionnels** : Mouvement du robot
- **Slider vitesse** : Vitesse de déplacement
- **Boutons caméra** : Démarrer/arrêter la caméra

## 🔍 Dépannage

### Problème de connexion série
- Vérifier que le port COM est correct
- Vérifier le baud rate (9600 pour Bluetooth, 115200 pour USB)
- Vérifier que le câble USB/Bluetooth est bien connecté

### Problème de caméra
- Vérifier que la caméra est bien connectée
- Modifier l'index de la caméra dans `Camera.py`
- Pour une caméra IP, vérifier l'adresse IP et la connexion réseau

### Problème de détection de gestes
- S'assurer que la main est bien visible
- Améliorer l'éclairage
- Vérifier que MediaPipe est bien installé

### Problème de servomoteurs
- Vérifier les connexions électriques
- Vérifier l'alimentation (les servos consomment beaucoup)
- Ajuster les offsets dans `RobotDog.ino`

## 👥 Auteurs

- **Ayman** - Développement initial
- **E-TECH** - Support et logo

## 📄 Licence

Ce projet est développé pour X-Ibition 2025. Tous droits réservés.

## 🙏 Remerciements

- **MediaPipe** pour la détection de gestes
- **Ultralytics** pour YOLO
- **CustomTkinter** pour l'interface moderne
- **Adafruit** pour la bibliothèque PCA9685

---

**Note** : Ce projet est en développement actif. N'hésitez pas à contribuer ou signaler des bugs !
