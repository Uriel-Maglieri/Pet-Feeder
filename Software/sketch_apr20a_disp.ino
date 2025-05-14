#include <WiFi.h>
#include <ESP32Servo.h>

//usar dev module

const char* ssid = "moto g52_6844";
const char* password = "uri00000";

// Pines del sensor ultrasónico
const int trigPin = 13;
const int echoPin = 12;

// Pin del relé (para la luz LED)
const int relePin = 14;

// Umbral para activar luz (en cm)
const int umbralNivel = 10;

WiFiServer server(80);

Servo servoMotor;

void setup() {
  Serial.begin(115200);

  // Iniciar servo
  servoMotor.attach(25); //25 funciona
  servoMotor.write(0);   // Posición inicial

  // Configurar sensor y relé
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, LOW); // Apagado al inicio

  // Conectar al WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando al WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado.");
    String request = client.readStringUntil('\r');
    Serial.println("Petición: " + request);
    client.flush();

    // Ver qué botón se tocó en la app
    if (request.indexOf("/dispensar=chico") != -1) {
      dispensarComida(1);
    } else if (request.indexOf("/dispensar=mediano") != -1) {
      dispensarComida(2);
    } else if (request.indexOf("/dispensar=grande") != -1) {
      dispensarComida(3);
    }

    // Leer nivel de comida 
    int nivel = leerNivel();
    Serial.print("Nivel: ");
    Serial.println(nivel);
    if (nivel < umbralNivel) {
      digitalWrite(relePin, HIGH); // Encender luz
    } else {
      digitalWrite(relePin, LOW); // Apagar luz
    }

    // Enviar respuesta al cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/plain");
    client.println();
    client.println("OK");
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}

void dispensarComida(int cantidad) {
  int tiempo = 1000 * cantidad; // Duración depende del tamaño
  servoMotor.write(90); // Abrir compuerta
  delay(tiempo);
  servoMotor.write(0); // Cerrar compuerta
}

int leerNivel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH);
  int distancia = duracion * 0.034 / 2;
  return distancia;
}
