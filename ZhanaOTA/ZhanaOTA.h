
#ifndef ZHANAOTA_H
#define ZHANAOTA_H

/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "Arduino.h"
#include <Esp.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

/** ****************************************************************************
 ** ************ PROTOTYPES ****************************************************
 ** ****************************************************************************/

// firmwareVersionCheck()
// @param none: void
// @return status: int
int firmwareVersionCheck();
bool isNewerVersion(const char* serverVersion, const char* deviceVersion);

// firmwareUpdate()
// @param none: void
// @return bool: bool
bool firmwareUpdate();

#endif /* SMARTBEDDINGOTA_H */

/****** END OF FILE ****/