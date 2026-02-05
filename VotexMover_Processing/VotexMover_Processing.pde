import controlP5.*;
import processing.serial.*;

PImage img;

Serial myPort;  // Create object from Serial class
int val;      
ControlP5 cp5;
boolean globalModeValue = false;
boolean globalStateValue = false;
boolean gmodeValue = false;
String textValue = "";
float Xposition = 0.00;
float Yposition = 0.00;
float Zposition = 0.00;
float Vitesse = 0.00;
float currentX = 0.00;
float currentY = 0.00;
float currentZ = 0.00;
float currentVitesse = 0.00;
int drawingAreaX = 100; // X coordinate of the drawing area
int drawingAreaY = 100; // Y coordinate of the drawing area
int drawingAreaSize = 300; // Size of the drawing area
boolean drawingMode = false; // New variable to track drawing mode
ArrayList<PVector> trajectory = new ArrayList<PVector>(); // Store trajectory points
String[] gcodeLines;
String Gcommand="G00";
int currentLine = 0;
boolean fileLoaded = false;
boolean waitingForAck = false;
Textfield XPOS;
Textfield YPOS;
Textfield ZPOS;
Textfield VIT;
Button SUBMISSION;
Button STATS;
Toggle MODE;
Toggle STATE;
Button ORIGIN;
Slider2D JOYSTICK;
Slider ZAXIS;
Button SEND;
Button SEND2;
Toggle DRAWMODE;
Button HAUT;
Button BAS;
Button GAUCHE;
Button DROITE;
Button CLEAR;
Button LOAD;
Button SENDLINE;
Toggle GMODE;
Button PICK;
Button PLACE;

void setup() {
  size(900, 600);
  img = loadImage("6020105.jpg");
  PFont font = createFont("arial", 20);
  cp5 = new ControlP5(this);
  myPort = new Serial(this, "COM5", 115200);
  myPort.clear();
  myPort.bufferUntil('\n');
  
  STATE = cp5.addToggle("toggleState")
           .setPosition(350, 100)
           .setSize(50, 20)
           .setLabel("ON/OFF")
           .setState(false);
           
  MODE = cp5.addToggle("toggleMode")
          .setPosition(350, 150)
          .setSize(50, 20)
          .setLabel("Absolute/Relative");

  XPOS = cp5.addTextfield("PositionX")
           .setPosition(520, 100)
           .setSize(100, 40)
           .setFont(font)
           .setFocus(true)
           .setAutoClear(true)
           .setColor(color(255, 0, 0))
           .setLabel("Pos X")
           .setColorLabel(color(255, 255, 255));
           
  YPOS = cp5.addTextfield("PositionY")
           .setPosition(520, 170)
           .setSize(100, 40)
           .setFont(font)
           .setFocus(false)
           .setAutoClear(true)
           .setColor(color(255, 0, 0))
           .setLabel("Pos Y")
           .setColorLabel(color(255, 255, 255));
           
  ZPOS = cp5.addTextfield("PositionZ")
           .setPosition(520, 240)
           .setSize(100, 40)
           .setFont(font)
           .setFocus(false)
           .setAutoClear(true)
           .setColor(color(255, 0, 0))
           .setLabel("Pos Z")
           .setColorLabel(color(255, 255, 255));
           
           
  VIT = cp5.addTextfield("Vitesse")
           .setPosition(650, 170)
           .setSize(100, 40)
           .setFont(font)
           .setFocus(false)
           .setAutoClear(true)
           .setColor(color(255, 0, 0))
           .setLabel("Vitesse")
           .setColorLabel(color(255, 255, 255));
  VIT.hide();
           

           
  SUBMISSION = cp5.addButton("Envoyer")
           .setValue(0)
           .setPosition(530, 310)
           .setSize(80, 40);
           
  STATS = cp5.addButton("Stats")
           .setValue(0)
           .setPosition(580, 450)
           .setSize(80, 40);
           
  CLEAR = cp5.addButton("clear")
           .setPosition(630, 310)
           .setSize(80, 40)
           .setCaptionLabel("Clear");
           
  ORIGIN = cp5.addButton("Initialize")
           .setValue(0)
           .setPosition(580, 380)
           .setSize(80, 40)
           .setLabel("To Origin");
           
  textFont(font);
  
  JOYSTICK = cp5.addSlider2D("XY")
           .setPosition(70, 60)
           .setSize(200, 200)
           .setMinMax(0, 0, 300, 300)
           .setValue(Yposition, Xposition)
           .setLabel("XY Axis");
           
  smooth();
  
  ZAXIS = cp5.addSlider("Z")
           .setPosition(350, 300)
           .setSize(20, 200)
           .setRange(0, 100)
           .setValue(0)
           .setLabel("Z Axis");
           
  SEND = cp5.addButton("SendXY")
           .setValue(0)
           .setPosition(140, 300)
           .setSize(80, 40)
           .setLabel("Send coordinates");
           
  SEND2 = cp5.addButton("SendTrajectory")
           .setValue(0)
           .setPosition(500, 300)
           .setSize(80, 40)
           .setLabel("Send coordinates");
  SEND2.hide();
  
  DRAWMODE = cp5.addToggle("toggleDrawingMode")
           .setPosition(425, 300)
           .setSize(50, 20)
           .setLabel("Drawing Mode");
  
  // Bouton flèche haut
  HAUT = cp5.addButton("haut")
           .setValue(0)
           .setPosition(150, 400)
           .setSize(50, 50)
           .setLabel("^");
     
  // Bouton flèche bas
  BAS = cp5.addButton("bas")
           .setValue(0)
           .setPosition(150, 500)
           .setSize(50, 50)
           .setLabel("v");
     
  // Bouton flèche gauche
  GAUCHE = cp5.addButton("gauche")
           .setValue(0)
           .setPosition(100, 450)
           .setSize(50, 50)
           .setLabel("<");
     
  // Bouton flèche droite
  DROITE = cp5.addButton("droite")
           .setValue(0)
           .setPosition(200, 450)
           .setSize(50, 50)
           .setLabel(">");
   GMODE = cp5.addToggle("toggleGMode")
        .setPosition(350, 200)
        .setSize(50, 20)
        .setLabel("G01/G00")
        .setState(false);

           
  LOAD = cp5.addButton("loadFile")
           .setLabel("Charger Fichier Gcode")
           .setPosition(420, 400)
           .setSize(100, 50);
  
  // Bouton pour envoyer la ligne suivante
  SENDLINE = cp5.addButton("sendLine")
           .setLabel("Envoyer Ligne")
           .setPosition(420, 500)
           .setSize(100, 50);
  PICK = cp5.addButton("Pick")
           .setLabel("PICK")
           .setPosition(700, 380)
           .setSize(80, 40);
  PLACE = cp5.addButton("Place")
           .setLabel("PLACE")
           .setPosition(700, 450)
           .setSize(80, 40);
           
  stroke(255);
  rect(drawingAreaX, drawingAreaY, drawingAreaSize, drawingAreaSize);
  fill(color(255, 255, 255));
}

void draw() {
  background(#0B1635); {
  image(img, 0, 0);
  }
  XY(JOYSTICK); // Met à jour les valeurs de position XY en fonction du joystick
  
  if (drawingMode) {
    fill(255); // Couleur de remplissage blanche
    rect(drawingAreaX, drawingAreaY, drawingAreaSize, drawingAreaSize); // Rectangle blanc pour la zone de dessin
  
    // Dessiner les points de la trajectoire
    stroke(0); // Couleur du trait blanc
    for (int i = 0; i < trajectory.size() - 1 ; i++) {
      PVector p1 = trajectory.get(i);
      PVector p2 = trajectory.get(i + 1);
      line(p1.x, p1.y, p2.x, p2.y);
    }
    
    // Effacer la trajectoire après l'envoi
    if (waitingForAck == true) {
      trajectory.clear();
      waitingForAck = false;
    }
  }
  
  // Show or hide the velocity text field based on GMODE
  if (gmodeValue) {
    VIT.show();
  } else {
    VIT.hide();
  }
}




void toggleDrawingMode(boolean mode) {
  drawingMode = mode;
  // Hide or show GUI elements based on drawing mode
  if (drawingMode) {
    // Hide all GUI elements except SEND button
    XPOS.hide();
    YPOS.hide();
    ZPOS.hide();
    VIT.hide();
    CLEAR.hide();
    HAUT.hide();
    BAS.hide();
    DROITE.hide();
    GAUCHE.hide();
    STATE.hide();
    MODE.hide();
    SUBMISSION.hide();
    ORIGIN.hide();
    JOYSTICK.hide();
    ZAXIS.hide();
    SEND.hide();
    DRAWMODE.show();
    SEND2.show();
    GMODE.hide();
  } else {
    // Show all GUI elements
    SEND2.hide();
    XPOS.show();
    YPOS.show();
    ZPOS.show();
    VIT.show();
    CLEAR.show();
    HAUT.show();
    BAS.show();
    DROITE.show();
    GAUCHE.show();
    STATE.show();
    MODE.show();
    SUBMISSION.show();
    ORIGIN.show();
    JOYSTICK.show();
    ZAXIS.show();
    SEND.show();
    DRAWMODE.show();
    SEND2.hide();
    GMODE.show();
  }
}
void toggleGMode (boolean Gmode){
  gmodeValue=Gmode;
  if (gmodeValue){
    Gcommand="G01";
  }
  else {
    Gcommand="G00";
  }
}

void toggleMode(boolean mode) {
  if (mode) {
    println("Mode is ON");
    myPort.write("G91;");
    globalModeValue = true;
  } else {
    println("Mode is OFF");
    myPort.write("G90;");
    globalModeValue = false;
  }
}

void toggleState(boolean state) {
  if (state) {
    println("State is ON");
    myPort.write("M19;");
    globalStateValue = true;
  } else {
    println("State is OFF");
    myPort.write("M18;");
    globalStateValue = false;
  }
}

void Initialize() {
  println("G10;");
  myPort.write("G10;");
  Xposition = 0;
  Yposition = 0;
  JOYSTICK.setValue(Yposition, Xposition);
}

public void haut() {
  Xposition = Xposition - 3;
  Xposition = constrain(Xposition, 0, 300);
  currentX = Xposition;
  String command = formatGcode(Gcommand, Xposition, Float.NaN, Float.NaN, Float.NaN);
  println(command);
  myPort.write(command); // Uncomment to send to serial port
  JOYSTICK.setValue(Yposition, Xposition);
}

public void bas() {
  Xposition = Xposition + 3;
  Xposition = constrain(Xposition, 0, 300);
  currentX = Xposition;

  String command = formatGcode(Gcommand, Xposition, Float.NaN, Float.NaN, Float.NaN);
  println(command);
  myPort.write(command); // Uncomment to send to serial port
  JOYSTICK.setValue(Yposition, Xposition);
}
void Pick(){
  myPort.write("G101;");
  println("G101;");
}
void Place(){
  myPort.write("G102;");
  println("G102;");
}
public void gauche() {
  Yposition = Yposition - 3;
  Yposition = constrain(Yposition, 0, 300);
  currentY = Yposition;
  String command = formatGcode(Gcommand, Float.NaN, Yposition, Float.NaN, Float.NaN);
  println(command);
  myPort.write(command); // Uncomment to send to serial port
  JOYSTICK.setValue(Yposition, Xposition);
}

public void droite() {
  Yposition = Yposition + 3;
  Yposition = constrain(Yposition, 0, 300);
  currentY = Yposition;
  String command = formatGcode(Gcommand, Float.NaN, Yposition, Float.NaN, Float.NaN);
  println(command);
  myPort.write(command); // Uncomment to send to serial port
  JOYSTICK.setValue(Yposition, Xposition);
}

public void clear() {
  XPOS.clear();
  YPOS.clear();
  ZPOS.clear();
  VIT.clear();
}

public void Envoyer() {
  XPOS.submit();
  YPOS.submit();
  ZPOS.submit();
  VIT.submit();
}

public void Stats() {
   String message;
   while (myPort.available() > 0) { myPort.read(); }
   myPort.write("M114");
   while (myPort.available() > 0) {
     message = myPort.readStringUntil('\n');
     if (message != null) {
       println(message);
     }
   }
}

public void PositionX(String posX) {
  try {
    float newX = Float.parseFloat(posX);
    if (!globalModeValue) {
      Xposition = newX;
    } else {
      Xposition += newX;
    }
    currentX = Xposition;
    String command = formatGcode(Gcommand, Xposition, Float.NaN, Float.NaN, Float.NaN);
    println(command);
    myPort.write(command + "\n"); // Uncomment to send to serial port
    JOYSTICK.setValue(Yposition, Xposition);
  } catch (NumberFormatException e) {
    println("Error: Invalid input format for X position.");
  }
}

public void PositionY(String posY) {
  try {
    float newY = Float.parseFloat(posY);
    if (!globalModeValue) {
      Yposition = newY;
    } else {
      Yposition += newY;
    }
    currentY = Yposition;
    String command = formatGcode(Gcommand, Float.NaN, Yposition, Float.NaN, Float.NaN);
    println(command);
    myPort.write(command + "\n"); // Uncomment to send to serial port
    JOYSTICK.setValue(Yposition, Xposition);
  } catch (NumberFormatException e) {
    println("Error: Invalid input format for Y position.");
  }
}

public void PositionZ(String posZ) {
  try {
    float newZ = Float.parseFloat(posZ);
    if (!globalModeValue) {
      Zposition = newZ;
    } else {
      Zposition += newZ;
    }
    currentZ = Zposition;
    String command = formatGcode(Gcommand, Float.NaN, Float.NaN, Zposition, Float.NaN);
    println(command);
    myPort.write(command); // Uncomment to send to serial port
    ZAXIS.setValue(Zposition);
  } catch (NumberFormatException e) {
    println("Error: Invalid input format for Z position.");
  }
}

public void Vitesse(String vit) {
  try {
    float newVit = Float.parseFloat(vit);
    currentVitesse = newVit;
    String command = formatGcode(Gcommand, Float.NaN, Float.NaN, Float.NaN, currentVitesse);
    println(command);
    myPort.write(command); // Uncomment to send to serial port
  } catch (NumberFormatException e) {
    println("Error: Invalid input format for velocity.");
  }
}




public void XY(Slider2D slider) {
  float[] sliderValues = slider.getArrayValue();
  if (sliderValues.length >= 2) {
    Xposition = sliderValues[1];
    Yposition = sliderValues[0];
  } else {
    println("Error: Unable to retrieve X and Y values from Slider2D.");
  }
}

public void Z(float posZ) {
  Zposition = posZ;
  currentZ = Zposition;
  MODE.setValue(false);
  String command = "G0 Z"+Zposition+";";
  println(command);
  myPort.write(command); // Uncomment to send to serial port
}


// Add a new point to the trajectory when mouse is dragged
void mouseDragged() {
  if (drawingMode) {
    if (mouseX >= drawingAreaX && mouseX <= drawingAreaX + drawingAreaSize && mouseY >= drawingAreaY && mouseY <= drawingAreaY + drawingAreaSize) {
      trajectory.add(new PVector(mouseX, mouseY));
    }
  }
}

// Send trajectory to serial port
void SendTrajectory() {
  for (PVector point : trajectory) {
    String command; 
    command = formatGcode(Gcommand, point.x-100, point.y-100, currentZ, currentVitesse);
    println(command);
    myPort.write(command); // Envoie la commande au port série

    // Attendre la confirmation de l'Arduino
    while (true) {
      if (myPort.available() > 0) {
        String incomingData = myPort.readStringUntil('\n');
        if (incomingData != null && incomingData.trim().equals(">")) {
          break; // Confirmation reçue, sortir de la boucle d'attente
        }
      }
    }

    // Vider le buffer série
    myPort.clear();
   Xposition = point.x-100;
   Yposition = point.y-100;
  JOYSTICK.setValue(Yposition,Xposition);
  delay(100);
  }
  println("Trajectory sent to serial port.");
  waitingForAck = true;
  
}



String formatGcode(String gCommand, float x, float y, float z, float f) {
  String gcode = gCommand;
  gcode += (Float.isNaN(x) ? (" X" +int(currentX)) : (" X" +int(x)));
  gcode += (Float.isNaN(y) ? (" Y" +int(currentY)) : (" Y" +int(y)));
  gcode += (Float.isNaN(z) ? (" Z" +int(currentZ)) : (" Z" +int(z)));
  if (gCommand.equals("G01") && !Float.isNaN(f)) {
    gcode += " F" + f;
  }
  gcode += ";";
  return gcode;
}


void loadFile() {
  selectInput("Sélectionner un fichier Gcode :", "fileSelected");
}

void fileSelected(File selection) {
  if (selection == null) {
    println("Fenêtre de sélection fermée ou fichier non sélectionné.");
  } else {
    gcodeLines = loadStrings(selection.getAbsolutePath());
    currentLine = 0;
    fileLoaded = true;
    println("Fichier chargé : " + selection.getAbsolutePath());
  }
}
public void SendXY() {
  String command; // Change to G00 if necessary
  command = formatGcode(Gcommand,Xposition, Yposition, currentZ, currentVitesse);
  println(command);
  myPort.write(command); // Uncomment to send to serial port
}

public void sendLine() {
  if (fileLoaded && currentLine < gcodeLines.length) {
    sendGcodeLine();
  }
}

void sendGcodeLine() {
  if (currentLine < gcodeLines.length) {
    String line = gcodeLines[currentLine].trim();
    if (line.length() > 0) {
      String command = line.startsWith("G01") ? "G01" : "G00";
      command = formatGcode(command,Xposition, Yposition, Zposition, line.startsWith("G01") ? currentVitesse : Float.NaN);
      myPort.write(command);
      println("Envoyé : " + command);
      waitingForAck = true;
    }
  } else {
    println("Toutes les lignes ont été envoyées."); 
  }
}




void serialEvent(Serial myPort) {
  String response = myPort.readStringUntil('\n');
  if (response != null) {
    response = response.trim();
    println("Reçu : " + response);
    if (response.equals("M114")) {
      String additionalData = myPort.readStringUntil('\n');
      if (additionalData != null) {
        println("M114 Data: " + additionalData);
      }
      // Clear the buffer after displaying the data
      myPort.clear();
    } else if (response.equals("ACK")) {
      currentLine++;
      waitingForAck = false;
      if (currentLine < gcodeLines.length) {
        sendGcodeLine();
      } else {
        println("Toutes les lignes ont été envoyées et exécutées.");
      }
    }
  }
}
