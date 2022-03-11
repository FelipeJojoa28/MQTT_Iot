#include <SD.h> //para la funcionalidad de la tarjeta microSD
bool conexionSD();
String floattostr(float variable, byte tamanio, byte nro_decimales);
void appendFile(fs::FS &fs, const char * path, const char * message);
String construirCSV(String fechaHora, float voltaje, float latitud, float longitud);
void leerArchivoYEnviar(fs::FS &fs, const char * path);
void backupAndPublish(String csv);
