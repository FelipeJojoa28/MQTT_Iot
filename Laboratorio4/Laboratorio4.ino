/* Autores:
 *   Andrés Felipe Jojoa Gómez
 *   Deiby Zair Melo Olivar
 *   Laura Muñoz Muñoz
 *   Juan Camilo Otero Velasco
 */

#include "conexionWiFi.h"
#include "conexionMQTT.h"
#include "persistencia.h"
#include "gps.h"
#include "FechaHora.h"

#define pin_sensor 35   // Pin analogico de entrada
#define N 10                   // Período de envío de datos en s

float latitude = 2.4455532, longitude = -76.598177; // para obtener la latitug y longitud del GPS

unsigned long currentMillis = 0;    // Tiempo de ejecucion actual del programa
unsigned long previousMillis = 0;   // Tiempo de ejecucion en el que se enviaron los ultimos datos

int lectura;        // Valor obtenido del pin
char datos[60];      // Datos para publicar en la red
int muestras[N];    // Vector de datos leidos en el período
int contador = 0;   // Numero de muestras tomadas hasta el mmomento

void setup() {
  Serial.begin(115200); // Puerto serial de alta frecuencia

  conexionWiFi();       // Conexion a la red WiFi
  conexionNTP();        // Conexion a servidor de fceha y hora
  conexionRTC();        // Coexión y configuración del RTC
  conexionMQTTBroker(); // Conexion a servicio MQTT
  conexionSD();
  conectarGPS();
  Serial.println("");
}

void loop()
{
  obtenerUbicacionGPS(&latitude,&longitude);
  currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) // Se evalua si a pasado 1 s.
  {
    if (contador < N) // Evalua si se llego al numero de muestras
    {
      lectura = analogRead(pin_sensor);  // Lee el valor analógico de entrada
      muestras[contador] = lectura;             // Agrega el dato leido a las  muestras
      contador += 1;                            // Actualiza el numero de muestras tomadas
      previousMillis = currentMillis;           // Actualiza el tiempo de toma de la ultima muestra
    }
    if (contador >= N)
    { 
      String dataStr = construirCSV(obtenerFechaHora(), promedio(), latitude, longitude);
      backupAndPublish(dataStr);
      contador = 0;           // Reinicio para pasar al sig. minuto
    }
  }
}

float promedio() // Funcion para obtener el valor promedio del vector de muestras
{
  int suma = 0;
  for (int i = 0; i < N; i++) { //Sumatoria de las muestras 
    suma += muestras[i];
  }
  float promedio = (float)suma / N;
  float voltaje = map(promedio, 0, 4095, 0, 3300); // Conversión a
  voltaje /= 1000.0;                               //   voltaje
  
  float temp = (voltaje - 0.41) * 100; //Conversión a variable deseada
  return temp;
}
