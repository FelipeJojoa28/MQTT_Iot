#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>

WiFiClient espClient;           // Cliente WiFi para
PubSubClient client(espClient); //  protocolo MQTT

// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org"; // Direccion del servidor del broker
const char *mqtt_name = "esp32Client";          // Nombre del dispositivo
const char *topic = "sensores/potenciometro";   // Tópico en el que se publican los datos
const int mqtt_port = 1883;                     // Puerto por defecto para MQTT

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Mensaje recibido en el tópico: ");
  Serial.println(topic);
  Serial.print("Mensaje:");           // Se imprimen los mensajes publicados
  for (int i = 0; i < length; i++) {  //  en el topico seleccionado
    Serial.print((char) payload[i]);
  }
  Serial.println();
}

void conexionMQTTBroker() {
  client.setServer(mqtt_broker, mqtt_port); // Fija el servidor del broker
  client.setCallback(callback);             //  Establecela función de retorno de MQTT
}

bool reconnect() {
  delay(1000);
  if (!client.connected()) {                       // Realiza la conexión con el broker MQTT
    Serial.println("Intentando conectarse MQTT...");    //  cuando se pierde o no se ha iniciado la conexion
    if (client.connect(mqtt_name)) {
      Serial.println("Conectado");
      client.subscribe(topic);
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      return false;
    }
  }
  return true;
}

bool publicar(char *datos) {
  if (!client.connected()) // Verifica la conexión establecida con  el broker
  {
    if (!reconnect()) { // Reconecta si se ha perdido la conexion
      return false;
    }
  }
  client.loop();
  if (client.publish(topic, datos)) //Publica los datos obtenidos en el topico
  {
    Serial.print("Publicó: ");
    Serial.print(datos);
    Serial.print(" en: ");
    Serial.println(topic);
    return true;
  }
  return false;
}
