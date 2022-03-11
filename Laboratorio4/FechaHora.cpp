#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>    // incluye libreria para interfaz I2C
#include <RTClib.h>   // incluye libreria para el manejo del modulo RTC

WiFiUDP ntpUDP;               // Cliente NTP
NTPClient timeClient(ntpUDP); //  para obtener tiempo actual
RTC_DS1307 rtc;     // crea objeto del tipo RTC_DS1307231

const int GMT = -5;           // Zona horaria en la que se encuentra
uint8_t fechaHora[7] = {};

void conexionNTP() {
  timeClient.begin();                   // Inicializa el clinte NTP para obtener el tiempo
  timeClient.setTimeOffset(GMT * 3600); // Configura la zona horaria
}

void configurarFechaHora() {
  while (!timeClient.update()) {    // Actualiza la fecha y  hora
    timeClient.forceUpdate();       //  a la actual
  }

  String fechaHoraS = timeClient.getFormattedDate();  // Se almacena e imprime
  int splitT = fechaHoraS.indexOf("T");

  Serial.print("Tiempo de sensado: ");
  Serial.println(fechaHoraS);

  String dayStamp = fechaHoraS.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);

  String timeStamp = fechaHoraS.substring(splitT + 1, fechaHoraS.length() - 1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);

  for (int i = 0 ; i < 3 ; i++) {
    fechaHora[i] = timeStamp.substring(6 - 3 * i, 8 - 3 * i).toInt();
  }

  for (int i = 4 ; i < 7 ; i++) {
    fechaHora[i] = dayStamp.substring(8 - 3 * (i - 4), 10 - 3 * (i - 4)).toInt();
  }
}

uint8_t bin2bcd(uint8_t bin) {  //Convierte un número binario a BCD
  return (bin / 10 * 16) + (bin % 10);
}

bool write_ds1307() {
  Wire.beginTransmission(0x68); // Iniciar el intercambio de información con el DS1307 (0x68)
  Wire.write(0x00); // Escribir la dirección del registro segundero

  // Escribir valores en los registros, nos aseguramos que el bit clock halt
  // en el registro del segundero este desactivado (esto hace que el reloj funcione)
  Wire.write(bin2bcd(fechaHora[0] & 0x7F)); // <--- Esto hace que el reloj comience a trabajar
  Wire.write(bin2bcd(fechaHora[1]));
  Wire.write(bin2bcd(fechaHora[2]));
  Wire.write(bin2bcd(fechaHora[3]));
  Wire.write(bin2bcd(fechaHora[4]));
  Wire.write(bin2bcd(fechaHora[5]));
  Wire.write(bin2bcd(fechaHora[6]));
//2018-04-30T16:00:13Z
  // Terminamos la escritura y verificamos si el DS1307 respondio
  // Si la escritura se llevo a cabo el metodo endTransmission retorna 0
  if (Wire.endTransmission() != 0)
    return false;

  return true;  // Retornar verdadero si se escribio con exito
}

void print_time() {
  Serial.print("Fecha: ");
  Serial.print(fechaHora[4]);
  Serial.print('/');
  Serial.print(fechaHora[5]);
  Serial.print('/');
  Serial.print(fechaHora[6]);

  Serial.print("  Hora: ");
  Serial.print(fechaHora[2]);
  Serial.print(':');
  Serial.print(fechaHora[1]);
  Serial.print(':');
  Serial.print(fechaHora[0]);

  Serial.println();
}

void conexionRTC(){
  // Preparar la librería Wire (I2C)
  Wire.begin();
  configurarFechaHora();
  // Escribir hora y fecha al RTC

  Serial.println("Poner a tiempo el DS1307 en Tiny RTC:");
  if (write_ds1307()) {
    print_time();
  }
}

String obtenerFechaHora() {
  if (! rtc.begin()) {       // si falla la inicializacion del modulo
    Serial.println("Modulo RTC no encontrado !");  // muestra mensaje de error
    Serial.print("No es posible conectarse con RTC externo ");
    return "";    
  }
  String SfechaHora = "";
  DateTime fecha = rtc.now();      // funcion que devuelve fecha y horario en formato
  // DateTime y asigna a variable fecha
  SfechaHora += fecha.year();
  SfechaHora += "-";
  SfechaHora += fecha.month();
  SfechaHora += "-";
  SfechaHora += fecha.day();
  SfechaHora += "T";
  SfechaHora += fecha.hour();
  SfechaHora += ":";
  SfechaHora += fecha.minute();
  SfechaHora += ":";
  SfechaHora += fecha.second();
  SfechaHora += "Z";
  Serial.print(fecha.year());     // funcion que obtiene el dia de la fecha completa
  Serial.print("-");       // caracter barra como separador
  Serial.print(fecha.month());     // funcion que obtiene el mes de la fecha completa
  Serial.print("-");       // caracter barra como separador
  Serial.print(fecha.day());      // funcion que obtiene el año de la fecha completa
  Serial.print("T");       // caracter espacio en blanco como separador
  Serial.print(fecha.hour());      // funcion que obtiene la hora de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.print(fecha.minute());      // funcion que obtiene los minutos de la fecha completa
  Serial.print(":");       // caracter dos puntos como separador
  Serial.println(fecha.second());    // funcion que obtiene los segundos de la fecha completa
  return SfechaHora;
}
