/*
 Name:		Relogio_ESP32.ino
 Created:	3/7/2021 7:30:24 PM
 Author:	Thiago Turcato do Rego
*/

#include <dummy.h>
#include <tinyxml2.h> //Biblioteca para decodificação de XML
#include <WiFi.h>

const char* host = "g1.globo.com";
String url = "/rss/g1/";

//Declaração de variáveis
char streamReadChar = '\0';
String completeText = "";
String texthtmlMarker = "";
String channelTitle = "";
String strteste = "";
String headLines[8];
long i = 0;
bool htmlMarkerBegin = false;
bool htmlMarkerCheck = false;
byte htmlMarkerNest = 0;
String htmlMarkerCaptured[8];


void setup() {

	const char* ssid = "NET_2G_IR";     // change this for your own network
	const char* password = "20101014";  // change this for your own network

	//Inicia a comunicação serial para impressão de resultado
	Serial.begin(115200);

	//Inicia conexão WiFi
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

}

void loop() {


	// put your main code here, to run repeatedly:
	delay(5000);

	Serial.print("connecting to ");
	Serial.println(host);

	// Use WiFiClient class to create TCP connections
	WiFiClient client;
	const int httpPort = 80;
	if (!client.connect(host, httpPort)) {
		Serial.println("connection failed");
		return;
	}

	// We now create a URI for the request
	Serial.print("Requesting URL: ");
	Serial.println(url);

	// This will send the request to the server
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 5000) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			return;
		}
	}

	// Read all the lines of the reply from server and print them to Serial

	// channel -> title (fonte)
	// item -> title

	while (client.available() && i < 1500) {
		streamReadChar = client.read();
		completeText = completeText + streamReadChar;
		if (streamReadChar == '>') {
			htmlMarkerBegin = false;
			htmlMarkerCheck = true;
		}
		if (htmlMarkerBegin) texthtmlMarker = texthtmlMarker + streamReadChar;
		if (streamReadChar == '<') {
			htmlMarkerBegin = true;
			texthtmlMarker = "";
		}

		if (htmlMarkerCheck) {

			if (texthtmlMarker == "channel") {
				htmlMarkerCaptured[1] = texthtmlMarker;
				htmlMarkerNest = 1;
			}
			else if (texthtmlMarker == "item") {
				htmlMarkerCaptured[1] = texthtmlMarker;
				htmlMarkerNest = 1;
				strteste = texthtmlMarker;
			}
			else if (texthtmlMarker == "/channel") {
				htmlMarkerCaptured[1] = "";
				htmlMarkerNest = 0;
			}
			else if (texthtmlMarker == "/item") {
				htmlMarkerCaptured[1] = "";
				htmlMarkerNest = 0;
			}

			else if (htmlMarkerCaptured[1] == "channel") {
				if (texthtmlMarker == "title") {
					htmlMarkerCaptured[2] = "title";
					htmlMarkerNest = 2;
				}
				else if (texthtmlMarker == "/title") {
					htmlMarkerCaptured[2] = "";
					htmlMarkerNest = 1;
				}
			}
			else if (htmlMarkerCaptured[1] == "item") {
				if (texthtmlMarker == "title") {
					htmlMarkerCaptured[2] = "title";
					htmlMarkerNest = 2;
				}
				else if (texthtmlMarker == "/title") {
					htmlMarkerCaptured[2] = "";
					htmlMarkerNest = 1;
				}
			}
			htmlMarkerCheck = false;
		}
		else if (!htmlMarkerBegin && htmlMarkerCaptured[1] == "channel" && htmlMarkerCaptured[2] == "title") {
			channelTitle += streamReadChar;
		}
		else if (!htmlMarkerBegin && htmlMarkerCaptured[1] == "item" && htmlMarkerCaptured[2] == "title") {
			headLines[0] += streamReadChar;
		}
		i++;
	}

	Serial.println();
	Serial.println();
	Serial.print("Complete text (");
	Serial.print(i);
	Serial.println(" chars):");
	Serial.print(completeText);
	Serial.println();
	Serial.println();
	Serial.print("Teste: ");
	Serial.print(strteste);
	Serial.println();
	Serial.println();
	Serial.print("Channel Title: ");
	Serial.print(channelTitle);
	Serial.println();
	Serial.println();
	Serial.print("Headline 1: ");
	Serial.print(headLines[0]);
	Serial.println();
	Serial.println();
	Serial.println();
	Serial.println("closing connection");

	delay(5000);
}
