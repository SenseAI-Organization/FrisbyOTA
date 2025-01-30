
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
const char *rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n" \
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n" \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n" \
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n" \
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n" \
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n" \
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n" \
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n" \
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n" \
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n" \
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n" \
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n" \
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n" \
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n" \
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n" \
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n" \
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n" \
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n" \
"+OkuE6N36B9K\n" \
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
    wificlient.setCACert(rootCACertificate);
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