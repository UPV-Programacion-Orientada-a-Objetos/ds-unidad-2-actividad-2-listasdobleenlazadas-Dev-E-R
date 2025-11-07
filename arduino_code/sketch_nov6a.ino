const char* tramas[] = {
  "L,H",
  "L,O",
  "L,L",
  "M,2",
  "L,A",
  "L,Space",
  "L,W",
  "M,-2",
  "L,O",
  "L,R",
  "L,L",
  "L,D"
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  randomSeed(analogRead(A0));
}

void loop() {
  // put your main code here, to run repeatedly:
  for (const char* trama : tramas) {
    Serial.println(trama);
  }
  delay(1000);
}
