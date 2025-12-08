#include <Arduino.h>
const int ENA = 5;   
const int IN1 = 7;   
const int IN2 = 8;   
const int ENB = 6;   
const int IN3 = 9;   
const int IN4 = 4;   
const int trigFront = 10, echoFront = 11;
const int trigLeft  = A1,  echoLeft  = A2;
const int trigRight = A3,  echoRight = A4;
const int buttonPin = 12;    
const int ledPin    = 13;
const int   CRUISE_PWM = 255;      
const int   MAX_PWM    = 255;
const float STOP_CM  = 28.0f;    
const float SIDE_CLEAR_CM  = 28.0f;
//const float SIDE_MIN_VALID = 3.0f;  //  floor splash
const unsigned long PIVOT_MS       = 850;   
const unsigned long STRAIGHT1_MS   = 1100;
const unsigned long STRAIGHT2_MS   = 1400;
const unsigned long STRAIGHT3_MS   = 1100;
const unsigned long FINAL_ALIGN_MS = 650;
enum State { IDLE, CRUISE, STOP_DECIDE, PASS_EXEC }; //constants
State state = IDLE;
bool systemOn = false;
unsigned long stateStart = 0;
enum Side { PASS_LEFT, PASS_RIGHT };
Side passSide = PASS_LEFT;
enum PassStage { PIVOT1, STRAIGHT_1, PIVOT2, STRAIGHT_2, PIVOT3, STRAIGHT_3, FINAL_ALIGN, PASS_DONE };
PassStage pstage = PASS_DONE;  //pstage stores current manuever in pass stage
unsigned long stageStart = 0;
bool lastButton = HIGH;
unsigned long bootTime = 0;
const unsigned long BOOT_HOLD_MS = 1000;
const int DECIDE_SAMPLES = 10;                 // Vote debounce ()
const int DECIDE_MIN_HITS = 6;                 // need 6/10 (seems to work)
const unsigned long DECIDE_SAMPLE_INTERVAL_MS = 25;
bool deciding = false;                         // To determine if in sampling mode (/*follow*/)
int  sampleCount = 0;
int  frontHits = 0, leftHits = 0, rightHits = 0;
unsigned long lastSampleAt = 0;
float readDistanceCM(int trigPin, int echoPin);
void changeState(State s);
void changeStage(PassStage ps);
void  runCruise(float dFront, float dLeft, float dRight);
void  runStopDecide();                         
void  runPass(float dFront, float dLeft, float dRight);
void leftForward(int pwm);
void leftBackward(int pwm);
void rightForward(int pwm);
void rightBackward(int pwm);
void stopMotors();
void driveForward(int pwm);
void spinLeft(int pwm);   
void spinRight(int pwm);  
void setup() {
Serial.begin(9600);
pinMode(ENA, OUTPUT); 
pinMode(IN1, OUTPUT); 
pinMode(IN2, OUTPUT);
pinMode(ENB, OUTPUT); 
pinMode(IN3, OUTPUT); 
pinMode(IN4, OUTPUT);
digitalWrite(ENA, LOW); 
digitalWrite(ENB, LOW);
digitalWrite(IN1, LOW); 
digitalWrite(IN2, LOW);
digitalWrite(IN3, LOW); 
digitalWrite(IN4, LOW);
pinMode(trigFront, OUTPUT); 
pinMode(echoFront, INPUT);
pinMode(trigLeft,  OUTPUT); 
pinMode(echoLeft,  INPUT);
pinMode(trigRight, OUTPUT); 
pinMode(echoRight, INPUT);
pinMode(buttonPin, INPUT_PULLUP);
pinMode(ledPin, OUTPUT);
  stopMotors();
  digitalWrite(ledPin, LOW);
  state = IDLE;
  systemOn = false;
  bootTime = millis();
}
void loop() {
  if (millis() - bootTime < BOOT_HOLD_MS) { stopMotors(); return; }
  bool b = digitalRead(buttonPin);
  if (lastButton == HIGH && b == LOW) {
    systemOn = !systemOn;
    if (!systemOn) 
    { state = IDLE; stopMotors(); digitalWrite(ledPin, LOW); }
    else          
     { changeState(CRUISE); digitalWrite(ledPin, HIGH); }
    delay(180);
  }
  lastButton = b;
  if (!systemOn) { stopMotors(); return; }
  float dFront = readDistanceCM(trigFront, echoFront);
  float dLeft  = readDistanceCM(trigLeft,  echoLeft);
  float dRight = readDistanceCM(trigRight, echoRight);
  //if (dLeft  > 0 && dLeft  < SIDE_MIN_VALID)  dLeft  = -1.0f;
  //if (dRight > 0 && dRight < SIDE_MIN_VALID)  dRight = -1.0f;
  Serial.print("Front: "); Serial.print(dFront); Serial.print(" cm  |  ");
  Serial.print("Left: ");  Serial.print(dLeft);  Serial.print(" cm  |  ");
  Serial.print("Right: "); Serial.print(dRight); Serial.print(" cm  |  ");
  Serial.print("  State: ");
  switch(state){
    case IDLE:        Serial.println("IDLE"); break;
    case CRUISE:      Serial.println("CRUISE"); break;
    case STOP_DECIDE: Serial.println("STOP_DECIDE"); break;
    case PASS_EXEC:   Serial.println("PASS_EXEC"); break;
  }
  switch (state) {
    case IDLE:        stopMotors(); break;
    case CRUISE:      runCruise(dFront, dLeft, dRight); break;
    case STOP_DECIDE: runStopDecide(); break;              
    case PASS_EXEC:   runPass(dFront, dLeft, dRight); break;
  }

  delay(10);
}
void runCruise(float dFront, float dLeft, float dRight) {
  bool frontBlocked = (dFront > 0 && dFront <= STOP_CM);
  bool leftBlocked  = (dLeft  > 0 && dLeft  <= SIDE_CLEAR_CM);
  bool rightBlocked = (dRight > 0 && dRight <= SIDE_CLEAR_CM);
  if (frontBlocked || leftBlocked || rightBlocked) {
    stopMotors();
    changeState(STOP_DECIDE);
    return;
  }
  driveForward(CRUISE_PWM);
}
void runStopDecide() {
  stopMotors();
  if (!deciding) {
    deciding = true;
    sampleCount = 0;
    frontHits = leftHits = rightHits = 0;
    lastSampleAt = 0;
    Serial.println("STOP_DECIDE: ");
  }
  if (lastSampleAt == 0 || (millis() - lastSampleAt) >= DECIDE_SAMPLE_INTERVAL_MS) {
    lastSampleAt = millis();
    float f = readDistanceCM(trigFront, echoFront);
    float l = readDistanceCM(trigLeft,  echoLeft);
    float r = readDistanceCM(trigRight, echoRight);
    bool fBlocked = (f > 0 && f <= STOP_CM);
    bool lBlocked = (l > 0 && l <= SIDE_CLEAR_CM);
    bool rBlocked = (r > 0 && r <= SIDE_CLEAR_CM);
    if (fBlocked) frontHits = frontHits+ 1; //++
    if (lBlocked) leftHits = leftHits + 1;
    if (rBlocked) rightHits = rightHits+ 1 ;
    sampleCount = sampleCount + 1;
    Serial.print("VOTE "); Serial.print(sampleCount); Serial.print("/");
    Serial.print(DECIDE_SAMPLES);
    Serial.print("  f:"); 
    Serial.print(frontHits);
    Serial.print(" l:");  
     Serial.print(leftHits);
    Serial.print(" r:");  
     Serial.println(rightHits);
  }
  if (sampleCount < DECIDE_SAMPLES) return;
  bool frontBlockedDeb = (frontHits >= DECIDE_MIN_HITS);
  bool leftBlockedDeb  = (leftHits  >= DECIDE_MIN_HITS);
  bool rightBlockedDeb = (rightHits >= DECIDE_MIN_HITS);
  Serial.println("Vote Summary");
  Serial.print("Front Blocked Votes: "); 
  Serial.print(frontHits);
  Serial.print("/"); 
  Serial.print(DECIDE_SAMPLES);
  Serial.print("  => "); 
  Serial.println(frontBlockedDeb ? "BLOCKED" : "CLEAR");
  Serial.print("Left Blocked Votes:  "); Serial.print(leftHits);
  Serial.print("/"); Serial.print(DECIDE_SAMPLES);
  Serial.print("  => "); Serial.println(leftBlockedDeb ? "BLOCKED" : "CLEAR");
  Serial.print("Right Blocked Votes: "); Serial.print(rightHits);
  Serial.print("/"); Serial.print(DECIDE_SAMPLES);
  Serial.print("  => "); Serial.println(rightBlockedDeb ? "BLOCKED" : "CLEAR");
  deciding = false; //Need reset for sampler
  if (frontBlockedDeb && leftBlockedDeb && rightBlockedDeb) {
    Serial.println("Vote Result: all blocked => wait");
    return;
  }
  if (leftBlockedDeb && !rightBlockedDeb) {
    passSide = PASS_RIGHT;
  } else if (rightBlockedDeb && !leftBlockedDeb) {
    passSide = PASS_LEFT;
  } else if (frontBlockedDeb && !leftBlockedDeb && !rightBlockedDeb) {
    passSide = PASS_LEFT; 
  } else if (!frontBlockedDeb && !leftBlockedDeb && !rightBlockedDeb) {
    Serial.println("Vote Result: clear => CRUISE");
    changeState(CRUISE);
    return;
  } else {
    Serial.println("Vote Result: Wait");
    return;
  }
  Serial.println("===== Pass Decision =====");
  if (passSide == PASS_LEFT) {
    Serial.println("LEFT");
  } else {
    Serial.println("RIGHT");
  }
  changeState(PASS_EXEC);
  changeStage(PIVOT1);
}
void runPass(float dFront, float dLeft, float dRight) {
  unsigned long t = millis() - stageStart;
  switch (pstage) {
    case PIVOT1:
      if (passSide == PASS_LEFT)  
      spinLeft(255);
      else                       
       spinRight(255);
      if (t >= PIVOT_MS) changeStage(STRAIGHT_1);
      break;
    case STRAIGHT_1:
      driveForward(255);
      if (t >= STRAIGHT1_MS) changeStage(PIVOT2);
      break;
    case PIVOT2:
      if (passSide == PASS_LEFT)  
      spinRight(255);  
      else                        
      spinLeft(255);
      if (t >= PIVOT_MS) changeStage(STRAIGHT_2);
      break;
    case STRAIGHT_2:
      driveForward(255);
      if (t >= STRAIGHT2_MS) changeStage(PIVOT3);
      break;
    case PIVOT3:
      if (passSide == PASS_LEFT)  
      spinRight(255);  
      else                       
       spinLeft(255);
      if (t >= PIVOT_MS) changeStage(STRAIGHT_3);
      break;
    case STRAIGHT_3:
      driveForward(255);
      if (t >= STRAIGHT3_MS) changeStage(FINAL_ALIGN);
      break;
    case FINAL_ALIGN:
      if (passSide == PASS_LEFT)  
      spinLeft(255);
      else                        
      spinRight(255);
      if (t >= FINAL_ALIGN_MS) changeStage(PASS_DONE);
      break;
    case PASS_DONE:
      stopMotors();
      
      if (dFront <= 0 || dFront > STOP_CM) 
      changeState(CRUISE);
      else                                  
      changeState(STOP_DECIDE);
      break;
  }
}
void changeState(State s) {
  state = s;
  stateStart = millis();          //Reset for vote sampler
  if (s != STOP_DECIDE) {
    deciding = false;
    sampleCount = 0;
    frontHits = leftHits = rightHits = 0;
    lastSampleAt = 0;
  }
}
void changeStage(PassStage ps) { pstage = ps; stageStart = millis(); }
float readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long us = pulseIn(echoPin, HIGH, 30000); //Used max wait time
  if (us == 0) return -1.0f; //No echo
  return us * 0.0343f * 0.5f; //Time to distance conversion
}
void leftForward(int pwm)   { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  analogWrite(ENA, pwm); }
 void leftBackward(int pwm)  { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); analogWrite(ENA, pwm); }
    void rightForward(int pwm)  { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  analogWrite(ENB, pwm); }
  void rightBackward(int pwm) { digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); analogWrite(ENB, pwm); }
void stopMotors()         
  { analogWrite(ENA, 0); analogWrite(ENB, 0); }
void driveForward(int pwm) {
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); 
  digitalWrite(IN4, LOW);
  analogWrite(ENA, pwm);  
  analogWrite(ENB, pwm);
}
void spinLeft(int pwm)  { leftBackward(pwm); rightForward(pwm); }
void spinRight(int pwm) { leftForward(pwm);  rightBackward(pwm); }
