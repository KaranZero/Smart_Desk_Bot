// Bot_Move.h

#ifndef BOT_MOVE_H
#define BOT_MOVE_H

int Motor_speed = 6;

// Motor Pins
int M1_p1 = 5;
int M1_P2 = 7;

int M2_p1 = 9;
int M2_P2 = 10;

void Bot_init() {
  pinMode(M1_p1, OUTPUT);
  pinMode(M1_P2, OUTPUT);
  pinMode(M2_p1, OUTPUT);
  pinMode(M2_P2, OUTPUT);
  pinMode(Motor_speed, OUTPUT);
  
  // Set motors to full speed
  digitalWrite(Motor_speed, HIGH);
}

void Bot_forward() {
  digitalWrite(M1_p1, HIGH);
  digitalWrite(M1_P2, LOW);
  digitalWrite(M2_p1, HIGH);
  digitalWrite(M2_P2, LOW);
}

void Bot_backward() {
  digitalWrite(M1_p1, LOW);
  digitalWrite(M1_P2, HIGH);
  digitalWrite(M2_p1, LOW);
  digitalWrite(M2_P2, HIGH);
}

void Bot_stop() {
  digitalWrite(M1_p1, LOW);
  digitalWrite(M1_P2, LOW);
  digitalWrite(M2_p1, LOW);
  digitalWrite(M2_P2, LOW);
}

void Bot_turnLeft() {
  digitalWrite(M1_p1, LOW);
  digitalWrite(M1_P2, HIGH);
  digitalWrite(M2_p1, HIGH);
  digitalWrite(M2_P2, LOW);
}

void Bot_turnRight() {
  digitalWrite(M1_p1, HIGH);
  digitalWrite(M1_P2, LOW);
  digitalWrite(M2_p1, LOW);
  digitalWrite(M2_P2, HIGH);
}

#endif
