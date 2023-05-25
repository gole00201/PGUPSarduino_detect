const int analogInputPin = A0;
const int pin1 = 4; // Бдительность
const int pin2 = 2; // Опасность
const int pin3 = 3; // Запуск генератора
const float minVoltage = 0.5;
const float maxVoltage = 4;
const float dangerVoltage = 4.5;
const int attentionThreshold = 10;
const int dangerBuffer = 5;
const unsigned long undangerCountThreshold = 10000;
const unsigned long thirtySeconds = 30000; // 30 секунд продолжительность работы таймера на A1 и A2  

unsigned int undangerCount = 0;
unsigned int attentionCount = 0;
unsigned int dangerCount = 0;
unsigned long timer1StartTime = 0;
unsigned long timer2StartTime = 0;
bool timer1Running = false;
bool timer2Running = false;

void setup()
{
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    Serial.begin(9600);
}

void loop()
{
    float analogVoltage = getAnalogVoltage();
    if (analogVoltage >= minVoltage && analogVoltage <= maxVoltage)
    {
        if (digitalRead(pin1) == HIGH || digitalRead(pin2) == HIGH)
        {
            undangerCount++;
        }
    }

    if (analogVoltage > maxVoltage && analogVoltage < dangerVoltage)
    {
        attentionCount++;

        if (attentionCount == attentionThreshold && !timer1Running)
        {
            digitalWrite(pin3, LOW);
            digitalWrite(pin1, HIGH);
            undangerCount  = 0;
            timer1StartTime = millis();
            timer1Running = true;
            Serial.println("Attention threshold reached. Timer 1 started.");
        }

    }

    if (analogVoltage > dangerVoltage)
    {
        dangerCount++;

        if (digitalRead(pin1) == HIGH && dangerCount == dangerBuffer && !timer2Running)
        {
            digitalWrite(pin2, HIGH);
            undangerCount  = 0;
            timer2StartTime = millis();
            timer2Running = true;
            Serial.println("Danger threshold reached. Timer 2 started.");
        }

    }

    if (analogVoltage >= minVoltage && analogVoltage <= maxVoltage)
    {
        if (digitalRead(pin1) == HIGH && digitalRead(pin2) == HIGH && undangerCount == undangerCountThreshold)
        {
            Serial.print('\n');
            undangerCount  = 0;
            attentionCount = 0;
            dangerCount    = 0;
            timer1Running = false;
            timer2Running = false;
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
            digitalWrite(pin3, HIGH);
            Serial.println("Undanger condition met. Pins 1 and 2 turned off. Pin 2 turned on.");
        }
        if (timer1Running && millis() - timer1StartTime >= thirtySeconds && digitalRead(pin1) == HIGH && digitalRead(pin2) == LOW)
        {
            digitalWrite(pin1, LOW);
            undangerCount = 0;
            attentionCount = 0;
            timer1Running = false;
            Serial.println("Timer 1 ended. Pin 1 turned off.");
        }
    } else {
        undangerCount = 0;
    }
}

float getAnalogVoltage()
{
    int rawValue = analogRead(analogInputPin);
    float voltage = rawValue * (5.0 / 1023.0);
    return voltage;
}