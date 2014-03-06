#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include "SD.h"

static uint8_t mac[6] = { 0x90, 0xA2, 0xDA, 0x0D, 0x7A, 0x20 };
static uint8_t ip[4] = { 192, 168, 1, 3 };

WebServer webserver("", 80);

#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6
#define CS_PIN 10
#define SD_PIN 4

int red = 0;
int blue = 0;
int green = 0;
int intensity = 0;
float time = 0;
boolean rainbow = false;

void setDefaultColor()
{
  red = 255;
  green = 155;
  blue = 0;
  intensity = 255;
  rainbow = false;
}

void HSLtoRGB(float h, float s, float l, int & outRed, int & outGreen, int & outBlue)
{
	float r, g, b;
	float q = l < 0.5 ? l * (1+s) : l + s - (l*s);
	float p = 2*l - q;
	float tR = h + (1.0 / 3.0);
	float tG = h;
	float tB = h - (1.0 / 3.0);
	
	if (tR < 0) tR += 1.0;
	if (tG < 0) tG += 1.0;
	if (tB < 0) tB += 1.0;
	if (tR > 1) tR -= 1.0;
	if (tG > 1) tG -= 1.0;
	if (tB > 1) tB -= 1.0;
	
	if (tR < (1.0 / 6.0))
		r = p + ((q - p) * 6.0 * tR);
	else if (tR < 0.5)
		r = q;
	else if  (tR < (2.0 / 3.0))
		r = p + ((q - p) * 6.0 * (2.0 / 3.0 - tR));
	else
		r = p;
	
	if (tG < (1.0 / 6.0))
		g = p + ((q - p) * 6.0 * tG);
	else if (tG < 0.5)
		g = q;
	else if  (tG < (2.0 / 3.0))
		g = p + ((q - p) * 6.0 * (2.0 / 3.0 - tG));
	else
		g = p;
	
	if (tB < (1.0 / 6.0))
		b = p + ((q - p) * 6.0 * tB);
	else if (tB < 0.5)
		b = q;
	else if  (tB < (2.0 / 3.0))
		b = p + ((q - p) * 6.0 * (2.0 / 3.0 - tB));
	else
		b = p;
		
	outRed = min(255, max(0, r * 255));
	outBlue = min(255, max(0, g * 255));
	outGreen = min(255, max(0, b * 255));
}


void rainbowAtTime(float inTime, float inDuration)
{
	float H = inTime/inDuration;
	float S = 1;
	float L = 0.5;
	
	int redVal = 0;
	int greenVal = 0;
	int blueVal = 0;
	
	HSLtoRGB(H,S,L,redVal,greenVal,blueVal);
	
	red = redVal;
	green = greenVal;
	blue = blueVal;
}

void webHandler(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);

      if (strcmp(name, "red") == 0)
      {
        red = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "green") == 0)
      {
        green = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "blue") == 0)
      {
        blue = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "intensity") == 0)
      {
        intensity = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "rainbow") == 0)
      {
        int test = strtoul(value, NULL, 10);
		if (test == 1)
		{
			rainbow = true;
		}
		else
		{
			rainbow = false;
		}
      }
    } while (repeat);
    return;
  }

  if (type == WebServer::HEAD)
	{
	  server.httpSuccess();
	}

	if (type == WebServer::GET)
  {
	  if (strcmp(url_tail, "/getColors") == 0) {
		  server.httpSuccess("application/json");
		  String json = "{\"red\":";
		  json += red;
		  json += ",\"green\":";
		  json += green;
		  json += ",\"blue\":";
		  json += blue;
		  json += ",\"intensity\":";
		  json += intensity;
		  json += ",\"rainbow\":";
		  json += rainbow;
		  json += "}";
		  server.print(json);
		  return;
	  }
	  
	  server.httpSuccess("text/html");
	  
	  if (strcmp(url_tail, "/setDefaultColor") == 0) {
		  setDefaultColor();
		  return;
	  }
	  
	  if (strcmp(url_tail, "") == 0) {
		  url_tail = "index.htm";
	  }
	  
	  File dataFile = SD.open(url_tail);

	  if (dataFile) {
	      while (dataFile.available()) {
	        server.write(dataFile.read());
	      }
	      dataFile.close();
	    }
		else
		{
			P(failMsg) =
			    "HTTP/1.0 404 File Not Found" CRLF
			    WEBDUINO_SERVER_HEADER
			    "Content-Type: text/html" CRLF
			    CRLF
			    WEBDUINO_FAIL_MESSAGE;

			  server.printP(failMsg);
		}
  }
}

void setup()
{
	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);

	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);
	
	pinMode(SD_PIN, OUTPUT);
	digitalWrite(SD_PIN, HIGH);
	
	SD.begin(SD_PIN);
	
	Ethernet.begin(mac, ip);
	
	webserver.setDefaultCommand(&webHandler);
	webserver.setFailureCommand(&webHandler);
	
	webserver.begin();
	
	setDefaultColor();
}

void loop()
{
	time += 0.01;
	
	if (time > 2048) time = 0;
	
	webserver.processConnection();
	
	if (rainbow == true)
	{
		rainbowAtTime(time, 2048.0);
	}
	
	int sendRed = floor((float)(red / 255.0) * (float)intensity);
	int sendGreen = floor((float)(green / 255.0) * (float)intensity);
	int sendBlue = floor((float)(blue / 255.0) * (float)intensity);
	
	analogWrite(RED_PIN, sendRed);
	analogWrite(GREEN_PIN, sendGreen);
	analogWrite(BLUE_PIN, sendBlue);
}