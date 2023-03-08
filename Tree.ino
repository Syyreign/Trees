// Pins used to check resistance
int analogPins[6] = {0, 1, 2, 3, 4, 5};

// The pins used for the leds
int ledPins[6] = {2, 4, 6, 8, 10, 12};

// Variables to handle checking the resistance
int raw = 0;
int Vin = 5;
float Vout = 0;
float R1 = 220;

float buffer = 0;

// The currently selected stat
int currentStat = 0;

// A struct to contain all the elements of a tree
struct tree{
  String treeName;
  String treeLatinName;
  String Description;
  float stats[7];
  float resistance[2];
  // {lower VI, Upper CM, LI, UI, Upper VI, Lower ML}
  bool locations[6];
};

// Each stat name
String statName[] = {"Fire", "Fungi", "Insects", "Drought", "Nutrient Deficiency", "Wind", "Heat"};

// The struct for each tree
tree arbutus = {"Arbutus", "", "", {1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f}, {150, 250},
  {true, false, false, false, false, false}};
tree sitka = {"Sitka", "", "", {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, {250, 350},
  {true, true, false, false, true, true}};
tree western = {"Western", "", "", {0.5f, 1.0f, 0.0f, 0.5f, 1.0f, 0.5f, 0.5f}, {750, 1250},
  {true, true, true, false, true, true}};
tree douglas = {"Douglas", "", "", {0.75f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 0.75f}, {1500, 2500},
  {true, false, true, true, true, false}};
tree paper = {"Paper", "", "", {0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f}, {4500, 5500},
  {false, false, true, true, false, false}};

void setup() {

  Serial.begin(9600);

  pinMode(27, OUTPUT);
}

void loop() {

  Serial.println("-----------------------------------------");

  int potent = analogRead(A7);
  Serial.println(potent);
  currentStat = getStat(potent);

  // The current tree
  tree currentTree;

  // Loop through each tree node and see if there are any connected
  for(int i=0; i < (sizeof(analogPins) / sizeof(analogPins[0])); i++){
    if(getTree(getPinResistance(analogPins[i]), currentTree)){
      Serial.print(i);
      Serial.print(" R2: ");
      Serial.println(currentTree.treeName);
      
      Serial.print(statName[currentStat]);
      Serial.print(" ");
      Serial.println(currentTree.stats[currentStat]);

      // If the current tree is in a bad place, make a beep and turn off the light
      if(!currentTree.locations[i]){
        Serial.print("BAD LOCATION");
        analogWrite(ledPins[i], 0.0);
        analogWrite(ledPins[i] + 1, 0.0);
        digitalWrite(27, HIGH);
        delay(1000);
        digitalWrite(27, LOW);
        continue;
      }

      Serial.print("Light Level: ");
      float lightLevel = currentTree.stats[currentStat];
      Serial.println(lightLevel);

      // The green colour level of the LED
      analogWrite(ledPins[i], lightLevel * 255);
      Serial.print("inverse Light level: ");

      // The red colour level of the LED
      int inverseLight = (1.0 - lightLevel) * 255;
      Serial.println(inverseLight);
      analogWrite(ledPins[i] + 1, inverseLight);
    }
    // If there is not tree on a node, then keep the light off
    else{
      analogWrite(ledPins[i], 0.0);
      analogWrite(ledPins[i] + 1, 0.0);
      Serial.println("BLINK");
    }
  }

  delay(1);
}

// Get the current stat position based on the potentiometer reading
int getStat(int potentiometer){

  for(int i=1; i<=7; i++){
    if(potentiometer <= 146.5 * i){
      return i - 1;
    }
  }

  return -1;
}

int getPinResistance(int analogPin){
  raw = analogRead(analogPin);
  if(raw){
    buffer = raw * Vin;
    Vout = (buffer)/1024.0;
    buffer = (Vin/Vout) - 1;
    return R1 * buffer;
  }

  return -1;
}

// Gets the tree struct given a resistance
bool getTree(float resistance, tree& outTree){
  if(resistance < 0){
    return false;
  }

  if(resistance > arbutus.resistance[0] && resistance < arbutus.resistance[1]){
    outTree = arbutus;
    return true;
  }
  else if(resistance > sitka.resistance[0] && resistance < sitka.resistance[1]){
    outTree = sitka;
    return true;
  }
  else if(resistance > western.resistance[0] && resistance < western.resistance[1]){
    outTree = western;
    return true;
  }
  else if(resistance > douglas.resistance[0] && resistance < douglas.resistance[1]){
    outTree = douglas;
    return true;
  }
  else if(resistance > paper.resistance[0] && resistance < paper.resistance[1]){
    outTree = paper;
    return true;
  }

  return false;
}
