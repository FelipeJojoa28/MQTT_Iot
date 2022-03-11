#include <WiFi.h>
#include <FS.h> //para manejar archivos
#include <SD.h> //para la funcionalidad de la tarjeta microSD
#include <SPI.h> //ya que estamos usando el protocolo de comunicación SPI entre la placa y el modulo SD
#include "conexionMQTT.h"
#define PIN_CS 5

bool inicializarSD(){
  if (!SD.begin(PIN_CS)) { //No reconoce el pin CS
    Serial.println("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }
  return true;
}

void conexionSD() {
  Serial.println("Inicializando SD");
  while(!inicializarSD()){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND); //abrir el archivo
  if (!file) //si el archivo no existe
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) //colocar el mensaje en el archivo
  {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close(); //cerramos el archivo
}



void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

String floattostr(float variable, byte tamanio, byte nro_decimales) {
  String valor = "NaN";
  char valorch[tamanio] = "";
  dtostrf(variable, tamanio, nro_decimales, valorch);
  valor = String(valorch);
  valor.replace(" ", "");
  return valor;
}

void leerArchivoYEnviar(fs::FS &fs, const char * path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Read from file: ");
  int contador = 0;
  char datosLeidos[60];
  bool publica = true;
  while (file.available())
  {
    char letra;
    letra = file.read();
    if (letra != '\n')
    {
      datosLeidos[contador] = letra;
      contador++;
    }
    else
    {
      if (publica) {
        datosLeidos[contador] = '\0'; //marca el fin de la cadena
        String datosEnvio = String(datosLeidos);
        datosEnvio.replace(";", "/");
        char datos[60] = "";
        datosEnvio.replace("\n","");
        datosEnvio.toCharArray(datos, 60);
        publica = publicar(datos); // Publica el valor promedio si se alcanzo el numero de muestras
      }
      if (!publica) {
        datosLeidos[contador] = '\n';
        datosLeidos[contador + 1] = '\0';
        appendFile(fs, "/unsended1.csv", datosLeidos);
      }
      contador = 0;
    }
  }
  file.close();
  deleteFile(fs, path);
  if (!publica) {
    renameFile(fs, "/unsended1.csv", path);
  }

}

String construirCSV(String fechaHora, float voltaje, float latitud, float longitud) {
  
  String datosS = fechaHora;
  datosS += ";";             //  Se concatena la informacion de fecha y hora
  datosS += floattostr(voltaje, 5, 2);
  datosS += ";";
  datosS += floattostr(latitud, 11, 7);
  datosS += ";";
  datosS += floattostr(longitud, 12, 7);
  datosS += "\n";

  datosS.replace(" ", "");
  return datosS;
}


void backupAndPublish(String csv) {
  char datos1[60];
  csv.toCharArray(datos1, 60);
  appendFile(SD, "/backup.csv", datos1); // Guarda los datos en la memoria
  appendFile(SD, "/unsended.csv", datos1); // Guarda los datos en la memoria
  if (WiFi.status() == WL_CONNECTED) { //probablemente causa algun confilcto Hay que revisarlo
    leerArchivoYEnviar(SD, "/unsended.csv");
  }
}
