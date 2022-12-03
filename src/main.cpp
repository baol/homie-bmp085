#include <Homie.h>
#include <Adafruit_BMP085.h>

const int RESET_BUTTON = 0;
const int BMP_SCL = D6;
const int BMP_SDA = D7;

float lastTemp = -273.15;
float lastPressure = -1;

HomieNode temperatureNode("temperature", "temperature", "temperature");
HomieNode pressureNode("pressure", "pressure", "pressure");

Adafruit_BMP085 bmp180;

void setupHandler()
{
  temperatureNode.setProperty("unit").send("degrees");
  pressureNode.setProperty("unit").send("hectopascals");
}

void loopHandler()
{
  float temp = bmp180.readTemperature();
  if (abs(temp - lastTemp) >= 0.1)
  {
    String stemp(temp, 1);
    Homie.getLogger() << "The current temperature is " << temp << " °C." << endl;
    if (temperatureNode.setProperty("degrees").send(stemp))
    {
      lastTemp = temp;
    }
    else
    {
      Homie.getLogger() << "Failed to send temperature to MQTT host. Reset the device to reconfigure." << endl;
    }
  }

  float hpa = bmp180.readPressure() / 100.0;
  if (abs(hpa - lastPressure) >= 0.5)
  {
    String shpa(hpa, 0);
    Homie.getLogger() << "The current pressure is " << shpa << " hPa." << endl;
    if (pressureNode.setProperty("hectopascal").send(shpa))
    {
      lastPressure = hpa;
    }
    else
    {
      Homie.getLogger() << "Failed to send pressure to MQTT host. Reset the device to reconfigure." << endl;
    }
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(RESET_BUTTON, INPUT);
  digitalWrite(RESET_BUTTON, HIGH);

  Wire.pins(BMP_SDA, BMP_SCL);
  bmp180.begin();
  Homie_setFirmware("homely-bmp085", "1.0.0");
  Homie_setBrand("100allora");
  Homie.setResetTrigger(RESET_BUTTON, LOW, 3000);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");
  pressureNode.advertise("pressure");
  pressureNode.advertise("hectopascals");
  Homie.setup();
}

void loop()
{
  Homie.loop();
  delay(1000);
}
