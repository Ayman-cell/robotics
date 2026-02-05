//Pin
const int PinEn[]={8,6,2}; //EN+
const int PinDir[]={9,5,22}; //CW+
const int PinStep[]={10,3,7}; //CLK+
const int PinCapt[]={21,20,19}; //Fin De Course
const int PinSoupape = 12;
const int PinPompe = 40;
const int x=0, y=1, z=2;

//Constantes
const float radius = 0.006;
const float pasVis = 0.008;
const float stepsPerRevolution = 200.0; //Nombre de pas par tour
//Precision
const float AngularPrecision=(2*PI)/stepsPerRevolution;
const float precisionXY=radius*AngularPrecision; //(=1.5708*10^-4)
const float precisionZ=AngularPrecision*(pasVis/(2*PI));
//Distance (mm) Vers Nombre De Pas
const float K[] = {0.001/precisionXY, 0.001/precisionXY, (0.001*2*PI)/(pasVis*AngularPrecision)};
//( K[x,y] = 6.366197 Du mm vers les pas )

//Interrupteurs
int State=1;
int Moving=0;
int type=0;
    //type 0 pour mouvement absolu
    //type 1 pour mouvement relatif
int MovingToOrigin=0;
bool reachedMax[]={LOW,LOW,LOW};

//Variables De Mouvement
float Distance[]={0.0,0.0,0.0};
float Position[]={0.0,0.0,0.0}; //Position Absolue Actuelle
float Target[3];
float maxDistance;
int Sens[]={-1,-1,-1};
bool SensPositif[]={LOW,HIGH,LOW};
float Speed[3]={0.0,0.0,0.0}; //(mm/s)
float DiagonalSpeed=0.0;
float ImpulsePeriod=1500.0; //(Microsecondes)
float InitialSteps=0.0;

//Gestion D'instruction
const int MaxBufferSize=64; //Taille maximale du buffer
char buffer[MaxBufferSize]; //Stockage intermédiaire d'une chaîne de caractères
int bufferSlot=0; //Taille de la chaîne de caractères stoquée
float Value[]={0.0,0.0,0.0}; //Stockage intermédiaire des valeurs d'un déplacement
const float MaxPos[]={250.0,300.0,100.0}; //Dimensions de l'espace de travail
int i; //Pour agir sur les listes déclarées (ci-haut) plus facilement

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void HELP() {
  Serial.print("#\n");
  Serial.println(F("\n[-][-][-][-][-][-][-][-][-][-][-][-][-][-]"));
  Serial.println(F("3 AXIS SYSTEM - TYPE 2 - GROUP 12"));
  Serial.println(F(" "));
  Serial.println(F("Commands:"));
  Serial.println(F("G00 [X(mm)] [Y(mm)] [Z(mm)]; - Rapid linear movement"));
  Serial.println(F("G01 [X(mm)] [Y(mm)] [Z(mm)] [F(mm/s)]; - Linear movement"));
  Serial.println(F("G04 P[seconds]; - Delay"));
  Serial.println(F("G10; - Return To Origin"));
  Serial.println(F("G11; - Return X Axis To Its Origin"));
  Serial.println(F("G12; - Return Y Axis To Its Origin"));
  Serial.println(F("G13; - Return Z Axis To Its Origin"));
  Serial.println(F("G90; - Absolute mode"));
  Serial.println(F("G91; - Relative mode"));
  Serial.println(F("G92 [X(mm)] [Y(mm)] [Z(mm)]; - Manualy update stored position"));
  Serial.println(F("G101; - Activate suction"));
  Serial.println(F("G102; - Deactivate suction"));
  Serial.println(F("M18; - Disable motors"));
  Serial.println(F("M19; - Enable motors"));
  Serial.println(F("M100; - This help message"));
  Serial.println(F("M114; - Stats"));
  Serial.println(F("[-][-][-][-][-][-][-][-][-][-][-][-][-][-]\n"));
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void STATS() {
  unsigned long Time=millis();
  Serial.print("#\n");
  Serial.println(F("\n[-][-][-][-][-][-][-][-][-][-][-][-][-][-]"));
  Serial.print("SYSTEM STATISTICS (Requested At: "); Serial.print(Time); Serial.println(" milliseconds)");
  
  Serial.print("Position (X,Y,Z) = (");
  Serial.print(Position[x]); Serial.print(",");
  Serial.print(Position[y]); Serial.print(",");
  Serial.print(Position[z]); Serial.println(")");

  Serial.print("Distance To Cover (X,Y,Z) = (");
  Serial.print(Distance[x]); Serial.print(",");
  Serial.print(Distance[y]); Serial.print(",");
  Serial.print(Distance[z]); Serial.println(")");

  Serial.print("Speed (X,Y,Z) = (");
  Serial.print(Speed[x]); Serial.print(",");
  Serial.print(Speed[y]); Serial.print(",");
  Serial.print(Speed[z]); Serial.println(")");

  Serial.print("Real Speed = "); Serial.println(DiagonalSpeed);

  Serial.print("Orientation Of Movement (X,Y,Z) = (");
  Serial.print(Sens[x]); Serial.print(",");
  Serial.print(Sens[y]); Serial.print(",");
  Serial.print(Sens[z]); Serial.println(")");
  
  Serial.print("MaxDistance = "); Serial.println(maxDistance);
  Serial.print("ImpulsePeriod="); Serial.println(ImpulsePeriod);
  Serial.println("END OF STATISTICS");
  Serial.println(F("[-][-][-][-][-][-][-][-][-][-][-][-][-][-]\n"));
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void setup() {
  Serial.begin(115200);
  for (i=0; i<=2; i++) {
    //Step
    pinMode(PinStep[i],OUTPUT);
    //Direction
    pinMode(PinDir[i],OUTPUT);
    digitalWrite(PinDir[i],!SensPositif[i]);
    //Enable
    pinMode(PinEn[i],OUTPUT);
    digitalWrite(PinEn[i],HIGH);
    //Capteurs
    pinMode(PinCapt[i],INPUT_PULLUP);
  }
  pinMode(PinSoupape,OUTPUT);
  pinMode(PinPompe,OUTPUT);
  digitalWrite(PinSoupape,HIGH);
  digitalWrite(PinPompe,LOW);
  attachInterrupt(digitalPinToInterrupt(PinCapt[x]),FinDeCourseX,CHANGE);
  attachInterrupt(digitalPinToInterrupt(PinCapt[y]),FinDeCourseY,CHANGE);
  attachInterrupt(digitalPinToInterrupt(PinCapt[z]),FinDeCourseZ,CHANGE);
  delay(1000); ready();
}

/*NOTE: Les boucles sont trop rapides par rapport à la transmission d'information.
 Serial.available devient = 0 pendant la lecture de l'instruction entréé, avant même qu'elle arrive complètement.
 Donc on délaie plus de 1/115200=8.7us pour laisser les infos passer.*/

void loop() {
  if (State==0) { digitalWrite(PinEn[x],HIGH);
                  digitalWrite(PinEn[y],HIGH);
                  digitalWrite(PinEn[z],HIGH); }
  else if (MovingToOrigin==1) {
                  for(i=0;i<=2;i++) {digitalWrite(PinEn[i],LOW);}
                  ReturnToOrigin(); }
  else if (Moving==1) { Move(); }
      
  if (Serial.available()>0) {UseReceivedInfo();}
}
//Il est crucial de vérifier le contenu du Buffer en tout dernier,
//pour donner la priorité à la désactivation des moteurs.

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void UseReceivedInfo() {
    char c;
    do { if (Serial.available()>0) {c = Instruction();} } while (c!=';' && c!='!');
    if (c=='!') {bufferSlot=0; ready();}
    else if (c==';')
         {buffer[bufferSlot]='\0';
          ProcessCommand();
         }
}

char Instruction() {
    char c = Serial.read();
    delay(1);
    //Si le stoquage maximal est dépassé, on signal une erreur.
    if (bufferSlot >= MaxBufferSize-1) { while (Serial.available()>0) {c = Serial.read();}  return('!'); }
    //Si aucune erreur n'a occuré, on ajoute le caractère à la liste.
    buffer[bufferSlot++]=c; return(c);
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void ready() {
  bufferSlot=0; // Réinitialise le buffer
  Serial.print(F(">\n")); // Le signal est prêt à recevoir une instruction
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void ProcessCommand() {
  int InvalidCommand=0;
  float EnteredValue;

  int Command = ParseNumber('G',-1);
  switch(Command) {
    case 0: EnteredValue=ParseNumber('X',(float)(type-1)); Distance[x]=HandleMovement(x, EnteredValue);
            EnteredValue=ParseNumber('Y',(float)(type-1)); Distance[y]=HandleMovement(y, EnteredValue);
            EnteredValue=ParseNumber('Z',(float)(type-1)); Distance[z]=HandleMovement(z, EnteredValue);
            maxDistance=max(abs(Distance[x]),abs(Distance[y])); maxDistance=max(maxDistance,abs(Distance[z]));
            if (maxDistance==0) {Moving=0; ready();}
            else {HandleSpeed(maxDistance, 120.0);}
            break;
    /*Concernant les valeurs par défaut, elles sont toujours des valeurs impossibles. En effet :
     *Relatif => Type=1 => Type-1=0 => EnteredValue=0 (Pas de mouvement relatif)
     *Absolu => Type=0 => Type-1=-1 => EnteredValue=-1 (Position absolue invalide)*/
    case 1: EnteredValue=ParseNumber('X',(float)(type-1)); Distance[x]=HandleMovement(x, EnteredValue);
            EnteredValue=ParseNumber('Y',(float)(type-1)); Distance[y]=HandleMovement(y, EnteredValue);
            EnteredValue=ParseNumber('Z',(float)(type-1)); Distance[z]=HandleMovement(z, EnteredValue);
            EnteredValue=ParseNumber('F',60.0);
            maxDistance=max(abs(Distance[x]),abs(Distance[y])); maxDistance=max(maxDistance,abs(Distance[z]));
            if (maxDistance==0) { Moving=0; ready();}
            else {HandleSpeed(maxDistance, EnteredValue);}
            break;
    case 4: EnteredValue=ParseNumber('P',0.0); EnteredValue=(int)EnteredValue*1000;
            delay(EnteredValue);
            if (EnteredValue<500) delay(500);
            ready(); break;
    case 10: MovingToOrigin=1; break;
    case 11: InitialiseX(); ready(); break;
    case 12: InitialiseY(); ready(); break;
    case 13: InitialiseZ(); ready(); break;
    case 90: type=0; ready(); break;
    case 91: type=1; ready(); break;
    case 92: EnteredValue=ParseNumber('X',Position[x]); Position[x]=EnteredValue;
             EnteredValue=ParseNumber('Y',Position[y]); Position[y]=EnteredValue;
             EnteredValue=ParseNumber('Z',Position[z]); Position[z]=EnteredValue;
             ready(); break;
    case 101:digitalWrite(PinSoupape,LOW); digitalWrite(PinPompe,HIGH); ready(); break;
    case 102:digitalWrite(PinPompe,LOW); digitalWrite(PinSoupape,HIGH); ready(); break;
    default: InvalidCommand+=1; break;
  }
  
  //Si une commande a déjà été détectée,
  //il n'y a aucune raison de chercher la suivante.
  if (InvalidCommand==1) {
    Command=ParseNumber('M',-1);
    switch(Command) {
      case 18: State=0; for (i=0;i<3;i++) {digitalWrite(PinEn[i],HIGH);} ready(); break;
      case 19: State=1; for (i=0;i<3;i++) {digitalWrite(PinEn[i],LOW);} ready(); break;
      case 100: HELP(); delay(500); ready(); break;
      case 114: STATS(); delay(500); ready(); break;
      default: InvalidCommand+=1; break;
    }
  }

  if (InvalidCommand==2) { ready(); }

  else {
       UpdateDirections();
       }
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

float ParseNumber(char Target,float Default) {
  //Un pointeur est utilisé pour lire le contenu du buffer convenablement.
  char *pointer=buffer;
  /*Tant que le pointeur est valide, && qu'il pointe vers un contenu non vide,
   && qu'il ne dépasse pas les normes actuelles du contenu stoqué*/
  while((long)pointer > 1 && (*pointer) && (long)pointer < (long)buffer+bufferSlot) {
    if(*pointer==Target) {  //Si le Target visé est trouvé
      return atof(pointer+1);  //Convertis le nombre qui suit directement ce Targeten float et Return-le.
    }
    pointer=strchr(pointer,' ')+1;  //Pointe vers le premier caractère suivant à succéder un espace.
  }
  return Default;  //Target non trouvé. Return la valeur par défaut.
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

//NOTE TO SELF : In relative mode, make it so you dont have to wait to finish movement if you want to increment the POSITION.

float HandleMovement(int axis, float EnteredValue) {

    float precision = (axis==z)? precisionZ : precisionXY;
    //Value[axis] va enregistrer la distance à parcourir, positive ou négative, de l'axe correspondant.
    if (type == 1) {
        Value[axis] = EnteredValue;
        Value[axis]+=Position[axis];
        Value[axis] = constrain(Value[axis],0,MaxPos[axis]);
        Value[axis]-=Position[axis];
    }
    //Ici, type = 0, donc le mode est absolu, pourtant la position demandée est négative. Erreur.
    else if (EnteredValue < 0.0) { Value[axis]=0.0; }
    else {
        Value[axis] = EnteredValue;
        Value[axis] = constrain(Value[axis],0,MaxPos[axis]);
        Value[axis] -= Position[axis];
        InitialSteps=0;
        //Pour ignorer le mouvement précédent
    }
    
    if (axis == y) { Value[axis]-=Distance[x]; }
    //( Loi du mouvement entre X et Y. )
    //Si le déplacement calculé est négligeable, il n'y a pas de déplacement.
    if (abs(Value[axis]) < precision) { digitalWrite(PinEn[axis], HIGH); return(0.0);
    } else {
        Moving = 1;
        digitalWrite(PinEn[axis], LOW);
        return(Value[axis]);
    }
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void HandleSpeed(float maxDistance, float EnteredSpeed) {
    if (EnteredSpeed <= 0) {return;}

    /*Vitesse Diagonale = Distance Diagonale / Temps
     *Vitesse Individuelle = Distance Individuelle / Temps
                           = (Distance Individuelle)x(Vitesse Diagonale)/(Distance Diagonale) */
    Distance[y]-=Distance[x];
    float DiagonalDistance = Distance[x]*Distance[x] + Distance[y]*Distance[y] + Distance[z]*Distance[z];
          DiagonalDistance = sqrt(DiagonalDistance);
    Distance[y]+=Distance[x];
    //On utilise le déplacement apparent de Y, pas le déplacement du moteur, pour calculer la vitesse. 

    /*Il suffit de calculer la vitesse de l'axe qui parcourera la plus grande distance.
     *Les vitesse des deux autres axes sont une conséquence bien paramétrée de l'algorithme de Bresenham. */

    int axis;
    for (i=0;i<=2;i++) { if (maxDistance==abs(Distance[i])) axis=i; }
    Speed[axis] = abs(Distance[axis]) * EnteredSpeed / DiagonalDistance;
    ImpulsePeriod = 1000000.0/(K[axis]*Speed[axis]);

    if (Distance[x]==0.0 && Distance[y]==0.0) {ImpulsePeriod*=10;}
    if (Distance[x]==0.0 && Distance[z]==0.0) {ImpulsePeriod*=2;}
    
    //On met quand même à jour les deux autres vitesses, pour avoir des statistiques correctes.
    for (i=0;i<=2;i++) { if (i!=axis) Speed[i]=(abs(Distance[i])/maxDistance)*Speed[axis]; }

    DiagonalSpeed = EnteredSpeed;
}


void UpdateDirections() {
   //Sens De Rotation      
   for (i=0;i<=2;i++) {
        if (Distance[i]>0) { Sens[i]=1;
                             digitalWrite(PinDir[i],SensPositif[i]);
                           }
        else if (Distance[i]<0) { Sens[i]=-1;
                                  digitalWrite(PinDir[i],!SensPositif[i]);
                                }
        Distance[i]=abs(Distance[i]);
                      }
}

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void Move() {

  double Steps;
  
  if (maxDistance==0||ImpulsePeriod==0) { ready(); return; }

  /*Pour recevoir les instructions d'urgence.
   *Implicitement, il ne faut pas demander un mouvement puis le suivre d'une autre instruction,
    sans être sùr qu'on veut l'appliquer avant la fin du mouvement.*/

  double maxSteps = max(K[x]*Distance[x],K[y]*Distance[y]);
         maxSteps = max(K[z]*Distance[z],maxSteps);
                  //K*Distance = Nombre De Pas
  float Increment[] = { (K[x]*Distance[x])/maxSteps, (K[y]*Distance[y])/maxSteps, (K[z]*Distance[z])/maxSteps };

  float SendImpulsion[] = {0.0,0.0,0.0};

  digitalWrite(PinEn[x],LOW); digitalWrite(PinEn[y],LOW); digitalWrite(PinEn[z],LOW);
  
  for (Steps=InitialSteps; Steps<maxSteps; Steps++) {

    for (i=0; i<=2; i++) {
       if (SendImpulsion[i]>=1) 
          { digitalWrite(PinStep[i],HIGH);
            SendImpulsion[i]-=1.0;
            Position[i]+=(float)(Sens[i]/K[i]);
            if (i==x) {Position[y]+=(float)(Sens[x]/K[x]);}
          }
       SendImpulsion[i] += Increment[i];
     }

    delayMicroseconds(ImpulsePeriod);

    for (i=0; i<=2; i++) { digitalWrite(PinStep[i],LOW); }
    
    /*On considère que si aucune instruction supplémentaire n'est pas voulue, alors le buffer est vide.
      Le buffer ne se remplira de la prochaine insruction du Gcode, normalement, que si Arduino signal être prêt.
      Recevoir quand même une instruction est donc considéré comme situation de priorité.*/
    /*if (Serial.available()>0) { bufferSlot=0; UseReceivedInfo();
                                if (State==0) { InitialSteps=Steps; break;}
                                else Move(); break; }*/
    //On réinitialise le bufferSlot pour lire la commande correctement. Donc le précédent contenu est effacé.
    
    if (reachedMax[x]) { Position[x]=0; SendImpulsion[x]=0; Increment[x]=0; Distance[x]=0;
                         Position[y]=0; SendImpulsion[y]=0; Increment[y]=0; Distance[y]=0;
                         digitalWrite(PinDir[x],SensPositif[x]); digitalWrite(PinEn[x],LOW);
                         while (reachedMax[x]) {
                                                reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
                                                digitalWrite(PinStep[x],HIGH);
                                                delayMicroseconds(1000);
                                                digitalWrite(PinStep[x],LOW);
                                                delayMicroseconds(1000);}
                         digitalWrite(PinEn[x],HIGH); digitalWrite(PinEn[y],HIGH);
                          }

    if (reachedMax[y]) { Position[y]=0; SendImpulsion[y]=0; Increment[y]=0; Distance[y]=0;
                         int counter=0;
                         digitalWrite(PinDir[y],SensPositif[y]); digitalWrite(PinEn[y],LOW);
                         while (reachedMax[y]||counter<=50) {reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
                                                counter+=1;
                                                digitalWrite(PinStep[y],HIGH); delayMicroseconds(8000);
                                                digitalWrite(PinStep[y],LOW); delayMicroseconds(8000);}
                         digitalWrite(PinEn[y],HIGH); counter=0;
                          }

    if (reachedMax[z]) { Position[z]=0; SendImpulsion[z]=0; Increment[z]=0; Distance[z]=0;
                         digitalWrite(PinDir[z],SensPositif[z]); digitalWrite(PinEn[x],LOW);
                         while (reachedMax[z]) {reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
                                                digitalWrite(PinStep[z],HIGH); delayMicroseconds(8000);
                                                digitalWrite(PinStep[z],LOW); delayMicroseconds(8000);}
                         digitalWrite(PinEn[x],HIGH);
                          }
  }

  //Si le mouvement est terminé
  if (Steps>=maxSteps) {
         Distance[x]=0; digitalWrite(PinEn[x],HIGH);
         Distance[y]=0; digitalWrite(PinEn[y],HIGH);
         Distance[z]=0; digitalWrite(PinEn[z],HIGH);
         Moving=0; ready();

  if (type==1) {Value[x]=0.0; Value[y]=0.0; Value[z]=0.0;}
  }
}

void FinDeCourseX() { reachedMax[x]=(digitalRead(PinCapt[x])==LOW); }
void FinDeCourseY() { reachedMax[y]=(digitalRead(PinCapt[y])==LOW); }
void FinDeCourseZ() { reachedMax[z]=(digitalRead(PinCapt[z])==LOW); }

//[o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o][o]\\

void ReturnToOrigin() {
  
    //Les 3 moteurs tournent au sens négatif.
    for(i=0;i<=2;i++){digitalWrite(PinDir[i],!SensPositif[i]);}

    ImpulsePeriod=2200;
    
    //La vitesse Y est doublée pour avoir un mouvement diagonal.
    int YspeedDoubler = 0;
    
    //Déclaration de tous les états possibles.
    int Deactivated=0,
        START=1,
        NothingFinished=2,
        FinishedX=3,
        FinishedY=4,
        FinishedZ=5,
        JustZremaining=6,
        DONE=7;
    
    //Seul l'un des états possibles est considéré comme l'état actuel à chaque itération.
    int currentState=START;

    //La boucle de mouvement est activéée.
    while (MovingToOrigin == 1) {
        reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
        reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
        reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
        
        switch (currentState) {
          
            //Déterminer l'état actuel des moteurs selon l'état des capteurs.
            case 1:
                if (!reachedMax[x] && !reachedMax[y]) {
                    if (NothingFinished!=Deactivated) currentState = NothingFinished;
                    //Un même état n'est jamais enregistré 2 fois.
                }
                else if (reachedMax[x] && !reachedMax[y]) {
                    if (FinishedX!=Deactivated) currentState = FinishedX;
                }
                else if (!reachedMax[x] && reachedMax[y]) {
                    if (FinishedY!=Deactivated) currentState = FinishedY;
                }
                if (FinishedX==Deactivated && FinishedY==Deactivated && FinishedZ!=Deactivated) {
                    if (JustZremaining!=Deactivated) currentState = JustZremaining;
                }

                if (FinishedX==Deactivated && FinishedY==Deactivated && FinishedZ==Deactivated) {
                    currentState = DONE;
                }
                break;

            //Mouvement Diagonal de X et Y.
            case 2:
                if (!reachedMax[y]) { digitalWrite(PinStep[y], HIGH); }
                //N'envoyer une impulsion à X qu'une fois toutes les 2 itérations.
                if (!reachedMax[x]) { digitalWrite(PinStep[x], HIGH); }
                //L'état du capteur Z est pris en compte dans chaque cas.
                if (FinishedZ!=Deactivated) {
                  if(!reachedMax[z]) { digitalWrite(PinStep[z], HIGH); }
                  else {
                     for (i=0;i<=1;i++) {digitalWrite(PinStep[i], LOW);}
                     FinishedZ=Deactivated;
                     digitalWrite(PinDir[z],SensPositif[z]);  
                     while(reachedMax[z]){
                     //Malgré la présence de attachInterrupt, les tests ont montré qu'il était instable.
                     //On a donc ajouté une vérification supplémentaire.
                       reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
                       digitalWrite(PinStep[z], HIGH);
                       delayMicroseconds(ImpulsePeriod * 0.5);
                       digitalWrite(PinStep[z], LOW);
                       delayMicroseconds(ImpulsePeriod * 0.5);
                       }
                    }
                }
                //Envoi des impulsions.
                delayMicroseconds(ImpulsePeriod * 0.5);
                for (i=0;i<=2;i++) {digitalWrite(PinStep[i], LOW);}
                delayMicroseconds(ImpulsePeriod * 0.5);

                //Si un cas est terminé pour de bon, il n'est plus jamais réutilisé.
                if (reachedMax[x] || reachedMax[y]) {
                    currentState = START;
                    NothingFinished=Deactivated;
                }
                break;

            //Le mouvement de X doit s'arrêter.
            case 3:
            if(reachedMax[x]){
              digitalWrite(PinDir[x], SensPositif[x]);
              digitalWrite(PinDir[y], !SensPositif[y]);
              while(reachedMax[x]&&!reachedMax[y]){
                reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
                reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
                digitalWrite(PinStep[x], HIGH);
                digitalWrite(PinStep[y], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[x], LOW);
                digitalWrite(PinStep[y], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
              }
            }
            
                //S'assurer que le moteur Y tourne bel et bien vers l'arrière.
                digitalWrite(PinDir[y], !SensPositif[y]);
                //Bouger Y et Z uniquement.
                digitalWrite(PinStep[y], HIGH);
                if (FinishedZ!=Deactivated) { 
                if(!reachedMax[z]) { digitalWrite(PinStep[z], HIGH); }
                else {
                for (i=0;i<=1;i++) {digitalWrite(PinStep[i], LOW);}
                FinishedZ=Deactivated;
                digitalWrite(PinDir[z], SensPositif[z]);  
                while(reachedMax[z]){
                reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
                digitalWrite(PinStep[z], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
                }
                } 
                }
                //Envoi des impulsions.
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[y], LOW);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);

                //Si un cas est terminé pour de bon, il n'est plus jamais réutilisé.
                if (reachedMax[y]) {
                digitalWrite(PinDir[y], SensPositif[y]);  
                while(reachedMax[y]){
                reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
                digitalWrite(PinStep[y], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[y], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
              }
                    currentState = START;
                    FinishedX=Deactivated; FinishedY=Deactivated;
                }
                break;

            //Le mouvement APPARENT de Y doit s'arrêter.
            case 4:
                if (reachedMax[y]) {
                digitalWrite(PinDir[y], SensPositif[y]);  
                while(reachedMax[y]){
                reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
                digitalWrite(PinStep[y], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[y], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
              }
                }
                //Le moteur Y tourne dans le sens opposé du moteur X.
                digitalWrite(PinDir[y], SensPositif[y]);
                //Tous les moteurs entrent en mouvement,
                digitalWrite(PinStep[y], HIGH);
                digitalWrite(PinStep[x], HIGH);
                //En prenant en compte l'état de Z.
                if (FinishedZ!=Deactivated) { 
                if(!reachedMax[z]) { digitalWrite(PinStep[z], HIGH); }
                else {
                for (i=0;i<=1;i++) {digitalWrite(PinStep[i], LOW);}
                FinishedZ=Deactivated;
                digitalWrite(PinDir[z], SensPositif[z]);  
                while(reachedMax[z]){
                reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
                digitalWrite(PinStep[z], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
                }
                } 
                }
                //Envoi des impulsions.
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[y], LOW);
                digitalWrite(PinStep[x], LOW);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);

                //Si un cas est terminé pour de bon, il n'est plus jamais réutilisé.
                if (reachedMax[x]) {
              digitalWrite(PinDir[x], SensPositif[x]);
              digitalWrite(PinDir[y], !SensPositif[y]);
              while(reachedMax[x]&&!reachedMax[y]){
              reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
              reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
              digitalWrite(PinStep[x], HIGH);
              digitalWrite(PinStep[y], HIGH);
              delayMicroseconds(ImpulsePeriod * 0.5);
              digitalWrite(PinStep[x], LOW);
              digitalWrite(PinStep[y], LOW);
              delayMicroseconds(ImpulsePeriod * 0.5);
              }
                    currentState = START;
                    FinishedY=Deactivated; FinishedX=Deactivated;
                }
                break;
                
            //Les moteurs X et Y ont atteint leurs positions initiales, mais pas Z.
            case 6:
                digitalWrite(PinStep[z], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);

                if (reachedMax[z]) {
                digitalWrite(PinDir[z], SensPositif[z]);  
                while(reachedMax[z]){
                reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
                digitalWrite(PinStep[z], HIGH);
                delayMicroseconds(ImpulsePeriod * 0.5);
                digitalWrite(PinStep[z], LOW);
                delayMicroseconds(ImpulsePeriod * 0.5);
              }
                    currentState = START;
                    JustZremaining=Deactivated; FinishedZ=Deactivated;
                }
                break;

            case 7:
                    MovingToOrigin = 0;
                    Position[x] = 0.0;
                    Position[y] = 0.0;
                    Position[z] = 0.0;
                    ready();
                break;
        }
    }
}

void InitialiseX() {
  digitalWrite(PinEn[y], HIGH);
  digitalWrite(PinEn[x], LOW);
  digitalWrite(PinDir[x],!SensPositif[x]);
  reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
  while (!reachedMax[x]) {
    reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
    digitalWrite(PinStep[x], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[x], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinDir[x],SensPositif[x]);
  while (reachedMax[x]) {
    reachedMax[x]=(digitalRead(PinCapt[x])==LOW);
    digitalWrite(PinStep[x], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[x], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinEn[x], HIGH);
  Position[x]=0;
}

void InitialiseY() {
  digitalWrite(PinEn[x], HIGH);
  digitalWrite(PinEn[y], LOW);
  digitalWrite(PinDir[y],!SensPositif[y]);
  reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
  while (!reachedMax[y]) {
    reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
    digitalWrite(PinStep[y], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[y], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinDir[y],SensPositif[y]);
  while (reachedMax[y]) {
    reachedMax[y]=(digitalRead(PinCapt[y])==LOW);
    digitalWrite(PinStep[y], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[y], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinEn[y], HIGH);
  Position[y]=0;
}

void InitialiseZ() {
  digitalWrite(PinEn[z], LOW);
  digitalWrite(PinDir[z],!SensPositif[z]);
  reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
  while (!reachedMax[z]) {
    reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
    digitalWrite(PinStep[z], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[z], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinDir[z],SensPositif[z]);
  while (reachedMax[z]) {
    reachedMax[z]=(digitalRead(PinCapt[z])==LOW);
    digitalWrite(PinStep[z], HIGH);
    delayMicroseconds(1500 * 0.5);
    digitalWrite(PinStep[z], LOW);
    delayMicroseconds(1500 * 0.5);
  }
  digitalWrite(PinEn[z], HIGH);
  Position[z]=0;
}
