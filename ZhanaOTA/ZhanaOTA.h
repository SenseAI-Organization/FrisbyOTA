
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

// firmwareUpdate()
// @param none: void
// @return none: void
void firmwareUpdate();

#endif /* SMARTBEDDINGOTA_H */

/****** END OF FILE ****/