
/** ****************************************************************************
 ** ************ INCLUDES ******************************************************
 ** ****************************************************************************/
#include "ZhanaOTA.h"

extern const char *Version_firmware;

/** ****************************************************************************
 ** ************ DEFINES *******************************************************
 ** ****************************************************************************/
// URLs to query new firmware version
#define URL_FW_VER "https://raw.githubusercontent.com/SenseAI-Organization/FrisbyOTA/main/bin_version.txt"
#define URL_FW_BIN "https://raw.githubusercontent.com/SenseAI-Organization/FrisbyOTA/main/firmware.bin"


/** ****************************************************************************
 ** ************ VARIABLES *****************************************************
 ** ****************************************************************************/
const char *rootCACertificate = "-----BEGIN CERTIFICATE-----\n"
"MIIEozCCBEmgAwIBAgIQTij3hrZsGjuULNLEDrdCpTAKBggqhkjOPQQDAjCBjzEL\n"
"MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n"
"BxMHU2FsZm9yZDEYMBYGA1UEChMPU2VjdGlnbyBMaW1pdGVkMTcwNQYDVQQDEy5T\n"
"ZWN0aWdvIEVDQyBEb21haW4gVmFsaWRhdGlvbiBTZWN1cmUgU2VydmVyIENBMB4X\n"
"DTI0MDMwNzAwMDAwMFoXDTI1MDMwNzIzNTk1OVowFTETMBEGA1UEAxMKZ2l0aHVi\n"
"LmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABARO/Ho9XdkY1qh9mAgjOUkW\n"
"mXTb05jgRulKciMVBuKB3ZHexvCdyoiCRHEMBfFXoZhWkQVMogNLo/lW215X3pGj\n"
"ggL+MIIC+jAfBgNVHSMEGDAWgBT2hQo7EYbhBH0Oqgss0u7MZHt7rjAdBgNVHQ4E\n"
"FgQUO2g/NDr1RzTK76ZOPZq9Xm56zJ8wDgYDVR0PAQH/BAQDAgeAMAwGA1UdEwEB\n"
"/wQCMAAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMEkGA1UdIARCMEAw\n"
"NAYLKwYBBAGyMQECAgcwJTAjBggrBgEFBQcCARYXaHR0cHM6Ly9zZWN0aWdvLmNv\n"
"bS9DUFMwCAYGZ4EMAQIBMIGEBggrBgEFBQcBAQR4MHYwTwYIKwYBBQUHMAKGQ2h0\n"
"dHA6Ly9jcnQuc2VjdGlnby5jb20vU2VjdGlnb0VDQ0RvbWFpblZhbGlkYXRpb25T\n"
"ZWN1cmVTZXJ2ZXJDQS5jcnQwIwYIKwYBBQUHMAGGF2h0dHA6Ly9vY3NwLnNlY3Rp\n"
"Z28uY29tMIIBgAYKKwYBBAHWeQIEAgSCAXAEggFsAWoAdwDPEVbu1S58r/OHW9lp\n"
"LpvpGnFnSrAX7KwB0lt3zsw7CAAAAY4WOvAZAAAEAwBIMEYCIQD7oNz/2oO8VGaW\n"
"WrqrsBQBzQH0hRhMLm11oeMpg1fNawIhAKWc0q7Z+mxDVYV/6ov7f/i0H/aAcHSC\n"
"Ii/QJcECraOpAHYAouMK5EXvva2bfjjtR2d3U9eCW4SU1yteGyzEuVCkR+cAAAGO\n"
"Fjrv+AAABAMARzBFAiEAyupEIVAMk0c8BVVpF0QbisfoEwy5xJQKQOe8EvMU4W8C\n"
"IGAIIuzjxBFlHpkqcsa7UZy24y/B6xZnktUw/Ne5q5hCAHcATnWjJ1yaEMM4W2zU\n"
"3z9S6x3w4I4bjWnAsfpksWKaOd8AAAGOFjrv9wAABAMASDBGAiEA+8OvQzpgRf31\n"
"uLBsCE8ktCUfvsiRT7zWSqeXliA09TUCIQDcB7Xn97aEDMBKXIbdm5KZ9GjvRyoF\n"
"9skD5/4GneoMWzAlBgNVHREEHjAcggpnaXRodWIuY29tgg53d3cuZ2l0aHViLmNv\n"
"bTAKBggqhkjOPQQDAgNIADBFAiEAru2McPr0eNwcWNuDEY0a/rGzXRfRrm+6XfZe\n"
"SzhYZewCIBq4TUEBCgapv7xvAtRKdVdi/b4m36Uyej1ggyJsiesA\n"
"-----END CERTIFICATE-----\n";

/* ***************************************************************************
 * **** FUNCTIONS ************************************************************
 * ***************************************************************************/
// firmwareVersionCheck()
// @param none: void
// @return status: int
int firmwareVersionCheck()
{
    String payload;
    int httpCode;
    String fwurl = "";
    fwurl += URL_FW_VER;
    fwurl += "?";
    fwurl += String(rand());
    Serial.println("Checking URL: " + fwurl);
    
    WiFiClientSecure *wificlient = new WiFiClientSecure;

    if (wificlient)
    {
        wificlient->setInsecure();  // Prueba con esto en lugar de setCACert
        HTTPClient https;

        if (https.begin(*wificlient, fwurl))
        { 
            Serial.println("[HTTPS] GET...");
            delay(100);
            httpCode = https.GET();
            delay(100);
            
            Serial.print("HTTP Response code: ");
            Serial.println(httpCode);
            
            if (httpCode == HTTP_CODE_OK)
            {
                payload = https.getString();
                Serial.println("Received version: " + payload);
                Serial.println("Current version: " + String(Version_firmware));
            }
            else
            {
                Serial.print("Error in downloading version file: ");
                Serial.println(httpCode);
                https.end();
                delete wificlient;
                return -1;
            }
            https.end();
        }
        delete wificlient;
    }

    if (httpCode == HTTP_CODE_OK)
    {
        payload.trim();
        if (payload.equals(Version_firmware))
        {
            Serial.printf("\nDevice already on latest firmware version:%s\n", Version_firmware);
            return 0;
        }
        else
        {
            Serial.println("Server version: " + payload);
            Serial.println("Device version: " + String(Version_firmware));
            Serial.println("New firmware detected");
            return 1;
        }
    }
    return -1;
}

// firmwareUpdate()
// @param none: void
// @return none: void
void firmwareUpdate(void)
{
    WiFiClientSecure wificlient;
    wificlient.setInsecure();
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);
    // Serial.println("Procedding to update firmware. Device will reboot if successful.");
    t_httpUpdate_return ret = httpUpdate.update(wificlient, URL_FW_BIN);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        // Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        // Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        // Serial.println("HTTP_UPDATE_OK");
        break;
    }
}

/****** END OF FILE ****/

/*
if (firmwareVersionCheck())
{
    firmwareUpdate();
}
else
{
    // Serial.println("No new firmware detected");
}
*/