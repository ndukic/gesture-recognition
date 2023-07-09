#include <Arduino_LSM9DS1.h>

const float pragUbrzanja = 2.5;
const int brojUzoraka = 119; // citanje podataka sa senzora je 119Hz, 119 uzoraka predstavlja jednu sekundu

int brojProcitanihUzoraka = brojUzoraka;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("IMU ne moze da se inicijalizuje!");
    while (1);
  }

  Serial.println("aX, aY, aZ");
}

void loop() {
  float aX, aY, aZ;

  // cekanje na znacajan pokret
  while (brojProcitanihUzoraka == brojUzoraka) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);

      float sumaUbrzanjaSvihOsa = fabs(aX) + fabs(aY) + fabs(aZ);

      if (sumaUbrzanjaSvihOsa >= pragUbrzanja) {
        brojProcitanihUzoraka = 0;
        break;
      }
    }
  }

  while (brojProcitanihUzoraka < brojUzoraka) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);

      brojProcitanihUzoraka++;

      Serial.print(aX, 3);
      Serial.print(',');
      Serial.print(aY, 3);
      Serial.print(',');
      Serial.print(aZ, 3);
      Serial.println();

      if (brojProcitanihUzoraka == brojUzoraka) {
        Serial.println();
      }
    }
  }
}