/*

  CanMsg.cpp

*/

#include "CanMsg.h"

gpio_num_t CanMsg::RxPin = GPIO_NUM_4;              // Optional, default Rx pin is GPIO_NUM_4
gpio_num_t CanMsg::TxPin = GPIO_NUM_5;              // Optional, default Tx pin is GPIO_NUM_5
uint32_t CanMsg::DESIRED_BIT_RATE = 1000UL * 1000UL; // 1 Mb/s;

CANMessage frameIn;

void CanMsg::setup()
{
  Serial.printf("[CanMsg %d] : Configure ESP32 CAN\n", __LINE__);
  ACAN_ESP32_Settings settings(DESIRED_BIT_RATE);
  settings.mRxPin = RxPin;
  settings.mTxPin = TxPin;

  uint32_t errorCode;

  // without filter
  errorCode = ACAN_ESP32::can.begin(settings);
  Serial.printf("[CanMsg %d] : config without filter\n", __LINE__);

  if (errorCode == 0)
    Serial.printf("[CanMsg %d] : configuration OK !\n", __LINE__);
  else
  {
    Serial.printf("[CanMsg %d] : configuration error 0x%x\n", __LINE__, errorCode);
    return;
  }
  xTaskCreatePinnedToCore(canReceiveMsg, "CanReceiveMsg", 2 * 1024, NULL, 5, NULL, 0);
}

// Lecture des informations en retour de LaBox
void CanMsg::canReceiveMsg(void *pvParameter)
{
  while (1)
  {
    CANMessage frameIn;
    if (ACAN_ESP32::can.receive(frameIn))
    {
      const uint8_t cmde = (frameIn.id & 0x7F80000) >> 19; // Commande
      const uint8_t exped = (frameIn.id & 0x7F800) >> 11;  // Expéditeur
      const uint8_t resp = (frameIn.id & 0x04) >> 2;       // Commande = 0 / Reponse = 1

      if (resp && exped == 253) // Il s'agit d'une réponse à un envoi de commande pour la Box
      {
        Serial.printf("\n[CanMsg %d]------ Recu de %d : Commande 0x%0X\n", __LINE__, exped, cmde);
        if (frameIn.rtr) // Remote frame
          ACAN_ESP32::can.tryToSend(frameIn);
        else
        {
          switch (cmde)
          {
          case 0xF0:
            if (resp)
            {
              Serial.println("Confirmation commande de traction : ");
              Serial.printf("Loco %d - Vitesse %d - Direction %d\n\n", (frameIn.data[0] << 8) + frameIn.data[1], frameIn.data[2], frameIn.data[3]);
            }
            break;
          case 0xFD:
            if (frameIn.data[0] == 2) // OVERLOAD
              Serial.printf("Power overload.\n");
            else
            {
              Serial.printf("Power %s\n", frameIn.data[0] ? "on" : "off");
              Serial.printf("Mesure de courant : %d\n\n", (frameIn.data[1] << 8) + frameIn.data[2]);
            }
            break;
          case 0xFE:
            if (resp)
            {
              Serial.printf("Confirmation power : %s\n\n", frameIn.data[0] ? "<1>" : "<0>");
            }
            break;
          case 0xFF:
            if (resp)
            {
              Serial.printf("Confirmation pour emergency stop\n\n");
            }
            break;
          }
        }
      }
    }
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

/*--------------------------------------
  Envoi CAN
  --------------------------------------*/

void CanMsg::sendMsg(CANMessage &frame)
{
  if (0 == ACAN_ESP32::can.tryToSend(frame))
    Serial.printf("Echec envoi message CAN\n");
  else
    Serial.printf("Envoi commande 0x%0X\n", (frame.id & 0x7F80000) >> 19);
}

auto formatMsg = [](CANMessage &frame, byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp) -> CANMessage
{
  frame.id |= prio << 27;       // Priorite 0, 1, 2 ou 3
  frame.id |= cmde << 19;       // commande appelée
  frame.id |= thisNodeId << 11; // ID expediteur
  frame.id |= destNodeId << 3;  // ID LaBox
  frame.id |= resp << 2;        // Réponse
  frame.ext = true;
  return frame;
};

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp)
{
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 0;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0)
{
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 1;
  frame.data[0] = data0;
  CanMsg::sendMsg(frame);
}

void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1)
{
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 2;
  frame.data[0] = data0;
  frame.data[1] = data1;
  CanMsg::sendMsg(frame);
}
void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2)
{
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 3;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  CanMsg::sendMsg(frame);
}
void CanMsg::sendMsg(byte prio, byte cmde, byte thisNodeId, byte destNodeId, byte resp, byte data0, byte data1, byte data2, byte data3)
{
  CANMessage frame;
  frame = formatMsg(frame, prio, cmde, thisNodeId, destNodeId, resp);
  frame.len = 4;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  CanMsg::sendMsg(frame);
}
