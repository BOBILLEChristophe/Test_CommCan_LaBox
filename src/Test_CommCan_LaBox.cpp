/*

  Test de communication CXAN avec LaBox Locoduino

  Christophe Bobille - Locoduino

  v 0.3 - 08/12/23
  v 0.4 - 09/12/23 - Optimisation de la fonction 0xFE
  v 0.5 - 09/12/23 - Ajout de la reception de messages en provenance de la centrale LaBox.
                     Pour ce test, c'est la mesure de courant qui a été choisie
  v 0.5.2 - 09/12/23
  v 0.5.3 - 10/12/23
  v 0.5.4 - 10/12/23 : Add POWERMODE::OVERLOAD
    /*******************************************************************************************************
  v 0.6.0 - 11/12/23 : Presentation en classes et méthodes 
                       Adoption d'un nouveau format de messages totalement incompatible avec les anciens
  v 0.6.1 - 11/12/23 : Ajout du retour d'informations
                       Ajouts de commandes dont la POM  case 0xF7:
                                                        // WRITE CV on MAIN <w CAB CV VALUE>
  v 0.6.2 - 21/02/24 : Modification de l'identifiant long   
  v 0.6.3 - 08/12/24 : Diverses MAJ   
*/

#ifndef ARDUINO_ARCH_ESP32
#error "Select an ESP32 board"
#endif

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "CanMsg.h"
#include "LaBoxCmd.h"

const uint32_t myID = 0x1811;     // Identifiant de cet expéditeur

Loco *loco = new Loco;

enum : byte
{
  off,
  on
};

LaBoxCmd laBox(myID);

void setup()
{
  Serial.begin(115200);
  CanMsg::setup();

  loco->address = 78; // Renseigner l'adresse de votre locomotive
}

void loop()
{
  /*------------------------------------------------------------------
   Serie de commandes envoyées a LaBox pour tests
  --------------------------------------------------------------------*/

  // Power on
  laBox.setPower(on);
  delay(100);

  // Test des differentes fonctions du decodeur
  for (byte i = 0; i <= 28; i++)
  {
    // Activation
    loco->fn[i] = on;
    laBox.setFunction(loco, i);
    delay(1000);

    // Desactivation
    loco->fn[i] = off;
    laBox.setFunction(loco, i);
    delay(1000);
  }

  // Active les feux et le bruit de la locomotive
  laBox.toggleFunction(loco, 0);
  laBox.toggleFunction(loco, 1);
  delay(10);

    // Vers l'avant
  loco->direction = 1;
  laBox.setThrottle(loco);
  delay(15000);

  // Vitesse 25
  loco->speed = 25;
  laBox.setThrottle(loco);
  delay(15000);

  // Arret de la locomotive
  loco->speed = 0;
  laBox.setThrottle(loco);
  delay(5000);

  // Vitesse 50
  loco->speed = 50;
  laBox.setThrottle(loco);
  delay(15000);

  // Vers l'arriere
  loco->direction = 0;
  laBox.toggleThrottleDir(loco);
  delay(100);

  // Vitesse 75
  loco->speed = 75;
  laBox.setThrottle(loco);
  delay(15000);

  // Vitesse 10
  loco->speed = 10;
  laBox.setThrottle(loco);
  delay(5000);

  // emergency stop
  laBox.emergency();
  delay(1000);

  // power off
  laBox.setPower(off);
  delay(5000);
}
