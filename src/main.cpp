#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>

// Coloca Aqui los credenciales del Wifi
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

// inicializamos el Bot de Telegram
#define BOTtoken "YOUR_BOT_TOKEN" // Coloca el Token de Telegram

// Coloc aqui la ID del Chat Telegram
#define CHAT_ID "YOUR_CHAT_ID"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Verifica si hay un nuevo mensaje cada segundo.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Bombas
int Entrada = 22;
int Salida = 23;

// pines de indicadores
int PinBajo = 13;
int PinMedio = 12;
int PinAlto = 14;

// Variables para saber el estado el tanque
int State1 = 0;
int State2 = 0;
int State3 = 0;
int State4 = 0;

// Pines del ultrasonico

const int Trigger = 15;
const int Echo = 2;

// Variables para saber los datos
float vc;
float vl;
float nc;
float nl;
float c;
float NTc;
float NTl;
int global4 = 0;
int medida = 0;
long t;
long d;

// Maneja lo que Sucede Cuando se recibe un nuevo mensaje
////////////////////////////////////////////////////////////////////////////Void mensaje///////////////////////////////////////////////////////////////////////////////////////////////////////
void handleNewMessages(int numNewMessages)
{
  ////////////////////////////////////////////////////////////////////////////For mensaje///////////////////////////////////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < numNewMessages; i++)
  {
    // Imprime el mensaje recibido
    String text = bot.messages[i].text;
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID)
    {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String from_name = bot.messages[i].from_name;
    if (text == "/start")
    {
      String welcome = "Bienvenido, " + from_name + ".\n";
      welcome += "Utilice los siguientes comandos para controlar el nivel del tanque principal. \n\n";
      welcome += "/Chequeo Para prueba de sensores, indicadores y bombas \n";
      welcome += "/Nivel_Bajo Para que el nivel del tanque sea bajo \n";
      welcome += "/Nivel_Medio Para que el nivel del tanque sea medio \n";
      welcome += "/Nivel_Alto Para que el nivel del tanque sea alto \n";
      welcome += "/Detener Para detener el las bombas de agua \n";
      welcome += "/Estado Para saber el estado actual del tanque principal  \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    ////////////////////////////////////////////////////////////////////////////if Prueba/////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Chequeo")
    {
      bot.sendMessage(chat_id, "Nivel bajo encendido", "");
      digitalWrite(PinBajo, HIGH);
      delay(1000);
      digitalWrite(PinBajo, LOW);
      bot.sendMessage(chat_id, "Nivel medio encendido", "");
      digitalWrite(PinMedio, HIGH);
      delay(1000);
      digitalWrite(PinMedio, LOW);
      bot.sendMessage(chat_id, "Nivel alto encendido", "");
      digitalWrite(PinAlto, HIGH);
      delay(1000);
      digitalWrite(PinAlto, LOW);
      bot.sendMessage(chat_id, "Bomba de entrada encendida", "");
      digitalWrite(Entrada, HIGH);
      delay(1000);
      digitalWrite(Entrada, LOW);
      bot.sendMessage(chat_id, "Bomba de Salida encendida", "");
      digitalWrite(Salida, HIGH);
      delay(1000);
      digitalWrite(Salida, LOW);

      digitalWrite(PinBajo, HIGH);
      delay(1000);
      digitalWrite(PinMedio, HIGH);
      delay(1000);
      digitalWrite(PinAlto, HIGH);
      delay(1000);

      digitalWrite(PinAlto, LOW);
      delay(1000);
      digitalWrite(PinMedio, LOW);
      delay(1000);
      digitalWrite(PinBajo, LOW);
      delay(1000);

      digitalWrite(PinBajo, HIGH);
      digitalWrite(PinMedio, HIGH);
      digitalWrite(PinAlto, HIGH);
      delay(1000);
      digitalWrite(PinBajo, LOW);
      digitalWrite(PinMedio, LOW);
      digitalWrite(PinAlto, LOW);

      bot.sendMessage(chat_id, "Chequedo finalizado con exito", "");
    }
    ////////////////////////////////////////////////////////////////////////////if Bajo/////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Nivel_Bajo")
    {
      medida = 19;
    }
    ////////////////////////////////////////////////////////////////////////////if Medio////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Nivel_Medio")
    {
      medida = 12;
    }
    ////////////////////////////////////////////////////////////////////////////if Alto/////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Nivel_Alto")
    {
      medida = 6;
    }
    ////////////////////////////////////////////////////////////////////////////if Detener/////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Detener")
    {
      global4 = 1;
    }
    ////////////////////////////////////////////////////////////////////////////if estado/////////////////////////////////////////////////////////////////////////////////////////////
    if (text == "/Estado")
    {
      long t2;
      long d2;
      digitalWrite(Trigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(Trigger, LOW);

      t2 = pulseIn(Echo, HIGH);
      d2 = t2 / 59;
      vc = 3146;             // Volumen total en cm
      vl = vc / 1000;        // Volumen total en L
      nc = vc - (121 * d2);  // Volumen vacio cm
      nl = nc / 1000;        // Volumen vacio L
      NTc = vc - nc;         // Volumen que ocupa el liquido cm
      NTl = vl - nl;         // Volumen que ocupa el liquido L
      c = (100 * nl) / (vl); // Porcentaje de capacidad del tanque

      if (d > 15)
      {
        bot.sendMessage(chat_id, "El tanque está en nivel bajo.", "");
        delay(500);

        String bajo1 = "La cantidad que soporta el tanque es: ";
        String bajo2 = String(vl, 3);
        String bajo3 = bajo1 + bajo2;
        String bajo4 = bajo3 + " Litros";
        bot.sendMessage(chat_id, bajo4, "");
        delay(500);

        String bajo5 = "La cantidad de liquido faltante en el interior del tanque es: ";
        String bajo6 = String(NTl, 3);
        String bajo7 = bajo5 + bajo6;
        String bajo8 = bajo7 + " Litros";
        bot.sendMessage(chat_id, bajo8, "");
        delay(500);

        String bajo9 = "El porcentaje que ocupa el liquido en el tanque es: ";
        String bajo10 = String(c, 1);
        String bajo11 = bajo9 + bajo10;
        String bajo12 = bajo11 + "% ";
        bot.sendMessage(chat_id, bajo12, "");
        delay(500);
      }

      if ((d > 9) && (d < 15))
      {
        bot.sendMessage(chat_id, "El tanque está en nivel medio.", "");
        delay(500);

        String medio1 = "La cantidad de liquido faltante en el interior del tanque es: ";
        String medio2 = String(vl, 3);
        String medio3 = medio1 + medio2;
        String medio4 = medio3 + " Litros";
        bot.sendMessage(chat_id, medio4, "");
        delay(500);

        String medio5 = "La cantidad de liquido actual en el interior del tanque es: ";
        Serial.print(NTl);
        String medio6 = String(NTl, 3);
        String medio7 = medio5 + medio6;
        String medio8 = medio7 + " Litros";
        bot.sendMessage(chat_id, medio8, "");
        delay(500);

        String medio9 = "El porcentaje que ocupa el liquido en el tanque es: ";
        String medio10 = String(c, 1);
        String medio11 = medio9 + medio10;
        String medio12 = medio11 + "% ";
        bot.sendMessage(chat_id, medio12, "");
        delay(500);
      }
      if (d < 9)
      {
        bot.sendMessage(chat_id, "El tanque está en nivel alto.", "");
        delay(500);

        String alto1 = "La cantidad de liquido faltante en el interior del tanque es: ";
        String alto2 = String(vl, 3);
        String alto3 = alto1 + alto2;
        String alto4 = alto3 + " Litros";
        bot.sendMessage(chat_id, alto4, "");
        delay(500);

        String alto5 = "La cantidad de liquido actual en el interior del tanque es: ";
        String alto6 = String(NTl, 3);
        String alto7 = alto5 + alto6;
        String alto8 = alto7 + " Litros";
        bot.sendMessage(chat_id, alto8, "");
        delay(500);

        String alto9 = "El porcentaje que ocupa el liquido en el tanque es: ";
        String alto10 = String(c, 1);
        String alto11 = alto9 + alto10;
        String alto12 = alto11 + "% ";
        bot.sendMessage(chat_id, alto12, "");
        delay(500);
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////Void mensaje///////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org"); // obtenemos el tiempo a traves del protocolo NTP
  client.setTrustAnchors(&cert);    // Agregamos el certificado Raiz api.telegram.org
#endif

  pinMode(Entrada, OUTPUT);
  pinMode(Salida, OUTPUT);

  pinMode(PinBajo, OUTPUT);
  pinMode(PinMedio, OUTPUT);
  pinMode(PinAlto, OUTPUT);

  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Agregamos el certificado raiz para api.telegram.org
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
}

void loop()
{
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH);
  d = t / 59;
  Serial.print("Distancia: ");
  Serial.print(d);
  Serial.print("cm");
  Serial.println();
  delay(100);
  vc = 3146;             // Volumen total en cm
  vl = vc / 1000;        // Volumen total en L
  nc = vc - (121 * d);   // Volumen vacio cm
  nl = nc / 1000;        // Volumen vacio L
  NTc = vc - nc;         // Volumen que ocupa el liquido cm
  NTl = vl - nl;         // Volumen que ocupa el liquido L
  c = (100 * nl) / (vl); // Porcentaje de capacidad del tanque

  if (millis() > lastTimeBotRan + botRequestDelay)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  if (global4 != 1)
  {
    if (d < medida)
    {
      digitalWrite(Entrada, LOW);
      digitalWrite(Salida, HIGH);
    }
    if (d > medida)
    {
      digitalWrite(Entrada, HIGH);
      digitalWrite(Salida, LOW);
    }

    if ((d > 18) && (d < 20))
    {
      digitalWrite(PinBajo, HIGH);
      digitalWrite(PinMedio, LOW);
      digitalWrite(PinAlto, LOW);
    }
    if ((d > 11) && (d < 13))
    {
      digitalWrite(PinBajo, LOW);
      digitalWrite(PinMedio, HIGH);
      digitalWrite(PinAlto, LOW);
    }
    if ((d > 5) && (d < 7))
    {
      digitalWrite(PinAlto, HIGH);
      digitalWrite(PinBajo, LOW);
      digitalWrite(PinMedio, LOW);
    }
  }

  if (global4 == 1)
  {
    State4 = 1;
    if (d < 19)
    {
      digitalWrite(PinBajo, HIGH);
    }
    if (d > 19)
    {
      digitalWrite(PinBajo, LOW);
    }
    if (d < 12)
    {
      digitalWrite(PinMedio, HIGH);
    }
    if (d > 12)
    {
      digitalWrite(PinMedio, LOW);
    }
    if (d < 6)
    {
      digitalWrite(PinAlto, HIGH);
    }
    if (d > 6)
    {
      digitalWrite(PinAlto, LOW);
    }
    global4 = 0;
  }
}
