# Mars Rover — Robot omnidirectionnel + détection QR (YOLO)

Projet de robot type Mars Rover : contrôle Arduino/ESP32, interface Streamlit et module de **détection de QR codes par YOLO** avec envoi des commandes vers l’Arduino.

---

## Structure du projet

```
mars-rover/
├── Arduino_G8_P4_S4.ino    # Firmware ESP32 (moteurs, capteurs, commandes série)
├── PY_G8_P4_S4.py         # Interface Streamlit (contrôle + monitoring)
├── qr_detection_arduino.py # Détection QR (YOLO) + communication Arduino
├── requirements_qr.txt    # Dépendances Python pour le module QR
└── README.md              # Ce fichier
```

- **Arduino** : code existant (pas modifié), à flasher sur l’ESP32.
- **Python (Streamlit)** : code existant (interface + liaison série), à lancer séparément.
- **Module QR** : script ajouté qui utilise YOLO pour détecter les QR, décode le contenu et envoie les commandes à l’Arduino sur le même port série.

---

## Prérequis matériel

- ESP32 avec shield moteurs, encodeurs, capteurs (ultrason, gaz, HTU21D, MPU6050, etc.)
- Connexion USB ou radio vers le PC (port série, ex. `COM14` sous Windows)
- Webcam ou caméra USB pour la détection QR

---

## Installation

### 1. Environnement Python

```bash
cd mars-rover
python -m venv venv
venv\Scripts\activate   # Windows
# source venv/bin/activate  # Linux/macOS
pip install -r requirements_qr.txt
```

### 2. Arduino / ESP32

- Ouvrir `Arduino_G8_P4_S4.ino` dans l’IDE Arduino.
- Installer les librairies : TinyGPS++, Adafruit HTU21DF, MPU6050_tockn, PID_v1, ESP32Servo, SimpleTimer.
- Sélectionner la carte ESP32 et le bon port, puis téléverser.

### 3. Interface Streamlit (optionnel, code existant)

```bash
streamlit run PY_G8_P4_S4.py
```

Configure le port série (ex. COM14, 57600) dans l’interface.

---

## Module détection QR (YOLO + Arduino)

Le script **`qr_detection_arduino.py`** :

- Utilise **YOLO** (via la librairie `qrdet`, basée sur YOLOv8) pour détecter les QR dans le flux caméra.
- Décode le contenu du QR (OpenCV `QRCodeDetector` ou `pyzbar`) dans la zone détectée.
- Envoie le **contenu du QR comme commande** vers l’Arduino (même protocole que l’interface Streamlit : une ligne par commande, terminée par `\n`).

### Commandes Arduino supportées (rappel)

Le firmware attend des lignes du type :

| Commande | Exemple | Effet |
|----------|---------|--------|
| Arrêt | `s` | Arrêt moteurs |
| Avant / Arrière | `a` / `r` | Marche avant / arrière |
| Latéral | `d` / `g` | Droite / gauche |
| Rotation | `l` / `t` | Rotation droite / gauche |
| Cinématique | `c Vx Vy Wz` | Vitesses (ex. `c 15 0 0`) |
| Position | `p x y theta` | Déplacement en position |

Vous pouvez imprimer des QR contenant exactement ces chaînes (ex. `a`, `s`, `c 15 0 0`) et le script les enverra à l’ESP32.

### Lancement du module QR

```bash
python qr_detection_arduino.py
```

Options en ligne de commande (exemples) :

- `--port COM14` : port série (à adapter).
- `--baud 57600` : débit (par défaut 57600).
- `--camera 0` : indice de la webcam (0 = première caméra).
- `--no-arduino` : exécution sans Arduino (détection + affichage uniquement).

Exemple :

```bash
python qr_detection_arduino.py --port COM14 --baud 57600 --camera 0
```

### Dépendances spécifiques QR

Voir `requirements_qr.txt` ; principales dépendances :

- `qrdet` : détection QR basée YOLOv8
- `opencv-python` : lecture caméra + décodage QR
- `pyserial` : communication avec l’Arduino

---

## Workflow recommandé

1. Flasher l’ESP32 avec `Arduino_G8_P4_S4.ino`.
2. Brancher l’ESP32 en USB (ou radio) et noter le port (ex. COM14).
3. Lancer l’interface Streamlit si besoin : `streamlit run PY_G8_P4_S4.py`.
4. Lancer le module QR : `python qr_detection_arduino.py --port COM14`.
5. Montrer un QR contenant une commande (ex. `a` ou `s`) devant la caméra : le script détecte, décode et envoie la commande à l’Arduino.

---

## Inspirations

Structure et idées inspirées de projets type **robotics** (dépôts type aymen-cell/robotics) : séparation firmware / interface / modules de perception, README avec structure, installation et usage.

---

## Licence et remarques

- Code Arduino et Python existants : inchangés, tels qu’ils étaient dans le projet G8_P4_S4.
- Seul le module **détection QR + communication Arduino** a été ajouté dans ce dossier.
- Pour toute modification du comportement du robot (moteurs, capteurs), modifier le firmware Arduino ou l’interface Streamlit dans leurs fichiers respectifs.
