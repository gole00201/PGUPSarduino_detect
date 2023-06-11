const int analogInputPin1 = A0;
const int analogInputPin2 = A1;
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
const unsigned long sixtySectonds = 60000; // 5 секунд продолжительность работы таймера на A1 и A2

unsigned int undangerCount = 0;
unsigned int attentionCount = 0;
unsigned int dangerCount = 0;
unsigned long timer1StartTime = 0;
unsigned long timer2StartTime = 0;
unsigned long timer3StartTime = 0;
bool timer1Running = false;
bool timer2Running = false;
bool timer3Running = false;

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
    Serial.println(analogVoltage);
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
        if (digitalRead(pin1) == HIGH && digitalRead(pin2) == HIGH && undangerCount == undangerCountThreshold || milis() - timer2StartTime >= thirtySeconds)
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
            digitalWrite(pin3, HIGH);
            digitalWrite(pin1, LOW);
            undangerCount = 0;
            attentionCount = 0;
            timer1Running = false;
            Serial.println("Timer 1 ended. Pin 1 turned off.");
        }
    } else {
        undangerCount = 0;
    }
    if (attentionCount >= 1 && !timer3Running)
    {
        timer3StartTime = milis();
        timer3Running = true;
    }
    if (milis() - timer3StartTime >= sixtySectonds)
    {
        undangerCount  = 0;
        attentionCount = 0;
        dangerCount    = 0;
        timer1Running = false;
        timer2Running = false;
        timer3Running = false;
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
    }
}

float getAnalogVoltage()
{
    int rawValue1 = analogRead(analogInputPin1);
    int rawValue2 = analogRead(analogInputPin2);
    float voltage = ((rawValue1 + rawValue2) / 2) * (5.0 / 1023.0);
    return voltage;
}