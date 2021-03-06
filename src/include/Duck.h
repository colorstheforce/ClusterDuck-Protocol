#ifndef DUCK_H
#define DUCK_H

#include <Arduino.h>
#include <WString.h>

#include "../DuckError.h"
#include "DuckLora.h"
#include "DuckNet.h"
#include "DuckTypes.h"
#include "cdpcfg.h"


class Duck {

public:
  /**
   * @brief Construct a new Duck object.
   *
   */
  Duck() {}
  /**
   * @brief Construct a new Duck object.
   *
   * @param id a unique id
   */
  Duck(String id);

  ~Duck() {}

  /**
   * @brief Setup serial connection.
   *
   * @param baudRate default: 115200
   */
  void setupSerial(int baudRate = 115200);

  /**
   * @brief Setup the radio component
   *
   * @param band      radio frequency in Mhz (default: 915.0)
   * @param ss        slave select pin (default CDPCFG_PIN_LORA_CS)
   * @param rst       reset pin  (default: CDPCFG_PIN_LORA_RST)
   * @param di0       dio0 interrupt pin (default: CDPCFG_PIN_LORA_DIO0)
   * @param di1       dio1 interrupt pin (default: CDPCFG_PIN_LORA_DIO1)
   * @param txPower   transmit power (default: CDPCFG_RF_LORA_TXPOW)
   */
  void setupRadio(float band = CDPCFG_RF_LORA_FREQ, int ss = CDPCFG_PIN_LORA_CS,
                  int rst = CDPCFG_PIN_LORA_RST, int di0 = CDPCFG_PIN_LORA_DIO0,
                  int di1 = CDPCFG_PIN_LORA_DIO1,
                  int txPower = CDPCFG_RF_LORA_TXPOW);

  /**
   * @brief Setup WiFi access point.
   *
   * @param accessPoint a string representing the access point. Default to
   * "🆘 DUCK EMERGENCY PORTAL"
   */
  void setupWifi(const char* ap = "🆘 DUCK EMERGENCY PORTAL");

  /**
   * @brief Setup DNS.
   *
   * @returns DUCK_ERROR_NONE if successful, an error code otherwise.
   */
  int setupDns();

  /**
   * @brief Setup web server.
   *
   * The WebServer is used to communicate with the Duck over ad-hoc WiFi
   * connection.
   *
   * @param createCaptivePortal set to true if Captive WiFi connection is
   * needed. Defaults to false
   * @param html A string representing custom HTML code used for the portal.
   * Default is an empty string Default portal web page is used if the string is
   * empty
   */
  void setupWebServer(bool createCaptivePortal = false, String html = "");

  /**
   * @brief Setup internet access.
   *
   * @param ssid        the ssid of the WiFi network
   * @param password    password to join the network
   */
  void setupInternet(String ssid, String password);

  /**
   * @brief
   *
   */
  void setupOTA();

  /**
   * @brief Send a duck LoRa message.
   *
   * @param msg         the message payload (optional: if not provided, it will
   * be set to an empty string)
   * @param topic       the message topic (optional: if not provided, it will be
   * set to "status")
   * @param senderId    the sender id (optional: if not provided, it will be set
   * to the duck device id)
   * @param messageId   the message id (optional: if not provided, a unique id
   * will be generated)
   * @param path        the message path to append the device id to (optional:
   * if not provided, the path will only contain the duck's device id)
   * @returns DUCK_ERR_NONE if success, an error code otherwise.
   */

  int sendPayloadStandard(String msg = "", String topic = "",
                          String senderId = "", String messageId = "",
                          String path = "");

  /**
   * @brief Check wifi connection status
   * 
   * @returns true if device wifi is connected, false otherwise. 
   */
  bool isWifiConnected() { return duckNet->isWifiConnected(); }
  /**
   * @brief Check if the give access point is available.
   * 
   * @param ssid access point to check
   * @returns true if the access point is available, false otherwise.
   */
  bool ssidAvailable(String ssid) { return duckNet->ssidAvailable(ssid); }

  /**
   * @brief Get the access point ssid
   * 
   * @returns the wifi access point as a string
   */
  String getSsid() { return duckNet->getSsid(); }
  /**
   * @brief Get the wifi access point password.
   * 
   * @returns the wifi access point password as a string. 
   */
  String getPassword() { return duckNet->getPassword(); }

  
protected:
  String deviceId;
  DuckLora* duckLora = DuckLora::getInstance();
  DuckNet* duckNet = DuckNet::getInstance();

  /**
   * @brief Tell the duck radio to start receiving packets from the mesh network
   *
   * @return DUCK_ERR_NONE if successful, an error code otherwise
   */
  int startReceive();
  
  /**
   * @brief Tell the duck radio to start receiving packets from the mesh network
   *
   * @return DUCK_ERR_NONE if successful, an error code otherwise
   */
  int startTransmit();

  /**
   * @brief Implement the duck's specific behavior.
   * 
   * This method must be implemented by the Duck's concrete classes such as DuckLink, MamaDuck,...
   */
  virtual void run() = 0;

  /**
   * @brief Setup a duck with default settings
   *
   * The default implementation simply initializes the serial interface.
   * It can be overriden by each concrete Duck class implementation.
   */
  virtual void setupWithDefaults(String ssid, String password) {
    duckNet->setDeviceId(deviceId);
    setupSerial();
  }

  virtual int getType() = 0;

  virtual int reconnectWifi(String ssid, String password) { return 0; }

  static volatile bool receivedFlag;
  static void toggleReceiveFlag() { receivedFlag = !receivedFlag; }
  static void setReceiveFlag(bool value) { receivedFlag = value; }
  static bool getReceiveFlag() { return receivedFlag; }

  static void onPacketReceived();

  static bool imAlive(void*);
  static bool reboot(void*);

  /**
   * @brief Handle request from emergency portal.
   *
   */
  void processPortalRequest();

  /**
   * @brief Handle over the air firmware update.
   *
   */
  void handleOtaUpdate();
};

#endif