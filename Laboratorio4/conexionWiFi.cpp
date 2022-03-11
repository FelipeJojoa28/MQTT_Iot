#include <WiFi.h>
//Red WiFi
const char *ssid = "";   // Nombre de la red WiFi
const char *password = "";        // WiFi password

void conexionWiFi()  {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {   // Intenta conectar a la red WiFi
    Serial.println("Conectando a WiFi..");  //  cada medio segundo
    delay(500);
  }
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");     // Se imprime la dirección IP del
  Serial.println(WiFi.localIP());     //  dispositivo cuando se conecta
}
