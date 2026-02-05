# Walking_Gait — Contrôle de marche pour robot quadrupède (2 pattes)

Ce projet Arduino pilote **six servos** (deux pattes à trois articulations chacune) via un **PCA9685** (Adafruit PWM Servo Driver). Il permet de **positionner les pattes en (x,y)** grâce à la cinématique inverse, et d’exécuter une **marche cyclique** (gait) en temps réel ou à la demande.

---

## Ce que fait le code (résumé)

- **Initialisation** : communication série 115200 baud, démarrage du PWM 50 Hz, centrage de tous les servos à 90°.
- **Contrôle des servos** : commande par **index de servo** (0–5) ou par **broche** (0–15), avec limites d’angle, corrections mécaniques et orientation (certains servos sont inversés).
- **Contrôle des pattes** : pour chaque patte (droite ou gauche), le code calcule les angles **hanche** et **genou** à partir d’une position cible **(x, y)** en mm (cinématique inverse 2R).
- **Marche (gait)** : une machine à états définit pour chaque patte une phase **STANCE** (pied au sol) ou **SWING** (pied en l’air). En fonction du temps dans le cycle, le code calcule les (x,y) cibles pour les deux pattes et met à jour les servos en continu.
- **Boucle principale** : lit les commandes série (caractère + nombres) et soit met à jour la marche en continu, soit exécute une commande ponctuelle (angle, position de patte, centrage, etc.).

Aucun autre rôle : pas de capteurs, pas de Bluetooth, uniquement Série + PWM + calcul de trajectoire et de gait.

---

## Commandes série (détail)

Toutes les commandes sont envoyées via le **Moniteur Série** (115200 baud). Après la **lettre**, on envoie les **nombres** séparés par des espaces ou retours à la ligne, selon la commande.

| Commande | Lettre | Paramètres | Effet exact |
|----------|--------|------------|-------------|
| **Centrer un servo** | `c` | 1 entier : index du servo (0–5), ou `-1` pour tous | Met le servo (ou tous) à 90° (position centrale). Ex. : `c 0` centre le servo 0 ; `c -1` centre les 6 servos. |
| **Déplacer un servo (angle)** | `s` | 1 entier + 1 float : index servo, puis angle en degrés (0–180) | Met le servo à l’angle absolu demandé, en respectant les limites et corrections. Ex. : `s 2 45` met le servo 2 à 45°. |
| **Positionner la patte droite (x,y)** | `l` | 2 float : x puis y en mm | Calcule les angles hanche/genou pour la patte **droite** avec la cible (x,y), puis envoie les ordres aux servos. La patte gauche n’est pas modifiée. Ex. : `l 50 100`. |
| **Avancer la marche (progression)** | `p` | 1 float : progression du cycle (0.0 à 1.0) | Met à jour les positions des **deux** pattes pour la phase de gait correspondant à cette progression (sans timer). Utile pour tester le gait pas à pas. Ex. : `p 0.25`. |
| **Marche ON/OFF** | `w` | aucun | Bascule le mode « marche continue ». Si activé : démarre le cycle à partir de l’instant actuel et met à jour les pattes à chaque `loop()`. Si désactivé : centre tous les servos (90°). |
| **Inverser genou (knee up/down)** | `k` | aucun | Bascule la configuration **knee up** / **knee down** pour la cinématique inverse. Change la solution (genou plié vers le haut ou vers le bas) pour les prochaines commandes `l`, `p` et pendant la marche. |

**Résumé des caractères :**  
`c` = center, `s` = servo angle, `l` = leg (x,y) droite, `p` = progress gait, `w` = walk toggle, `k` = knee config.

---

## Objets et constantes principaux

### Pilote PWM et servos

- **`Adafruit_PWMServoDriver pwm`** — Objet qui parle au PCA9685 en I2C ; utilisé pour `setPWM(pin, 0, pulseWidth)`.
- **`pinCount`** = 16 — Nombre de broches PWM du PCA9685.
- **`servoCount`** = 6 — Nombre de servos utilisés (3 par patte × 2 pattes).
- **`servoList[]`** — Tableau des broches PWM utilisées : `{0,1,2}` patte droite, `{3,4,6}` patte gauche (broche 5 non utilisée).
- **`SERVOMIN`, `SERVOMAX`, `servoCenterPWM`** — Bornes et centre du signal PWM (500–2500 µs, 50 Hz) pour une plage 0–180°.

### Index des articulations

- **`JointIndex`** (enum) : `RIGHT_HIP`, `RIGHT_KNEE`, `RIGHT_ANKLE`, `LEFT_HIP`, `LEFT_KNEE`, `LEFT_ANKLE` — Correspondance index 0–5 ↔ articulation.
- **`Orientation[]`** — Pour chaque servo, indique si l’angle est pris tel quel ou inversé (180° − angle) pour corriger le sens de rotation mécanique.
- **`jointCorrection[]`** — Décalage en degrés ajouté à chaque articulation pour le calage mécanique (ex. cheville).
- **`angleLimit[][]`** — Limites min/max en degrés par articulation (hanche limitée à 124°, genou/cheville 0–180°).
- **`currentAngles[]`** — Dernier angle envoyé à chaque servo (état courant).

### Géométrie des pattes (cinématique inverse)

- **`L1`** = 63 mm — Longueur du premier segment (cuisse).
- **`L2`** = 75 mm — Longueur du deuxième segment (tibia).
- **`kneeUp`** — Booléen : `true` = genou plié « vers le haut », `false` = autre solution (genou vers le bas).
- **`LegInverseKinematics(kneeUp, x, y, output_theta)`** — Calcule les deux angles (hanche, genou) à partir de (x, y) en mm ; remplit `output_theta[]` ; retourne `true` si la cible est atteignable.
- **`setLegPosition(leg, kneeUp, x, y)`** — Applique la position (x,y) à la patte **Right** (0) ou **Left** (1) : appelle la cinématique inverse puis `moveServo` pour hanche et genou. La cheville n’est pas pilotée par (x,y).

### Gait (marche)

- **`STEP_LENGTH`** = 20 mm — Longueur du pas (avance/recul du pied).
- **`REST_HEIGHT`** = L1 + L2 − 10 mm — Hauteur verticale « au repos » (distance hanche–sol).
- **`MAX_SWING_HEIGHT`** = 20 mm — Amplitude de montée du pied en phase SWING.
- **`STANCE_DURATION`, `SWING_DURATION`** — Durées relatives des phases (0.5 s chacune dans le code).
- **`CYCLE_PERIOD`** = 4000 ms — Durée totale d’un cycle de marche (une période complète pour les deux pattes).
- **`gaitStartTime`** — Timestamp (millis) du début du cycle courant ; utilisé pour calculer la progression dans le cycle.
- **`GaitState`** — Structure : `phase` ('S' = STANCE, 'W' = SWING) et `progressInPhase` (0.0 à 1.0).
- **`GetGaitState(cycleProgress, leg)`** — Donne la phase et la progression dans la phase pour une patte (0 = droite, 1 = gauche) à partir de la progression globale du cycle.
- **`ComputeLegTarget(cycleProgress, leg, targetX, targetY)`** — Calcule la cible (x,y) pour la patte à l’instant donné : en STANCE le pied recule avec une légère oscillation verticale ; en SWING il avance et monte.
- **`UpdateGaitProgress(cycleProgress)`** — Pour une progression donnée (0.0–1.0), calcule les (x,y) des deux pattes et appelle `setLegPosition` pour les deux.
- **`UpdateGait(currentTime)`** — Calcule la progression du cycle à partir de `gaitStartTime` et de `currentTime`, puis appelle `UpdateGaitProgress`.
- **`ResetWalk()`** — Réinitialise `gaitStartTime` à l’instant actuel (redémarre le cycle).
- **`zid_asa7bi`** — Variable globale : si `true`, la boucle principale appelle `UpdateGait(millis())` à chaque tour, donc la marche continue ; si `false`, seules les commandes série sont traitées.

### Fonctions de bas niveau

- **`pinValid(pin)`** — Vérifie que la broche est entre 0 et 15.
- **`servoValid(servo)`** — Vérifie que l’index servo est entre 0 et 5 (ou -1 pour « tous »).
- **`Angle_To_PWM(angle)`** — Convertit un angle 0–180° en valeur PWM (entre SERVOMIN et SERVOMAX).
- **`moveServoPin(pin, angle)`** — Envoie l’angle à la broche PWM donnée (sans correction ni limite).
- **`centerServoPin(pin)`** — Met la broche au centre (servoCenterPWM).
- **`constrainAngle(angle, index)`** — Ramène l’angle dans les limites de l’articulation.
- **`applyOrientation(angle, index)`** — Applique l’inversion 180° − angle si nécessaire.
- **`moveServo(servo, angle)`** — Commande un servo par index : applique limites, corrections, orientation, puis appelle `moveServoPin` sur la broche correspondante. Si `servo == -1`, applique l’angle à tous les servos.
- **`centerServo(servo)`** — Met le servo (ou tous si -1) à 90°.

---

## Conventions et unités

- **Repère (x, y)** : x = horizontal vers l’avant, y = vertical vers le haut ; unités en **mm**. Origine au niveau de la hanche.
- **Angles** : en **degrés**, 0–180, avec limites par articulation.
- **Temps** : **millisecondes** (`millis()`), cycle de marche = 4000 ms.
- **Série** : **115200** baud ; commande = un caractère puis, selon la commande, un ou plusieurs nombres (entiers ou décimaux).

---

## Fichiers (dossier courant)

- **`Walking_Gait.ino`** — Code Arduino : setup, loop, commandes série, cinématique inverse, gait et contrôle servos.
- **`README.md`** — Ce fichier (documentation du projet).

---

## Dépendances

- **Adafruit PWM Servo Driver Library** (pour `Adafruit_PWMServoDriver`).
- Carte compatible (ex. Arduino) avec **Wire** (I2C) et **Serial** ; PCA9685 câblé en I2C.

Ce README décrit exactement ce que fait le code, les commandes disponibles et les objets principaux du projet.
