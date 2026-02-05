import processing.serial.*; // Importer la bibliothèque pour la communication série

Serial port; // Objet Serial pour la communication série
String portName = "COM5"; // Port série sur lequel Arduino est connecté (changez-le selon votre configuration)
int baudRate = 115200; // Débit en bauds pour la communication série

ArrayList<PVector> trajectory;  // Liste des points formant la trajectoire simplifiée
int currentPoint;  // Index du point actuel vers lequel le système se déplace
PVector position;  // Position actuelle du système
float speed = 2;  // Vitesse de déplacement du système
PVector startPoint;  // Point de départ de la ligne en cours de dessin
ArrayList<PVector> currentDrawing;  // Points temporaires pendant le dessin

void setup() {
  size(300, 300);  // Taille de la fenêtre réduite à 300x300
  trajectory = new ArrayList<PVector>();  // Initialisation de la liste des points
  currentPoint = 0;  // Début au premier point
  position = new PVector(0, height);  // Position initiale du système (coin inférieur gauche)
  currentDrawing = new ArrayList<PVector>();  // Initialisation de la liste des points temporaires

  // Initialiser la communication série
  port = new Serial(this, portName, baudRate);
}

float maxX=250, maxY=300;
float bottomRightX=(width+maxX)/2, bottomRightY=(height+maxY)/2;
float topLeftX=(width-maxX)/2, topLeftY=(height-maxX)/2;

void draw() {
  background(255);  // Efface l'écran avec un fond blanc
  fill(255,0,0);
  rect(topLeftX,topLeftY,maxX,maxY);
  drawGrid();  // Dessine la grille du repère orthonormé

  // Dessine le dessin temporaire pendant que l'utilisateur dessine
  stroke(0, 0, 255);  // Bleu pour le dessin temporaire
  noFill();
  beginShape();
  for (PVector p : currentDrawing) {
    vertex(p.x, p.y);
  }
  endShape();

  // Dessine la trajectoire simplifiée
  stroke(0);  // Noir pour la trajectoire simplifiée
  beginShape();
  for (PVector p : trajectory) {
    vertex(p.x, p.y);
  }
  endShape();

  // Déplace le système s'il y a des points à suivre
  if (trajectory.size() > 0 && currentPoint < trajectory.size()) {
    PVector target = trajectory.get(currentPoint);  // Cible actuelle
    PVector direction = PVector.sub(target, position);  // Direction vers la cible
    if (direction.mag() < speed) {
      // Si la distance à la cible est inférieure à la vitesse, atteignez la cible
      position = target.copy();
      currentPoint++;
    } else {
      // Déplacez-vous vers la cible
      direction.setMag(speed);
      position.add(direction);
    }
  }

  // Dessine le système à sa position actuelle
  fill(255, 0, 0);
  ellipse(position.x, position.y, 20, 20);
}

void mousePressed() {
  // Enregistre le point de départ de la ligne et commence à enregistrer les points du dessin temporaire

  startPoint = new PVector(mouseX, mouseY);
  currentDrawing.clear();
  currentDrawing.add(startPoint);
}

void mouseDragged() {
  // Ajoute des points au dessin temporaire pendant que l'utilisateur dessine
  currentDrawing.add(new PVector(mouseX, mouseY));
}

void mouseReleased() {
   trajectory.clear();
  
  // Enregistre le point d'arrivée et transforme le dessin en une ligne droite
  PVector endPoint = new PVector(mouseX, mouseY);
  currentDrawing.add(endPoint);
  trajectory.add(startPoint);
  trajectory.add(endPoint);
  currentDrawing.clear();  // Efface le dessin temporaire
  
  // Envoyer les coordonnées de la trajectoire à l'Arduino
  sendTrajectoryToArduino(trajectory);
  // Enregistre le point d'arrivée et transforme le dessin en une ligne droite
  // Efface le dessin temporaire
  
  // Envoyer les coordonnées de la trajectoire à l'Arduino
 
}

void sendTrajectoryToArduino(ArrayList<PVector> trajectory) {
  String Instruction;
  for (PVector point : trajectory) {
    
    Instruction="G00 X" + map(point.x, 0, width, 0, 300) + " Y" + map(point.y, 0, height, 300, 0) + ";";
    // Envoyer les coordonnées X et Y à l'Arduino
    println(Instruction);
    port.write(Instruction);
    delay(1); // Ajouter un petit délai entre chaque envoi pour éviter les problèmes de buffer
  }
}

void keyPressed() {
  if (key == 'r') {
    // Réinitialise le système et les trajectoires si la touche 'r' est pressée
    trajectory.clear();
    currentPoint = 0;
    position.set(0, height);
  }
}

void drawGrid() {
  stroke(200);  // Couleur des lignes de la grille
  int gridSize = 20;  // Taille de chaque cellule de la grille

  // Dessine les lignes verticales
  for (int x = 0; x < width; x += gridSize) {
    line(x, 0, x, height);
  }

  // Dessine les lignes horizontales
  for (int y = 0; y < height; y += gridSize) {
    line(0, y, width, y);
  }

  // Dessine les axes X et Y
  stroke(0);  // Couleur des axes
  line(0, height, width, height);  // Axe X (au bas de la fenêtre)
  line(0, 0, 0, height);  // Axe Y (à gauche de la fenêtre)

  // Ajoute des étiquettes aux axes
  fill(0);
  textSize(12);
  textAlign(CENTER, CENTER);

  // Étiquettes sur l'axe X
  for (int x = 0; x < width; x += gridSize) {
    text(x / gridSize, x, height + 10);
  }

  // Étiquettes sur l'axe Y
  for (int y = 0; y < height; y += gridSize) {
    text((height - y) / gridSize, -10, y);
  }
}
