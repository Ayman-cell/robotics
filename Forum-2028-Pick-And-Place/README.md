# Forum 2028 — Robot Pick & Place (Contrôle 3 axes)

Projet de **robot Pick & Place** à 3 axes (X, Y, Z) avec contrôle par **G-code**, interface **Processing** et **contrôle par gestes de la main** (Python + MediaPipe). Réalisé dans le cadre du Forum 2028.

---

## 📋 Table des matières

- [Vue d'ensemble](#-vue-densemble)
- [Structure du projet](#-structure-du-projet)
- [Matériel et câblage](#-matériel-et-câblage)
- [Installation et utilisation](#-installation-et-utilisation)
- [Commandes G-code (firmware Arduino)](#-commandes-g-code-firmware-arduino)
- [Contrôle par gestes (Python)](#-contrôle-par-gestes-python)
- [Dépannage](#-dépannage)

---

## 🎯 Vue d'ensemble

Le système comprend :

1. **Firmware Arduino** (`VortexMover_Silent`) : interprète du G-code, pilotage des moteurs pas-à-pas (X, Y, Z), fin de course, pompe/vanne pour le pick & place.
2. **Interfaces Processing** :
   - **Processing_2** : dessin de trajectoires (clic–glisser) puis envoi des points à l'Arduino.
   - **VotexMover_Processing** : interface complète (joystick 2D, champs X/Y/Z, mode dessin, chargement de fichiers G-code, boutons Pick/Place).
3. **Script Python** : détection de gestes de la main (MediaPipe + webcam) et envoi de commandes série pour déplacer le robot.

Espace de travail typique : **250 mm (X) × 300 mm (Y) × 100 mm (Z)** (à adapter selon votre machine).

---

## 📁 Structure du projet

```
Forum-2028-Pick-And-Place/
├── README.md                 # Ce fichier
├── requirements.txt          # Dépendances Python (contrôle par gestes)
├── Python Forum 2028.txt     # Script Python — contrôle par gestes
├── Processing_2/             # Interface Processing — dessin de trajectoires
│   ├── Processing_2.pde
│   └── (images optionnelles)
├── VortexMover_Silent/       # Firmware Arduino — contrôleur 3 axes
│   └── VortexMover_Silent.ino
└── VotexMover_Processing/    # Interface Processing — contrôle complet
    ├── VotexMover_Processing.pde
    └── 6020105.jpg
```

---

## 🔌 Matériel et câblage

### Côté Arduino (VortexMover_Silent)

- **Moteurs** : 3 axes (X, Y, Z) avec pilotes type step/dir.
- **Broches (à adapter si besoin)** :
  - **EN+** : 8 (X), 6 (Y), 2 (Z)
  - **CW+ (Direction)** : 9 (X), 5 (Y), 22 (Z)
  - **CLK+ (Step)** : 10 (X), 3 (Y), 7 (Z)
  - **Fin de course** : 21 (X), 20 (Y), 19 (Z)
  - **Pompe** : 40 — **Soupape** : 12
- **Communication** : USB série **115200 baud**.

Vérifiez les constantes dans `VortexMover_Silent.ino` (dimensions, pas par tour, etc.) pour les adapter à votre mécanique.

---

## 🚀 Installation et utilisation

### 1. Firmware Arduino

1. Ouvrir `VortexMover_Silent/VortexMover_Silent.ino` dans l'IDE Arduino.
2. Vérifier/adapter les broches et constantes (voir commentaires dans le code).
3. Sélectionner la carte et le port, puis téléverser.
4. Noter le **port série** (ex. `COM5` sous Windows, `/dev/ttyUSB0` sous Linux).

### 2. Interfaces Processing

- **Processing 2 (dessin de trajectoires)**  
  - Ouvrir `Processing_2/Processing_2.pde`.  
  - Dans le code, mettre à jour `portName` (ex. `"COM5"`).  
  - Lancer le sketch. Dessiner en glissant la souris ; les coordonnées sont envoyées en G-code à l'Arduino.

- **VotexMover_Processing (contrôle complet)**  
  - Ouvrir `VotexMover_Processing/VotexMover_Processing.pde`.  
  - Installer la librairie **ControlP5** (Sketch → Importer une bibliothèque → ControlP5).  
  - Remplacer `"COM5"` par votre port série dans le code.  
  - Lancer le sketch pour joystick, champs X/Y/Z, mode dessin, Pick/Place, etc.

### 3. Contrôle par gestes (Python)

1. Créer un environnement virtuel (recommandé) :
   ```bash
   python -m venv venv
   venv\Scripts\activate   # Windows
   ```

2. Installer les dépendances :
   ```bash
   pip install -r requirements.txt
   ```

3. Dans le script Python (`Python Forum 2028.txt`), modifier le port série si besoin :
   ```python
   ser = serial.Serial('COM5', 115200, timeout=1)  # Remplacez COM5 par votre port
   ```

4. Lancer le script :
   ```bash
   python "Python Forum 2028.txt"
   ```
   Une fenêtre s'ouvre avec la webcam ; les gestes détectés envoient des commandes à l'Arduino.

---

## 📟 Commandes G-code (firmware Arduino)

| Commande | Description |
|----------|-------------|
| `G00 X.. Y.. Z..;` | Déplacement rapide (coordonnées en mm) |
| `G01 X.. Y.. Z.. F..;` | Déplacement linéaire avec vitesse F (mm/s) |
| `G04 P..;` | Pause (P en secondes) |
| `G10;` | Retour à l'origine (X, Y, Z) |
| `G11;` | Remise à zéro axe X |
| `G12;` | Remise à zéro axe Y |
| `G13;` | Remise à zéro axe Z |
| `G90;` | Mode coordonnées absolues |
| `G91;` | Mode coordonnées relatives |
| `G92 X.. Y.. Z..;` | Mise à jour manuelle de la position stockée |
| `G101;` | Activer l'aspiration (Pick) |
| `G102;` | Désactiver l'aspiration (Place) |
| `M18;` | Désactiver les moteurs |
| `M19;` | Activer les moteurs |
| `M100;` | Afficher l'aide |
| `M114;` | Afficher les statistiques (position, vitesse, etc.) |

Chaque commande doit se terminer par `;`. L'Arduino renvoie `>` lorsqu'il est prêt pour la commande suivante.

---

## ✋ Contrôle par gestes (Python)

Le script utilise **MediaPipe** pour détecter une main et reconnaître des gestes. Les gestes reconnus envoient les commandes suivantes (en mode relatif `G91`) :

| Geste | Action envoyée |
|-------|----------------|
| **Main ouverte** (5 doigts) | `G0 Z-20` (descendre Z) |
| **Index seul** (Left) | `G0 Y-20` |
| **Pouce + auriculaire** (Right) | `G0 Y20` |
| **Peace** (index + majeur) | `G0 X20` |
| **OK** (pouce + index en cercle) | `G0 X-20` |
| **Poing fermé** (Closed) | `G10` (retour origine) |
| **Autre geste** | `G0 Z20` (monter Z) |

Ajustez les valeurs (ex. `20`) dans le script si vous voulez des pas plus grands ou plus petits.

---

## 🔧 Dépannage

- **Port série introuvable** : Vérifier le port dans le Gestionnaire de périphériques (Windows) ou `ls /dev/tty*` (Linux). Adapter `COMx` ou le chemin dans Arduino, Processing et Python.
- **Aucune réponse de l'Arduino** : Vérifier que le débit est **115200** partout et qu'aucun autre logiciel n'utilise le port.
- **Moteurs ne bougent pas** : Vérifier `M19` (moteurs activés), alimentation des pilotes et câblage des broches EN/DIR/STEP.
- **Python : "No module named 'cv2'"** : Exécuter `pip install -r requirements.txt` dans le même environnement que le script.

---

Projet **Forum 2028** — Robot Pick & Place. Vous pouvez adapter le code à votre matériel (broches, dimensions, gestes).

**[← Retour au portfolio](../../)**
