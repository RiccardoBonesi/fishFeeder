#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>
#include "secrets.h"

// WiFi
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;     // the WiFi radio's status


// telegram
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;  //last time messages' scan has been done


Servo servo;
#define servoPin D0


void feedFish() {
  Serial.println("Feeding fish...");
  servo.write (90);
  delay (1000);
  servo.write (0);  
  delay(1000);
}

// telegram message handler
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    Serial.print("chat_id: ");
    Serial.println(chat_id);

    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if ( chat_id != "REPLACEME") {
      bot.sendMessage(chat_id, "Hey, you are not allowed to play with my fish!", "");
    } else if ( chat_id == "REPLACEME") {

      if (text == "/feed") {
        bot.sendMessage(chat_id, "Feeding fish...", "");
        feedFish();
        bot.sendMessage(chat_id, "Fish feeded!", "");
      }


      if (text == "/status") {
        bot.sendMessage(chat_id, "System connected. Everything is ok :)", "");
      }

      if (text == "/options") {
        String keyboardJson = "[[\"/feed\"],[\"/status\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
      }

      if (text == "/start") {
        String welcome = "Welcome to the most awesome fish feeder bot, " + from_name + "!\n\n";
        welcome += "/feed : Delivers one dose of feed.\n";
        welcome += "/status : Returns current status\n";
        welcome += "/options : returns the reply keyboard\n";
        bot.sendMessage(chat_id, welcome, "Markdown");
        String keyboardJson = "[[\"/feed\"],[\"/status\"],[\"/options\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, "", "", keyboardJson, true);
      }
    }
  }
}




void setup() {  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  servo.attach(servoPin);


  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("\nConnected to WiFi.");

  client.setInsecure(); 

  servo.write(0);
  

}







void loop() {

  if (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    while ( status != WL_CONNECTED) {
      status = WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected to WiFi.");
  }


  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }


}
