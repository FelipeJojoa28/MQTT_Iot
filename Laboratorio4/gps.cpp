#include <SoftwareSerial.h>//incluimos SoftwareSerial
#include <TinyGPS.h>//incluimos TinyGPS
TinyGPS gps;//Declaramos el objeto gps
SoftwareSerial serialgps(17, 16); //Declaramos el pin 17 para Tx y 16 para Rx en la ESP32

unsigned long chars;
unsigned short sentences, failed_checksum;
char latitudChar[11];      // para guardar la latitud como una cadena de caracteres
char longitudChar[12];     // para guardar la longitud como una cadena de caracteres

void conectarGPS()
{
  serialgps.begin(9600);//Iniciamos el puerto serie del gps
  Serial.println("");
  Serial.println("GPS GY-GPS6MV2");
  Serial.println(" ---Buscando senal--- ");
  Serial.println("");
  while (!serialgps.available()) {
    Serial.print(".");
    delay(1000);
  }
}

//Falta hacer una función que verifique que el GPS conectó o no conectó

void obtenerUbicacionGPS(float* latitude, float* longitude)
{
  //Una vez se valide que se ha conectado el GPS a un satelite , se imprimen los datos de interes
  while (serialgps.available())
  {
    int c = serialgps.read();

    if (gps.encode(c)) //canalizar repetidamente
    {
      //Obtener latitud y longitud
      gps.f_get_position(latitude, longitude);

      //Imprimir los valores en el monitor serial
      Serial.print("Latitud/Longitud: ");
      Serial.print(*latitude, 7);
      Serial.print(", ");
      Serial.println(*longitude, 7);
      Serial.println();
      gps.stats(&chars, &sentences, &failed_checksum); //garantiza la integridad de los datos
    }
  }
}
