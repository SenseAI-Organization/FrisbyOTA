
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
const char *rootCACertificate = 
"-----BEGIN CERTIFICATE-----\n"
"MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
"MjAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS\n"
"U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
"AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a\n"
"qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn\n"
"g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW\n"
"raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB\n"
"Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r\n"
"eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB\n"
"/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU\n"
"TiJUIBiV5uNu5g/6+rkS7QYXjzkwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG\n"
"CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG\n"
"GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh\n"
"Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNydDB6BgNV\n"
"HR8EczBxMDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH\n"
"bG9iYWxSb290RzIuY3JsMDegNaAzhjFodHRwOi8vY3JsNC5kaWdpY2VydC5jb20v\n"
"RGlnaUNlcnRHbG9iYWxSb290RzIuY3JsMDAGA1UdIAQpMCcwBwYFZ4EMAQEwCAYG\n"
"Z4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEBAHer\n"
"t3onPa679n/gWlbJhKrKW3EX3SJH/E6f7tDBpATho+vFScH90cnfjK+URSxGKqNj\n"
"OSD5nkoklEHIqdninFQFBstcHL4AGw+oWv8Zu2XHFq8hVt1hBcnpj5h232sb0HIM\n"
"ULkwKXq/YFkQZhM6LawVEWwtIwwCPgU7/uWhnOKK24fXSuhe50gG66sSmvKvhMNb\n"
"g0qZgYOrAKHKCjxMoiWJKiKnpPMzTFuMLhoClw+dj20tlQj7T9rxkTgl4ZxuYRiH\n"
"as6xuwAwapu3r9rxxZf+ingkquqTgLozZXq8oXfpf2kUCwA/d5KxTVtzhwoT0JzI\n"
"8ks5T1KESaZMkE4f97Q=\n"
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
    Serial.println("Checking URL: " + fwurl);  // Añadir para debug
    
    WiFiClientSecure *wificlient = new WiFiClientSecure;

    if (wificlient)
    {
        wificlient->setCACert(rootCACertificate);
        HTTPClient https;

        if (https.begin(*wificlient, fwurl))
        { 
            Serial.println("[HTTPS] GET...");
            delay(100);
            httpCode = https.GET();
            delay(100);
            if (httpCode == HTTP_CODE_OK)
            {
                payload = https.getString();
                Serial.println("Received version: " + payload);  // Añadir para debug
                Serial.println("Current version: " + String(Version_firmware));  // Añadir para debug
            }
            else
            {
                Serial.print("Error in downloading version file: ");
                Serial.println(httpCode);
                https.end();
                delete wificlient;
                return -1;  // Retornar error en lugar de 0
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
    return -1;  // Retornar error en lugar de 0
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