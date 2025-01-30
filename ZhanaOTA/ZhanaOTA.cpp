
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
// Función auxiliar para comparar versiones
bool isNewerVersion(const char* serverVersion, const char* deviceVersion) {
    int serverNums[3] = {0};
    int deviceNums[3] = {0};
    
    // Parsear versión del servidor
    sscanf(serverVersion, "%d.%d.%d", &serverNums[0], &serverNums[1], &serverNums[2]);
    
    // Parsear versión del dispositivo
    sscanf(deviceVersion, "%d.%d.%d", &deviceNums[0], &deviceNums[1], &deviceNums[2]);
    
    // Comparar versiones
    for(int i = 0; i < 3; i++) {
        if(serverNums[i] > deviceNums[i]) return true;
        if(serverNums[i] < deviceNums[i]) return false;
    }
    
    return false; // Si son iguales, no es una versión más nueva
}

// Modificar la función firmwareVersionCheck para usar la nueva lógica
int firmwareVersionCheck() {
    if (WiFi.status() != WL_CONNECTED) return -1;
    
    HTTPClient http;
    http.begin("https://raw.githubusercontent.com/SenseAI-Organization/FrisbyOTA/main/bin_version.txt");
    
    int httpCode = http.GET();
    if(httpCode != HTTP_CODE_OK) {
        http.end();
        return -1;
    }
    
    String newFWVersion = http.getString();
    http.end();
    
    // Limpiar cualquier caracter no deseado
    newFWVersion.trim();
    
    Serial.print("Server version: ");
    Serial.println(newFWVersion);
    Serial.print("Device version: ");
    Serial.println(Version_firmware);
    
    // Usar la nueva función de comparación
    if(isNewerVersion(newFWVersion.c_str(), Version_firmware)) {
        return 1;
    }
    
    return 0;
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