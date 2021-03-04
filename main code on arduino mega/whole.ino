
// Define pins for ultrasonic and buzzer
int const trigPinfront = 11;
int const echoPinfront = 10;
int const buzzer = 2;
int const trigPinback = 9;
int const echoPinback = 8;
#include <Arduino_FreeRTOS.h>
#include<Wire.h>

#include <TEA5767Radio.h>

#include <LiquidCrystal.h>
int first;
boolean on;
int last;
#define enA 36
TEA5767Radio radio = TEA5767Radio();
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float frequency = 0;
int fUP = A0;
int fDOWN = A1;
int count = 0;
#define enA1 12
#define enB1 12
#define in1 26
#define in2 3
#define in3 4
#define in4 27

#define enA2 12
#define enB2 12
#define in5 28
#define in6 5
#define in7 6
#define in8 29

int control = 1;
int pwmOutput = 255;

///fuel
int const A = 47;
int const B = 48;
int const C = 49;
int const D = 50;
int const E = 51;
int const F = 52;
int const G = 53;
int resval = 0;  // holds the value
int respin = A7;
////////////servo and rain
#include <Servo.h>
Servo servo;
int angle = 10;

int rainPin = A0;
int val = 0;
int threshold = 400;

void windshieldSetup() {
  servo.attach(7);
  servo.write(angle);
}
void radioSetup() {
  pinMode(enA, OUTPUT);


  Wire.begin();
  digitalWrite(enA, HIGH);

  frequency = 90.90;   //starting frequency


  radio.setFrequency(frequency);
  Serial.println("welcome");
}

void fuelSetup() {
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
}
void carSetup() {
  pinMode(enA1, OUTPUT);
  pinMode(enB1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(enA2, OUTPUT);
  pinMode(enB2, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);

  pinMode(trigPinfront, OUTPUT); // trig pin will have pulses output
  pinMode(echoPinfront, INPUT); // echo pin should be input to get pulse width

  pinMode(trigPinback, OUTPUT); // trig pin will have pulses output
  pinMode(echoPinback, INPUT); // echo pin should be input to get pulse width

  pinMode(buzzer, OUTPUT); // buzz pin is output to control buzzering
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  fuelSetup();
  carSetup();
  windshieldSetup();
  radioSetup();

  xTaskCreate(carLoop, "Parallel Parking", 128, NULL, 1, NULL);
  xTaskCreate(fuelLoop, "Water and Servo", 128, NULL, 1, NULL);
  xTaskCreate(windshieldLoop, "Rain", 128, NULL, 1, NULL);
  xTaskCreate(radioloop, "Radio", 128, NULL, 1, NULL);
}
void loop() {
  // put your main code here, to run repeatedly:

}
void turnRight(int pwmOutput) {
  analogWrite(enA1, pwmOutput);
  analogWrite(enB1, pwmOutput);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW );
  digitalWrite(in4, HIGH );

  analogWrite(enA2, pwmOutput);
  analogWrite(enB2, pwmOutput);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  digitalWrite(in7, HIGH );
  digitalWrite(in8, LOW );
}

void turnLeft(int pwmOutput) {
  analogWrite(enA1, pwmOutput);
  analogWrite(enB1, pwmOutput);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH );
  digitalWrite(in4, LOW );

  analogWrite(enA2, pwmOutput);
  analogWrite(enB2, pwmOutput);
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  digitalWrite(in7, LOW );
  digitalWrite(in8, HIGH );
}

void reverse(int pwmOutput) {
  analogWrite(enA1, pwmOutput);
  analogWrite(enB1, pwmOutput);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH );
  digitalWrite(in4, LOW );

  analogWrite(enA2, pwmOutput);
  analogWrite(enB2, pwmOutput);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  digitalWrite(in7, LOW );
  digitalWrite(in8, HIGH );
}

void forward(int pwmOutput) {
  analogWrite(enA1, pwmOutput);
  analogWrite(enB1, pwmOutput);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW );
  digitalWrite(in4, HIGH );

  analogWrite(enA2, pwmOutput);
  analogWrite(enB2, pwmOutput);
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  digitalWrite(in7, HIGH );
  digitalWrite(in8, LOW );
}

void stopNow() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW );
  digitalWrite(in4, LOW );

  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  digitalWrite(in7, LOW );
  digitalWrite(in8, LOW );
}

void carLoop(void *pvParameters) {
  while (1) {
    if (control == 1) {
      delay(2000);
    }
    long durationFront, distanceFront, durationBack, distanceBack;

    Serial.println(control);
    //Serial.println(distanceBack);
    //Serial.println(distanceFront);

    if (control == 1) {
      turnRight(pwmOutput);
      delay(900);
      stopNow();
      //    turnLeft(pwmOutput);
      //    delay(100);
      control = 2;
    }
    if (control == 2) {
      digitalWrite(trigPinback, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinback, LOW);

      durationBack = pulseIn(echoPinback, HIGH);

      distanceBack = durationBack / 58;

      //Serial.println(distanceBack);

      if (distanceBack <= 15) {
        digitalWrite(buzzer, HIGH);
        stopNow();
        control = 3;
        delay(300);
        digitalWrite(buzzer, LOW);
      } else {
        reverse(pwmOutput);
      }
    }
    if (control == 3) {
      turnLeft(pwmOutput);
      delay(700);
      control = 4;
    }
    if (control == 4) {
      digitalWrite(trigPinfront, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinfront, LOW);

      durationFront = pulseIn(echoPinfront, HIGH);

      distanceFront = durationFront / 58;
      //Serial.println(distanceFront);

      if (distanceFront <= 15) {
        Serial.println(distanceFront);
        digitalWrite(buzzer, HIGH);
        stopNow();
        control = 5;
        delay(300);
        digitalWrite(buzzer, LOW);
      } else {
        forward(pwmOutput);
      }
    }
    if (control == 5) {
      stopNow();
    }
  }
}


void fuelLoop(void * pvParameters) {
  while (1) {
    resval = analogRead(respin); //Read data from analog pin and store it to resval variable
    Serial.println(resval);
    if (resval <= 100) {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, HIGH);
    }
    else if (resval > 100 && resval <= 350) {
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      digitalWrite(E, HIGH);
      digitalWrite(F, HIGH);
      digitalWrite(G, HIGH);
    } else if (resval > 350) {
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, HIGH);
      digitalWrite(G, LOW);
    }

    delay(1000);
  }
}

void windshieldLoop(void *pvParameters) {
  while (1) {
    val = analogRead(rainPin);
    Serial.println(val);

    if (val < threshold )
    {
      // scan from 0 to 180 degrees
      for (angle = 10; angle < 180; angle++)
      {
        servo.write(angle);
        delay(15);
      }
      // now scan back from 180 to 0 degrees
      for (angle = 180; angle > 10; angle--)
      {
        servo.write(angle);
        delay(15);
      }
    }

    else
    {
      servo.write(0);
    }

    delay(1000);
  }
}
void radioloop(void * pvParameters) {
  while (1) {
    Serial.println("hi");
    Wire.requestFrom(5, 3);
    Serial.println(Wire.available());

    while (Wire.available()) {



      first = Wire.read();
      on = Wire.read();
      last = Wire.read();

      Serial.println("first");

      Serial.println(first);
      Serial.println("last");

      Serial.println(last);

      if (on == true) {
        Serial.println("hi");
        digitalWrite(enA, HIGH);
      }
      else {
        Serial.println("hi2");

        digitalWrite(enA, LOW);

      }
      Serial.println(last);
      if (last >= 10)
      {
        frequency = first + last / 100.0; //starting frequency
      }
      else
      {
        frequency = first + last / 10.0;
      }
      Serial.println(frequency);
      radio.setFrequency(frequency);

    }

    delay(1000);
  }
}
