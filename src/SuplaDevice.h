/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SUPLADEVICE_H
#define SUPLADEVICE_H

#include "supla-common/proto.h"
#include "supla/network/network.h"
#include "supla/storage/config.h"
#include "supla/uptime.h"
#include "supla/clock/clock.h"
#include "supla/storage/config.h"
#include "supla/device/last_state_logger.h"
#include "supla/action_handler.h"

#define ACTIVITY_TIMEOUT 30

#define STATUS_UNKNOWN                   -1
#define STATUS_ALREADY_INITIALIZED       1
#define STATUS_MISSING_NETWORK_INTERFACE 2
#define STATUS_UNKNOWN_SERVER_ADDRESS    3
#define STATUS_UNKNOWN_LOCATION_ID       4
#define STATUS_INITIALIZED               5
#define STATUS_SERVER_DISCONNECTED       6
#define STATUS_ITERATE_FAIL              7
#define STATUS_NETWORK_DISCONNECTED      8

#define STATUS_REGISTER_IN_PROGRESS      10  // Don't change
#define STATUS_REGISTERED_AND_READY      17  // Don't change

#define STATUS_TEMPORARILY_UNAVAILABLE   21
#define STATUS_INVALID_GUID              22
#define STATUS_CHANNEL_LIMIT_EXCEEDED    23
#define STATUS_PROTOCOL_VERSION_ERROR    24
#define STATUS_BAD_CREDENTIALS           25
#define STATUS_LOCATION_CONFLICT         26
#define STATUS_CHANNEL_CONFLICT          27
#define STATUS_DEVICE_IS_DISABLED        28
#define STATUS_LOCATION_IS_DISABLED      29
#define STATUS_DEVICE_LIMIT_EXCEEDED     30
#define STATUS_REGISTRATION_DISABLED     31
#define STATUS_MISSING_CREDENTIALS       32
#define STATUS_INVALID_AUTHKEY           33
#define STATUS_NO_LOCATION_AVAILABLE     34
#define STATUS_UNKNOWN_ERROR             35

#define STATUS_CONFIG_MODE               40
#define STATUS_SOFTWARE_RESET            41
#define STATUS_SW_DOWNLOAD               50

typedef void (*_impl_arduino_status)(int status, const char *msg);

namespace Supla {
  namespace Device {
    class SwUpdate;
  };
};

class SuplaDeviceClass : public Supla::ActionHandler {
 public:
  SuplaDeviceClass();
  ~SuplaDeviceClass();

  void fillStateData(TDSC_ChannelState &channelState);
  void addClock(Supla::Clock *clock);
  Supla::Clock *getClock();

  bool begin(const char GUID[SUPLA_GUID_SIZE],
             const char *Server,
             const char *email,
             const char authkey[SUPLA_AUTHKEY_SIZE],
             unsigned char version = 16);

  bool begin(unsigned char version = 16);

  // Use ASCII only in name
  void setName(const char *Name);

  void setGUID(const char GUID[SUPLA_GUID_SIZE]);
  void setAuthKey(const char authkey[SUPLA_AUTHKEY_SIZE]);
  void setEmail(const char *email);
  void setServer(const char *server);
  void setSwVersion(const char *);
  void setManufacurerId(_supla_int16_t);
  void setProductId(_supla_int16_t);
  void addFlags(_supla_int_t);
  void removeFlags(_supla_int_t);

  int generateHostname(char*, int macSize = 6);

  // Timer with 100 Hz frequency (10 ms)
  void onTimer(void);
  // TImer with 2000 Hz frequency (0.5 ms)
  void onFastTimer(void);
  void iterate(void);

  void setStatusFuncImpl(_impl_arduino_status impl_arduino_status);
  void setServerPort(int value);

  void onVersionError(TSDC_SuplaVersionError *version_error);
  void onRegisterResult(TSD_SuplaRegisterDeviceResult *register_device_result);
  void channelSetActivityTimeoutResult(
      TSDC_SuplaSetActivityTimeoutResult *result);
  void onGetUserLocaltimeResult(TSDC_UserLocalTimeResult *result);
  int handleCalcfgFromServer(TSD_DeviceCalCfgRequest *request);

  void enterConfigMode();
  void enterNormalMode();
  // Schedules timeout to restart device. When provided timeout is 0
  // then restart will be done asap.
  void scheduleSoftRestart(int timeout = 0);
  void softRestart();
  void saveStateToStorage();
  void disableCfgModeTimeout();
  void resetToFactorySettings();

  int getCurrentStatus();
  void loadDeviceConfig();
  bool prepareLastStateLog();
  char *getLastStateLog();
  void addLastStateLog(const char*);
  void setRsaPublicKeyPtr(const uint8_t *ptr);
  const uint8_t *getRsaPublicKey();

  void handleAction(int event, int action) override;

  // Enables automatic software reset of device in case of network/server
  // connection problems longer than timeSec.
  // timeSec is always round down to multiplication of 10 s.
  // timeSec <= 60 will disable automatic restart.
  void setAutomaticResetOnConnectionProblem(unsigned int timeSec);

  void setLastStateLogger(Supla::Device::LastStateLogger *logger);

 protected:
  void *srpc = nullptr;
  int8_t registered = 0;
  int port = -1;
  unsigned int connectionFailCounter = 0;
  unsigned int lastConnectionResetCounter = 0;
  int networkIsNotReadyCounter = 0;

  unsigned long lastIterateTime = 0;
  unsigned long waitForIterate = 0;
  unsigned long deviceRestartTimeoutTimestamp = 0;
  unsigned int forceRestartTimeMs = 0;
  unsigned int resetOnConnectionFailCounter = 0;

  enum Supla::DeviceMode deviceMode = Supla::DEVICE_MODE_NOT_SET;
  int currentStatus = STATUS_UNKNOWN;
  bool goToConfigModeAsap = false;
  bool triggerResetToFacotrySettings = false;
  bool triggerStartLocalWebServer = false;
  bool triggerStopLocalWebServer = false;
  bool triggerCheckSwUpdate = false;
  Supla::Device::SwUpdate *swUpdate = nullptr;
  const uint8_t *rsaPublicKey = nullptr;

  _impl_arduino_status impl_arduino_status = nullptr;

  Supla::Uptime uptime;
  Supla::Clock *clock = nullptr;
  Supla::Device::LastStateLogger *lastStateLogger = nullptr;

  bool isSrpcInitialized(bool msg);
  // used to indicate if begin() method was called - it will be set to
  // true even if initialization procedure failed for some reason
  bool initializationDone = false;

  void setString(char *dst, const char *src, int max_size);

  void iterateAlwaysElements(unsigned long _millis);
  bool iterateNetworkSetup();
  bool iterateSuplaProtocol(unsigned int _millis);
  void handleLocalActionTriggers();
  void checkIfRestartIsNeeded(unsigned long _millis);

 private:
  void status(int status, const char *msg, bool alwaysLog = false);

};

extern SuplaDeviceClass SuplaDevice;
#endif
