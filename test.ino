#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Définir les broches pour le RFID-RC522
#define RST_PIN 4    // Broche de Reset du module RC522
#define SS_PIN 5     // Broche SDA du module RC522

// Définir les broches pour le buzzer et les LEDs
#define BUZZER_PIN 13 // Broche du buzzer
#define GREEN_LED 15  // Broche de la LED verte
#define RED_LED 2    // Broche de la LED rouge

// Initialisation de l'écran LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adresse I2C 0x27, 16 colonnes et 2 lignes

// Initialisation du module RFID
MFRC522 rfid(SS_PIN, RST_PIN);

// Liste des UIDs autorisés
const String validUIDs[] = {"53 91 41 A6", "AB CD EF 90"}; // Remplacez par vos UIDs

// Réseau Wi-Fi
const char* ssid = "PC_Othmane";          // Remplacez par votre SSID Wi-Fi
const char* password = "othmanealaoui"; // Remplacez par votre mot de passe Wi-Fi

// Supabase
const char* supabaseUrl = "https://epmgkerkxgtcibpqzbht.supabase.co"; // Remplacez par l'URL de votre projet
const char* supabaseApiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImVwbWdrZXJreGd0Y2licHF6Ymh0Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3MjU2OTkzOTgsImV4cCI6MjA0MTI3NTM5OH0.NGcQ8EGkJJ0uMFpokjvcmYv5a5JIwX4Wc6qRVECPaYM";           // Remplacez par votre clé API

// Fonction pour vérifier si l'UID est autorisé
bool isAuthorized(String uid) {
  for (String validUID : validUIDs) {
    if (uid == validUID) {
      return true;
    }
  }
  return false;
}

// Fonction pour envoyer des données à Supabase
void sendDataToSupabase(String uid, float amount) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(String(supabaseUrl) + "/rest/v1/views");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabaseApiKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseApiKey));

    // Construire le JSON avec ArduinoJson
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["uid"] = uid;
    jsonDoc["amount"] = amount;

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    Serial.println(httpResponseCode);
    if (httpResponseCode > 0) {
      Serial.println("Data sent to Supabase successfully!");
      Serial.println("Response: " + http.getString());
    } else {
      Serial.println("Error sending data to Supabase: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

void setup() {
  // Initialisation des périphériques
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  SPI.begin();          // Initialiser SPI
  rfid.PCD_Init();      // Initialiser le module RFID

  pinMode(BUZZER_PIN, OUTPUT); // Configurer le buzzer comme sortie
  pinMode(GREEN_LED, OUTPUT);  // Configurer la LED verte comme sortie
  pinMode(RED_LED, OUTPUT);    // Configurer la LED rouge comme sortie

  digitalWrite(GREEN_LED, LOW); // Assurez-vous que les LEDs sont éteintes au départ
  digitalWrite(RED_LED, LOW);

  // Connexion au Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  Serial.println("WiFi connected!");
  delay(2000);
  lcd.clear();
  lcd.print("Scan your card");
}

void loop() {
  // Vérifier si une carte est présente
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Faire sonner le buzzer pour indiquer qu'une carte est détectée
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);

  // Lire l'UID de la carte
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase(); // Mettre en majuscules pour correspondance

  lcd.clear();
  lcd.print("Card UID:");
  lcd.setCursor(0, 1);
  lcd.print(uid);

  delay(2000); // Pause pour afficher l'UID

  // Vérifier si la carte est autorisée
  if (isAuthorized(uid)) {
    // LED verte s'allume
    digitalWrite(GREEN_LED, HIGH);

    lcd.clear();
    lcd.print("Card Accepted");
    lcd.setCursor(0, 1);
    lcd.print("Processing...");
    delay(3000);

    // Envoyer les données à Supabase
    float transactionAmount = 50.0; // Montant fictif
    sendDataToSupabase(uid, transactionAmount);

    lcd.clear();
    lcd.print("Payment Done!");

    digitalWrite(GREEN_LED, LOW); // Éteindre la LED verte après le traitement
  } else {
    // LED rouge s'allume
    digitalWrite(RED_LED, HIGH);

    lcd.clear();
    lcd.print("Card Rejected");
    lcd.setCursor(0, 1);
    lcd.print("Try again");
    delay(3000);

    digitalWrite(RED_LED, LOW); // Éteindre la LED rouge après le message
  }

  delay(3000); // Attendre avant de scanner une nouvelle carte
  lcd.clear();
  lcd.print("Scan your card");

  // Arrêter la communication avec la carte
  rfid.PICC_HaltA();
}