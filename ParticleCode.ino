// This #include statement was automatically added by the Particle IDE.
#include <LiquidCrystal.h>

LiquidCrystal lcd(5, 4, 3, 2, 1, 0);
String cmd1;
String cmd2;
String cmd1sub;
String distanceStr;
String growthStr;
String timeStr;
int jarHeight;
int doughHeightStart; 
int distance; 
int growth = 0;
int timeStart;
int flag;
double pcgrowth = 0;
int led1 = D6; 
int led2 = D7; 
int timeElapsed;
int hours;
int mins;


const char *EVENT_NAME_A = "tempEvent";
const char *EVENT_NAME_B = "growEvent";
const char *EVENT_NAME_C = "timeEvent";

int resetbutton(String command)
{
  lcd.clear();
  lcd.begin(16, 2);
  cmd1 = "Restarting";
  cmd2 = "";
  cmd1sub = "";
  timeStart = (int)Time.now();
  flag = 0;
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  return 0;
}

void setup() {
  lcd.clear();
  lcd.begin(16, 2);
  Particle.function("displayTemp",tempDisplay);
  Particle.function("reset", resetbutton);
  cmd1 = "Panettone";
  cmd2 = "By Daniel Fantin";
  cmd1sub = "";
  timeStart = (int)Time.now();
  flag = 0;
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);
  jarHeight = 170;
  doughHeightStart = 30; 
}

void timeString() {
    timeElapsed = (int)Time.now() - timeStart;
    hours = timeElapsed / 3600;
    mins = (timeElapsed - hours * 3600) / 60;
    timeStr = String(hours) + "hr " + String(mins) + "min";
}

void loop() {
    delay(10000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(cmd1);
    lcd.setCursor(0,1);
    lcd.print(cmd2);
    Particle.publish(EVENT_NAME_A, cmd1sub);
    Particle.publish(EVENT_NAME_B, growthStr);
    timeString();
    Particle.publish(EVENT_NAME_C, timeStr);
    if ((int)Time.now() - timeStart > 14400 and flag == 0) // 4 hour timer
    {
        String pcstring;
        if(pcgrowth < 200) {
            pcstring = String((int)pcgrowth) + "% rise in 4hrs. You need to increase feed frequency!";
            cmd1 = String((int)pcgrowth) + "% rise in 4hrs.";
            cmd2 = "Increase feeding";
            digitalWrite(led2, HIGH);

        } else {
            pcstring = String((int)pcgrowth) + "% rise in 4hrs. Your starter is ready to use!";
            cmd1 = String((int)pcgrowth) + "% rise in 4hrs.";
            cmd2 = "Ready to use!";
            digitalWrite(led1, HIGH);
        }
        Particle.publish("EVENT_NAME_TIMER", pcstring);
        
        flag = 1;
        
    }
}

int tempDisplay(String input) {
    distanceStr = input.substring(27,30); 
    distance = distanceStr.toInt();
    growth = jarHeight - distance - doughHeightStart;
    if (growth < 0 )
    {
        growth = 0;
    }
    pcgrowth = ((double)growth/(double)doughHeightStart)*100.0;
    growthStr = String(growth);
    if(flag != 1)
    {
        cmd1 = "Temp: " + input.substring(11,15) + "C   " + input.substring(42,43) + input.substring(55,56);
        cmd2 = "Rise: " + growthStr + "mm=" + String((int)pcgrowth) + "%";
        cmd1sub = cmd1.substring(6,10);
    }
    return 0;
}
