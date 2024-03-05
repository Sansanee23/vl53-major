//BKKPAD:96531248
//IoT_1Mbps:1234567890
//log http://192.168.40.21/majorette_pp/pp-log-display.php
//log http://bunnam.com/projects/majorette_pp/pp-log-display.php
// touch http://192.168.40.21/majorette_pp/update/touch.php?id_mc=02-37&id_rfid=0004902042
// touch http://bunnam.com/projects/majorette_pp/update/touch.php?id_mc=02-37&id_rfid=0004902042
// reset touch http://192.168.40.21/majorette_pp/update/touch-reset.php?id_mc=02-37
// reset touch http://bunnam.com/projects/majorette_pp/update/touch-reset.php?id_mc=02-00
// dashboard https://bunnam.com/projects/majorette_pp/pp-machine.php
// dashboard http://192.168.40.21/majorette_pp/pp-machine.php
// OTA http://pd02-32/update
// http://pd02-32/webserial
#define URL_BUNNAM  //Uncomment when use URL Public --------------> NO USE since 24nov2022
#define DEV_ver "v.1.7-22nov2022"
#define INT_CH1 5  // x
#define INT_CH2 17  // y
#define INT_CH3       18  // X
#define INT_CH4       19  // reserve
//====================================EEPROM Address
#define addr_ssid 0x60
#define addr_pass 0x50
#define addr_mc_no 0x40
#define addr_ee_qty 0x30
#define addr_ee_count 0x20
#define addr_chk_ee 0x10
#define send_url 0x0
//====================================I2C Address
#define I2CADDR 0x20
#define RDM6300_RX_PIN 16
#include <Wire.h>
#include <SH1106Wire.h>
#include <Keypad_I2C.h>
#include <rdm6300.h>
#include <WiFi.h>
#include <time.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "string.h"
#include "stdlib.h"
#include "dw_font.h"
#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>
#include <ESPmDNS.h>
#include "Adafruit_VL53L0X.h"

AsyncWebServer server(80);
//19feb
int err_msg;
int old_mc_no = 0;
int x_read, y_read, z_read, result, pre_result, pre_dataqty;
uint8_t flag0, flag1, flag2, flag3, flag4;
// -------------------------------------------------------
#define time_interval 1000
#define connecttime 50000
#define connecttime1 500
#define EEPROM_SIZE 512

#define ERR_TOUCH_001 1
#define ERR_COUNT_002 2
#define ERR_BREAK_003 3
#define ERR_COUNT_007 7
#define ERR_TOUCH_008 8
#define ERR_BREAK_009 9
#define ERR_QUITE_010 10
#define ERR_QUITE_011 11
#define ERR_CONTINUE_012 12
#define ERR_BREAK_013 13
#define ERR_BREAK_014 14
#define ERR_DOWNTIME_015 15
#define ERR_TECH_016 16

#define STATUS_OK 0
#define ERR_GET_NULL -2
#define ERR_JSON_STACK -1

#define LINE1 8
#define LINE2 23
#define LINE3 38
#define LINE4 53
#define LINE5 68
/*
  #define LINE1       10
  #define LINE2       25
  #define LINE3       40
  #define LINE4       55
  #define LINE5       70
*/
#define TAB0 0
#define TAB5 5
#define TAB10 10
#define TAB15 15
#define TAB20 20
#define TAB40 40
//void easter_eggs();
void print_cf_cc_5line4();
//int read_no_pulse3();
void star_line1();
void star_line2();
unsigned int p_result = 0, p_Qty_MC = 0;
String str_log = "";
unsigned int time_out = 0x05FF;
unsigned int time_out1 = 0x0FFF;
bool flag_count_down = 0;
short counter_ = 0;
//short counter__= 0;
String model_no = "";
String mcName;
int Sub = 0;
unsigned int Qty_MC;
bool ledState = 1;
bool ButtonState = 0;
byte SubState = 0;
int tmp;
bool sendURL = 0;
int _url;
SH1106Wire display(0x3c, 21, 22);                     // ADDRESS, SDA, SCL
extern dw_font_info_t font_th_sarabun_new_regular14;  // font TH
dw_font_t myfont;

unsigned long period_get = 5000;
unsigned long period_post = 3000;
unsigned long last_time_get = 0;
unsigned long last_time_post = 0;
// setting WiFi and Time server
const char* ssid;
const char* password;
String setssid;
char setpassword;
String readssid;
String readpassword;
String wifi1 = "";
int ok;
unsigned int f_multiplier;
//void count_sender(void);
// --------------------------- set keypad
const byte ROWS = 4;  //four rows
const byte COLS = 4;  //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  { '1', '4', '7', '*' },
  { '2', '5', '8', '0' },
  { '3', '6', '9', '#' },
  { 'A', 'B', 'C', 'D' }
};
byte rowPins[ROWS] = { 3, 2, 1, 0 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 6, 5, 4 };  //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad_I2C customKeypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR);
Rdm6300 rdm6300;
//-------------------------------------------------------
int a = 32, b = 0, staterf = 0, con = 0;
int set = 0, c = 0, key = 0, num1;
int RFstate = 0; int keystate = 0; int setsh ; int setb = 0;
int settingmachine = 0; int numrole;
int u = 18; //10;
String l; String pass;  String IDcard;  String IDcard1; String IDcard2;
String machine; String customKeyset; String readmachine="02-00";
int confirmRF = 0;  int confirmtime = 0; int settingmenu = 0;
unsigned int prev_time, prev_time1;
byte led_state = 0;
char customKey1; char customKey; char customKey2;
int dataqty , datacount;
int readcount, readqty , chack;                         // chack = ตรวจสถานะไฟตก
int n; int pauseset = 0; int f = 0; int setmain = 0;
String tem; String tem1; String code;
//------------------------------------------------------- set employ
typedef struct employ_touch {
  char* id_staff ;
  char* name_first ;
  char* name_last ;
  uint8_t role ;
  char * id_task ;
  char * id_job ;
  char * item_no ;
  char* operation ;
//12 oct
  char * op_color ;
  char * op_side ;
//
  char * op_name ;  
  char * qty_order ;
  char * qty_comp ;
  char * qty_open ;
  uint8_t flag_err ;
//12 oct
  char * multiplier ;
//26 oct
  char * qty_per_tray ;
  int qty_shif_pulse1 ;
  int qty_shif_pulse2 ;
  int qty_shif_pulse3 ;
//12sep2022
  char * activity_type;
  char * id_activity;
  char * id_break;
//
  char * code_downtime;
} employ_touch_TYPE ;
typedef enum {
  BR_NOBREAK,
  BR_CUTOFF ,
  BR_TOILET ,
  BR_LUNCH
} break_type ;
break_type state_break ;
volatile bool interruptWork;
volatile bool interruptBreak;
int workCounter;
int breakCounter;
//------- 13/11/64
volatile bool interruptSleep;
volatile bool interruptDown;
int SleepCounter;
int DownCounter;
int DownTimeState;
volatile bool isWork = 0;
volatile bool isBreak = 0;
const int interruptPin = 21;
volatile uint32_t count, qty;
volatile int8_t flag = 0;
char buff[300];
char buff1[300];
char buff2[300];
char buff3[300];
String msg, msg1;
static employ_touch_TYPE dst;
String httpPOSTRequest(const char* serverName , const char* msg) ;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimerWork() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptWork = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR onTimerBreak() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptBreak = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
//-------- 13/11/64
void IRAM_ATTR onTimerSleep() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptSleep = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR onTimerDown() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptDown = 1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR countUpCH1 (void) {
  if (flag1 == 0)
    flag1 = 1;
}
void IRAM_ATTR countUpCH2 (void) {
  if (flag2 == 0)
    flag2 = 1;
}
void IRAM_ATTR countUpCH3 (void) {
  if (flag3 == 0)
    flag3 = 1;
}
void IRAM_ATTR countUpCH4 (void) {
  if (flag4 == 0)
    flag4 = 1;
}
// -------------------------------------
const int xshut0 = 27; //x
const int xshut1 = 25; //y
Adafruit_VL53L0X lox0 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
float yt=160;
float y3=0;
float y2=0;
float x=0;
float t1=0;
int p = 0;

String translate_hh_mm_cc(int sec) {
  int h, m, s;
  String buff = "";
  h = (sec / 3600);
  m = (sec - (3600 * h)) / 60;
  s = (sec - (3600 * h) - (m * 60));
  buff += (h > 9) ? "" : "0";
  buff += h;
  buff += ": ";
  buff += (m > 9) ? "" : "0";
  buff += m;
  buff += ": ";
  buff += (s > 9) ? "" : "0";
  buff += s;
  return buff;
}
///////////////////WebSerial/////////////////////////////////
void recvMsg(uint8_t* data, size_t len) {
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  WebSerial.println(d);
}
void mDNS() {
  EEPROM.get(addr_mc_no, readmachine);
  char mdnsName[10];
  mcName = String("pd") + readmachine;
  mcName.toCharArray(mdnsName, 10);
  char* host = mdnsName;
  if (!MDNS.begin(host)) {  //OTA// http://pd02-xx.local/update       //WebSerial// http://pd02-xx.local/webserial
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
}
void setup() {
  Wire.begin();
  //Wire.setClock(100000);
  EEPROM.begin(EEPROM_SIZE);
  customKeypad.begin();
  rdm6300.begin(RDM6300_RX_PIN);
  Serial.begin(38400);
  star_line2();
  Serial.println("\n\n\nMajorette (Thailand) Co.,LTD. ---> Start");
  pinMode(INT_CH1, INPUT);
  pinMode(INT_CH2, INPUT);
  pinMode(INT_CH3, INPUT );
  pinMode(INT_CH4, INPUT );
   while (! Serial) {
    delay(1);
  }
  //13sep2022
  //--- timer
  timer = timerBegin(0, 80, true);
  timerDetachInterrupt(timer);
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  dw_font_init(&myfont, 128, 64, draw_pixel, clear_pixel);
  dw_font_setfont(&myfont, &font_th_sarabun_new_regular14);
  // Wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // EEPROM.get(send_url, sendURL);
  Serial.print("sendURL: ");
  Serial.println(sendURL);
  EEPROM.get(addr_ssid, readssid);
  EEPROM.get(addr_pass, readpassword);
  ///////////// <TB>
  // EEPROM.put(addr_mc_no, readmachine);
  // EEPROM.commit();
  ///////////// </TB>
  Serial.print("SSID: ");
  Serial.println(readssid);
  Serial.print("Password: ");
  Serial.println(readpassword);
  ssid = readssid.c_str();
  password = readpassword.c_str();
  WiFi.begin(ssid, password);
  
  if (WiFi.status() != WL_CONNECTED) {
    display.clear();
    dw_font_goto(&myfont, TAB0, LINE1);
    dw_font_print(&myfont, "Majorette (Thailand) Co.,LTD");
    dw_font_goto(&myfont, TAB0, LINE2);
    dw_font_print(&myfont, "-----------------------------------------");
    dw_font_goto(&myfont, TAB0, LINE3);
    dw_font_print(&myfont, "กำลังเชื่อมต่อ WiFi หรือกด A");
    ssid = readssid.c_str();
    password = readpassword.c_str();
    dw_font_goto(&myfont, 3, LINE4);
    sprintf(buff, "SSID: %s", ssid);
    dw_font_print(&myfont, buff);
    display.display();
    ssid = "";
    password = "";
    con = 0;
    mDNS();
    /////////////////////////////////OTA///////////////////////////////////
    AsyncElegantOTA.begin(&server);  // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");
    //////////////////////////////////////////////////////////////////////
    WebSerial.begin(&server);
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);
    server.begin();
    /////////////////////////////////////////////////////////////////////
  }
  EEPROM.get(addr_mc_no, readmachine);
  Serial.print("machine: ");
  Serial.println(readmachine);
  Serial.println(DEV_ver);
  star_line1();
  tmp = workCounter;
  //26sep2022
  /*
  #ifdef URL_BUNNAM
    sendURL = 0;
  #else
    sendURL = 1;
  #endif
*/

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  EEPROM.get(addr_mc_no, readmachine);
  Serial.print("machine: ");
  Serial.println(readmachine);
  Serial.println(DEV_ver);
  star_line1();
  tmp = workCounter;
  Serial.println("log_mac_address_when_start : system start");
  delay(5000);  //delay for wait wifi connected succssful
  log_mac_address_when_start(0); 

}
int query_Continue_GetMethod( char* id_activity, char* activity_type, char* id_rfid, char* id_mc, char* id_break) {
  String msg = " ";
  if(sendURL==0)
    sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/continue_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&id_break=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, dst.id_break);
 else
    sprintf( buff , "http://192.168.40.21/majorette_pp/update/continue_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&id_break=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, dst.id_break);
  star_line2();
  Serial.println(buff);
  msg = httpGETRequest(buff);
  if ( msg != "null") {
    Serial.print("msg: "); Serial.println( msg );
    star_line1();
    //Serial.println( msg.length() );
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("Error Continue!");
      return -1;
    }
    if (doc["code"])    {
      return ((doc["code"]).as<int>());
    }
    if ( doc["total_break"] ) {
      //star_line1();
      //Serial.println((const char *)(doc["total_break"]));
      //star_line1();
      return 0;
    }
  }
  else  {
    Serial.println("Error!");
    return -2;
  }
}
void loop() {
  // show Wifi options
 if (WiFi.status() != WL_CONNECTED) {
    customKey1 = customKeypad.getKey();
    while (customKey1 != NO_KEY && customKey1 == 'A' && WiFi.status() != WL_CONNECTED) {
      if (con == 0) {
        key = 0; set = 0; b = 0;
        ssid = "";  password = ""; pass = "";
        display.resetDisplay();
        display.clear();
        dw_font_goto(&myfont, 35, LINE1);
        dw_font_print(&myfont, "กำลังสแกน WiFi");
        display.display();
        con = 1;
      }
      if (con == 1) {
        if (set == 0) {
          int n = WiFi.scanNetworks();
          if (n == 0) {
            display.drawString(19, 0, "No networks found.");
          }
          display.clear();
          display.drawString(0, 0, "Select SSID:");
          display.display();
          for (int i = 0; i < 5; ++i) {
            String ion = WiFi.SSID(i);
            wifi1 = String(i + 1) + String(": ") + String(ion);
            Serial.println(wifi1);
            b = b + 10;
            display.drawString(0, b, wifi1);
            display.display();
            set = 1 ;
            customKey = NO_KEY;
          }
        }
        //get value keypad to select Wifi
        String readstr;
        char customKey = customKeypad.getKey();
        if (customKey != NO_KEY && key == 0) {
          readstr += customKey;
          num1 = readstr.toInt();
          l = WiFi.SSID(num1 - 1);
          setssid = l;
          ssid = l.c_str();
          display.clear();
          display.drawString(0, 0, "SSID:");
          display.drawString(30, 0, ssid);
          display.drawString(0, 10, "Pass: ");
//print_cf_cc_5line4();

dw_font_goto(&myfont, 5, LINE4);
dw_font_print(&myfont, "* ยืนยัน              ย้อนกลับ #");
display.display();

          key = 1;
          customKey = NO_KEY;
       }
        // put password in SerialMonitor and connext
        //----------------
          if (customKey != NO_KEY && key == 1) {
          if (customKey != '*' && customKey != '#') {
            pass += customKey;
            String customKey1 = String(customKey);
            display.drawString(a, 10, customKey1);
            a = a + 7;
            if (a >= 120) {
              a = 32;
            }
            display.display();
            customKey = NO_KEY;
          }
          if (customKey == '*') {
            password = pass.c_str();
            keystate = 1; b = 0; a = 32;
            customKey = NO_KEY;
            display.drawString(0, LINE2, "Wait....");
            display.display();
          }
          else if (customKey == '#') {
            set = 0; b = 0; key = 0; a = 32;
            ssid = "";  password = ""; pass = ""; readstr = "";
            customKey = NO_KEY;
            display.clear();
          }
          if (keystate == 1) {
            keystate = 0;
            WiFi.begin(ssid, password);
            EEPROM.put(addr_ssid , setssid);
            EEPROM.put(addr_pass, pass);
            EEPROM.commit();
            delay(2000);
            // connect wait completed
            while (WiFi.status() != WL_CONNECTED) {
              if (millis() - prev_time > connecttime) {
                key = 0; con = 1; set = 0; b = 0;
                prev_time = millis();
                ssid = "";  password = ""; pass = "";

                display.drawString(30, LINE2, "WiFi Not connect");
                display.display();
                Serial.println("log_mac_address_when_start : WiFi Not connected");
                delay(5000);  //delay for wait wifi connected succssful
                log_mac_address_when_start(1); 
                display.clear();
                break;
              }
            }
            if (key == 1) {
              display.drawString(34, LINE2, "        ");
              display.display();
              display.drawString(30, LINE2, "WiFi connected");
              display.display();
              key = 0, con = 0, set = 1;
              //ssid = "";  password = ""; pass = "";
              delay(3000);
              display.clear();
              display.resetDisplay();
            }
          }
        }
      }
    }
    if (customKey1 != NO_KEY && customKey1 == 'B' && WiFi.status() != WL_CONNECTED && con != 1) {
      if (customKey1 == 'B') {
        display.resetDisplay();
        settingmachine = 1;
        settingmenu = 0;
        customKey1 = NO_KEY;
      }
    }
    if (settingmachine == 1) {
      dw_font_goto(&myfont, 10, LINE1);
      dw_font_print(&myfont, "โปรดใส่เลขรหัสเครื่อง");
      display.display();
      if (u == 24) {
        display.drawString(24, LINE1, "-");
        machine += "-";
        u = 31;
      } else if (u == 45) {
        print_cf_cc_5line4();
        /*
dw_font_goto(&myfont, 5, LINE4);
dw_font_print(&myfont, "* ยืนยัน              ย้อนกลับ #");
display.display();
*/
      } else if (u >= 46) {
        dw_font_goto(&myfont, 20, LINE2);
        dw_font_print(&myfont, "เกิดข้อผิดพลาด");
        display.display();
        delay(3000);
        display.resetDisplay();
        machine = "";
        customKeyset = "";
        u = 10;
      }
      // put numbet machine
      customKey = customKeypad.getKey();
      if (customKey != NO_KEY) {
        if (customKey != NO_KEY && customKey != '*' && customKey != '#') {
          machine += customKey;
          customKeyset = String(customKey);
          display.drawString(u, 10, customKeyset);
          u = u + 7;
          display.display();
          customKey = NO_KEY;
        } else if (customKey == '*') {
            Qty_MC = 0;
            customKey = NO_KEY;
            settingmachine = 0;
            Serial.println(machine);
            EEPROM.put(addr_mc_no, machine);
            EEPROM.commit();
          machine = "";
          u = 10;
          display.resetDisplay();
          dw_font_goto(&myfont, 20, LINE2);
          dw_font_print(&myfont, "Please restart ");
          display.display();
        } else if (customKey == '#') {
          u = 10;
          customKey = NO_KEY;
          settingmachine = 1;
          machine = "";
          display.resetDisplay();
        }
      }
    }
  }
  //-------------------- upper scen WiFi and config ----------------\\
  // connect completed
  if (WiFi.status() == WL_CONNECTED) {
    if (RFstate == 0 && pauseset != 1 && confirmRF != 2) {
      //---------- 13/11/64
      timerAttachInterrupt(timer, &onTimerSleep, true);
      timerAlarmWrite(timer, 1000000, true);
      timerAlarmEnable(timer);
      //------------------
      EEPROM.get(addr_mc_no, readmachine);
      display.clear();
      //9nov2022
      dw_font_goto(&myfont, 22, LINE2);
      //Serial.print("++sendURL: "); Serial.println(sendURL);
      if (sendURL == 0)
        sprintf(buff, "(P) mc/no. %s", readmachine);
      else
        sprintf(buff, "Machine No. %s", readmachine);
      /*      
      #ifdef URL_BUNNAM
        sprintf( buff , "(P) mc/no. %s", readmachine);
      #else
        sprintf( buff , "(L) mc/no. %s", readmachine);
      #endif
*/
      dw_font_print(&myfont, buff);
      dw_font_goto(&myfont, 15, LINE3);
      dw_font_print(&myfont, "โปรดทำการสแกนบัตร");
      //------------13/11/64
      if (interruptSleep) {
        portENTER_CRITICAL_ISR(&timerMux);
        interruptSleep = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        SleepCounter++;
        Sub = 0;
      }
      dw_font_goto(&myfont, 40, LINE4);
      sprintf(buff, "%s", translate_hh_mm_cc(SleepCounter));
      dw_font_print(&myfont, buff);
      display.display();
      easter_eggs();
      /*
//------------ easter_eggs()
      customKey1 = customKeypad.getKey();
      //customKey3 = customKeypad.getKey();
      if (customKey1 != NO_KEY) {
        Serial.print("customKey1 : ");
        Serial.print(customKey1);
        Serial.print(" + Count : ");
        Serial.println(counter_);

        if (customKey1 == 'A' && !flag_count_down) {
          Serial.println("A");
          flag_count_down = 1;
          counter_++;
          //counter__++;
        }
        if (customKey1 == 'C' && counter_ == 1) {
          Serial.println("C");
          counter_++;
          //counter__++;
        }
        if (customKey1 == 'A' && counter_ == 2) {
          Serial.println("A");
          counter_++;
          Serial.println("A + C + A !!");
          Serial.println("Success !");
        
          WiFi.disconnect();
          EEPROM.begin(EEPROM_SIZE);
          EEPROM.put(addr_ssid , 0);
          EEPROM.put(addr_pass, 0);
          EEPROM.put(addr_ee_count, 0);
          readmachine = "02-00";
          old_mc_no = 1;
          EEPROM.put(addr_mc_no, readmachine);      // Fill machine no.
          EEPROM.commit();
          
          con = 0;
          //time out
          time_out = 0x05FF;
          flag_count_down = 0;
          counter_ = 0;
          display.clear();
          dw_font_goto(&myfont, 5, 36);
          dw_font_print(&myfont, "โปรดเลือก A เพื่อเลือก WiFi");
          display.display();
        }
        if (customKey1 == 'B' && counter_ == 2) {
          Serial.println("B");
          counter_++;
          sendURL= !sendURL;
          Serial.println("A + C + B !!");
          Serial.print("sendURL: ");
          Serial.print(sendURL);
          EEPROM.put(send_url , sendURL);
          EEPROM.commit();
          Serial.print(" - switch to ");
          if(sendURL==0)
            Serial.println("Public URL.");
          else
            Serial.println("Local URL.");
          con = 0;
          //time out
          time_out = 0x05FF;
          flag_count_down = 0;
          counter_ = 0;
          delay(2000);
        }
      }
      else {
        if (flag_count_down) {
          if (time_out < 100) {
            Serial.println("Time Out !");
            time_out = 0x05FF;
            flag_count_down = 0;
            counter_ = 0;
          }
          time_out--;
        }
      }
//----------- easter_eggs()
*/
      if (rdm6300.update()) {
        msg = String(rdm6300.get_tag_id());
        if (strlen((const char*)msg.c_str()) == 8)
          msg = String("00") + msg;
        else if (strlen((const char*)msg.c_str()) == 7)
          msg = String("000") + msg;
        Serial.print("RFID No.: ");
        Serial.println(msg);
        if (msg != "") {
          ///////////// <TB>
          // Serial.print("ID MC: ");
          // EEPROM.get(addr_mc_no, readmachine);
          ///////////// </TB>
          switch (query_Touch_GetMethod((const char*)readmachine.c_str(), (const char*)msg.c_str(), &dst))  // <-- Normal Case
          //switch(query_Touch_GetMethod( (const char *)"1234", (const char *)"1234" , &dst)) // <-- Test Case Error!!
          {
            case 0:
              IDcard = msg;
              //attachInterrupt(digitalPinToInterrupt(PIN_COUNTER), countUp, CHANGE );
              //attachInterrupt(digitalPinToInterrupt(INT_CH1), countUpCH1, CHANGE );
              attachInterrupt(digitalPinToInterrupt(INT_CH1), countUpCH1, FALLING );
              attachInterrupt(digitalPinToInterrupt(INT_CH2), countUpCH2, HIGH );
              attachInterrupt(digitalPinToInterrupt(INT_CH3), countUpCH3, HIGH );
              attachInterrupt(digitalPinToInterrupt(INT_CH4), countUpCH4, HIGH );
              sprintf(buff, "ID : %s TIMESTAMP : %s VALUE : %s", dst.id_staff, dst.name_first, dst.name_last);
              numrole = dst.role;
              //12 0ct เพิ่มอีก 3 param
              WebSerial.print("multiplier : "); WebSerial.println(dst.multiplier);
              Serial.print("multiplier : ");    Serial.println(dst.multiplier);
              Serial.print("op_color   : ");      Serial.println(dst.op_color);
              Serial.print("op_side    : ");       Serial.println(dst.op_side);
              Serial.print("item_no    : ");       Serial.println(dst.item_no);
              Serial.print("Role       : ");
              display.resetDisplay();

              dw_font_goto(&myfont, 0, LINE1);
              sprintf(buff ,"mc/no. %s", readmachine);
              dw_font_print(&myfont, buff);
              display.display();

              dw_font_goto(&myfont, 0, LINE2);
              sprintf(buff ,"ID : %s", dst.id_staff);
              dw_font_print(&myfont, buff);

              dw_font_goto(&myfont, 60, LINE2);
              if (dst.role == 1) {
                Serial.println("Operator");
                sprintf(buff ,"%s" ,"[Operator]");
              }
              else {
                Serial.println("Technician");
                sprintf(buff ,"%s" ,"[Technician]");
              }
              star_line1();
              dw_font_print(&myfont, buff);

              dw_font_goto(&myfont, 0, LINE3);
              sprintf(buff ,"ชื่อ : %s" ,dst.name_first);
              dw_font_print(&myfont, buff);

              sprintf(buff, " %s" ,dst.name_last);
              dw_font_print(&myfont, buff);
              Serial.println("***************************");
              dw_font_goto(&myfont, 5, LINE4);
              sprintf(buff, "%s",  "* ยืนยัน              ย้อนกลับ #");
              dw_font_print(&myfont, buff);
              display.display();
              noInterrupts();
              msg = "";
              tem = "";
              confirmRF = 2;
              interrupts();
              rdm6300.update();
              while (!rdm6300.update()) {
                break;
              }
              break ;
             case 1:
              Serial.println("ERR_TOUCH_001");
              display.clear();
              dw_font_goto(&myfont, 15, LINE3);
              dw_font_print(&myfont, "ERR_TOUCH_001");
              display.display();
              delay(1500);
              rdm6300.update();
              while (!rdm6300.update()) {
                break;
              }
              Serial.println("+++++++++ end of  case 001");
              break;
            //19feb
            case -1:
              Serial.print("err_msg:");
              Serial.println(err_msg);

              dw_font_goto(&myfont, 5, LINE4);
              //sprintf( buff1 , "-%d ", err_msg);
              //dw_font_print(&myfont, buff1);
              sprintf(buff, "-%d ", err_msg);
              dw_font_print(&myfont, buff);
              display.display();
              delay(1000);
              err_msg = 0;
              Serial.println("+++++++++ end of  case -1");
          }
          noInterrupts();
          rdm6300.update();
          while (!rdm6300.update()) {
            break;
          }  // switch
          //Serial.println("+++++++++ end of switch");
          interrupts();
        }  // if
        //Serial.println("+++++++++ end of if");
      }
      //Serial.println("+++++++++ end of if (rdm6300.update()");
    }
    //Serial.println("+++++++++ wait cf/cc");
    if (confirmRF == 2) {
      // confrim Data RFID
      customKey1 = customKeypad.getKey();
      if (customKey1 != NO_KEY) {
        //confrim
        if (customKey1 == '*') {
          //star_line2();
          Serial.println("ยืนยัน");
          star_line1();
          //09nov2022
          /*
if(dst.role == 2) {
Serial.println("dst.id_task         : "+String(dst.id_task));
Serial.println("readmachine.c_str() : "+String(readmachine.c_str()));
Serial.println("dst.id_staff        : "+String(dst.id_staff));
dst.activity_type= "3"; // 1=BF, 2=RW, 3=DT

Serial.print("dst.activity_type    : ");  Serial.println(dst.activity_type);
query_Activity_GetMethod(dst.id_task, (char*)readmachine.c_str(), dst.id_staff, dst.activity_type, &dst);
}
*/
          timerAttachInterrupt(timer, &onTimerWork, true);
          timerAlarmWrite(timer, 1000000, true);
          timerAlarmEnable(timer);
          //---- 13/11/64
          SleepCounter = 0;
          if (numrole == 2)
            settingmenu = 1;
          else setmain = 1;
          display.clear();
          display.resetDisplay();
          msg = "";
          tem = "";

          rdm6300.update();
          customKey1 = NO_KEY;
          while (!rdm6300.update()) {
            rdm6300.update();
            break;
          }
        }
        // unconfrimed - cancel
        else if (customKey1 == '#') {
          star_line2();
          Serial.println("ยกเลิก");
          star_line2();
          EEPROM.get(addr_ee_count, readcount);
          String h = String(readcount);
          String strqty = String(dataqty);
          Serial.print("dst.id_activity       = ");          Serial.println(String(dst.id_activity));
          Serial.print("dst.activity_type     = ");          Serial.println(String(dst.activity_type));
          Serial.println("IDcard.c_str()      = " + String(IDcard.c_str()));
          Serial.println("readmachine.c_str() = " + String(readmachine.c_str()));
          Serial.println("h/readcount         = "+h);
          Serial.println("strqty/dataqty      = "+strqty);
//12sep2022 quit by cancel
          query_Quit_GetMethod("0", "0", (char*)IDcard.c_str(), (char*)readmachine.c_str(), "0", "0", "0", "0");
          //query_Quit_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str() ,(char*)readmachine.c_str(), "0", "0", "0", "0");
          //Serial.println("===== 4");
          confirmRF = 0;  RFstate = 0;
          settingmenu = 0; tem = ""; msg = ""; IDcard = "";
          display.clear();
          display.resetDisplay();
          rdm6300.update();
          customKey1 = NO_KEY;
          //Serial.println("===== 8");
          while (!rdm6300.update()) {
            rdm6300.update();
            break;
          }
          //Serial.println("===== 9");
        }
      }
    }
    //-----------13/11/64
  if (setmain == 1) {
      if (interruptWork) {
        portENTER_CRITICAL_ISR(&timerMux);
        interruptWork = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        workCounter++;
      }
      dw_font_goto(&myfont, 0, LINE1);
      sprintf(buff, "mc/no. %s", readmachine);
      dw_font_print(&myfont, buff);

      dw_font_goto(&myfont, 78, LINE1);
      sprintf(buff, "%s", translate_hh_mm_cc(workCounter));
      dw_font_print(&myfont, buff);

      dw_font_goto(&myfont, 0, LINE2);
      dw_font_print(&myfont, "กด A Backflush");

      dw_font_goto(&myfont, 0, LINE3);
      dw_font_print(&myfont, "กด B Rework");
      display.display();

      char Keyset = customKeypad.getKey();
      if (Keyset != NO_KEY) {
        if (Keyset == 'A') {
          Sub = 0;
          Qty_MC = 0;
          Serial.println("Select Backflash"); 
          star_line1();
          dst.activity_type= "1";
          dst.code_downtime= "X01";
          Serial.println("dst.id_task        : "+String(dst.id_task));
          Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
          Serial.println("dst.id_staff       : "+String(dst.id_staff));
          Serial.print("dst.code_downtime    : ");  Serial.println(dst.code_downtime);
          Serial.print("dst.activity_type    : ");  Serial.println(dst.activity_type);
          query_Activity_GetMethod(dst.id_task, (char*)readmachine.c_str(), dst.id_staff, dst.activity_type, dst.code_downtime, &dst);
          //Serial.print("dst.id_activity      : ");  Serial.println(dst.id_activity);
//20nov2022
          confirmRF = 1; RFstate = 1; setmain = 0;
          display.clear();
          display.resetDisplay();
          msg = ""; tem = "";
          if (chack == 0) {
            int v = 1;
            EEPROM.get(addr_ee_qty, dataqty);
            EEPROM.put(addr_chk_ee, v);
            datacount = 1;
            //dataqty = 0;
            EEPROM.put(addr_ee_count , datacount);
            EEPROM.put(addr_ee_qty, dataqty);
            EEPROM.commit();
            display.resetDisplay();
          }
        }
        else if (Keyset == 'B') {
          Serial.println("Select Rework");  
          star_line2();
//20nov2022
          dst.activity_type="2";
          dst.code_downtime= "X02";
          Serial.println("dst.id_task        : "+String(dst.id_task));
          Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
          Serial.println("dst.id_staff       : "+String(dst.id_staff));
          Serial.print("dst.code_downtime    : ");  Serial.println(dst.code_downtime);
          Serial.print("dst.activity_type    : ");  Serial.println(dst.activity_type);
          Serial.print("dst.id_activity      : ");  Serial.println(dst.id_activity);
          query_Activity_GetMethod(dst.id_task, (char*)readmachine.c_str(), dst.id_staff, dst.activity_type, dst.code_downtime, &dst);
          Serial.print("dst.id_activity      : ");  Serial.println(dst.id_activity);
          confirmRF = 1; RFstate = 1; setmain = 0;
          display.clear();
          display.resetDisplay();
          msg = ""; tem = "";
          if (chack == 0) {
            int v = 1;
            EEPROM.get(addr_ee_qty, dataqty);
            EEPROM.put(addr_chk_ee, v);
            datacount = 1;
            //dataqty = 0;
            EEPROM.put(addr_ee_count , datacount);
            EEPROM.put(addr_ee_qty, dataqty);
            EEPROM.commit();
            display.resetDisplay();
          }
        }
        Serial.println("\n\n\nOperation Start>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
      }
    }
    //--------- upper scen employ ---------\\
// show qty and cont
     if (confirmRF == 1 && numrole == 1) {
      // up qty and cont to dashboard
      if (RFstate == 1) {
        if (interruptWork) {
          portENTER_CRITICAL_ISR(&timerMux);
          interruptWork = 0;
          portEXIT_CRITICAL_ISR(&timerMux);
          workCounter++;
          //Serial.print("Work : ");
          //Serial.println(translate_hh_mm_cc(workCounter));
        }
        dw_font_goto(&myfont, 0, LINE1);
        sprintf(buff1, "mc/no. %s", readmachine);
        dw_font_print(&myfont, buff1);

        dw_font_goto(&myfont, 78, LINE1);
        sprintf(buff1, "%s", translate_hh_mm_cc(workCounter));
        dw_font_print(&myfont, buff1);

        dw_font_goto(&myfont, 0, LINE2);
        sprintf(buff1, "ชื่อ: %s %s", dst.name_first, dst.name_last);
        dw_font_print(&myfont, buff1);

        dw_font_goto(&myfont, 0, LINE3);
        //button test//

        sprintf(buff1, "Qty: %d / %s ", Qty_MC, dst.qty_order);
        dw_font_print(&myfont, buff1);
        //pp_log_display();
        display.display();
        //&&&
        if ((result != p_result) || (Qty_MC != p_Qty_MC)) {
          WebSerial.println("----------------");
          WebSerial.println("Result=" + String(result));
          WebSerial.println("Qty=" + String(Qty_MC));
          p_result = result;
          p_Qty_MC = Qty_MC;
        }
        if (flag_count_down == 0) {
          dw_font_goto(&myfont, 2, LINE4);
          //sprintf( buff1 , "%sC,Item No. %s ", dst.op_color, dst.item_no);
          sprintf(buff1, "จำนวนชิ้นงาน:");
          dw_font_print(&myfont, buff1);
        }
        display.display();
        //------------------------------------------

        if (millis() - last_time_get > period_get) {
          last_time_get = millis();

          count++;
          // EEPROM.get(addeecount, readcount);
          // EEPROM.get(addeeqty, readqty);
          EEPROM.get(addr_ee_qty, readqty);
          EEPROM.get(addr_ee_count, readcount);
          if (sendURL == 0)
            sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/count.php?id_task=%s&id_mc=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0", dst.id_task, readmachine.c_str(), readcount, Qty_MC);
          else
            //sprintf( buff , "http://192.168.40.21/SB_Admin_Pro/update/count.php?id_task=%s&id_mc=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0", dst.id_task, readmachine.c_str() , readcount, readqty);
            //sprintf(buff, "http://192.168.100.125/majorette_pp/update/count.php?id_task=%s&id_mc=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=%d", dst.id_task, readmachine.c_str(), readcount, readqty);
            sprintf(buff, "http://192.168.137.1/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=0&no_pulse2=0&no_pulse3=%d&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, dataqty);

          Serial.println(buff);
          WebSerial.println(buff);
          msg = httpPOSTRequest(buff, "");
          Serial.println(msg);
          WebSerial.println(msg);
        }
        x_read = digitalRead(INT_CH1);
        y_read = digitalRead(INT_CH2);
        result = y_read or x_read;
        if (result != pre_result) {
          Serial.println();
          Serial.print("Result: " + String(result));
          Serial.print("/ yx:");
          Serial.print(y_read);
          Serial.print(x_read);
          //WebSerial.print("y=" + String(y_read));
          //WebSerial.print("  z=" + String(z_read));
          //WebSerial.println("  x=" + String(x_read));
          Serial.print("/ ");
          pre_result = result;
          WebSerial.println("result = " + String(result));
          WebSerial.println();
        }
        //=========================================== 23nov
        /*        if(flag1==0){
                  //Serial.print(" Flag1= ");
                  Serial.print(flag1);
                }
                else {
                  Serial.print(" Flag1= ");
                  Serial.println(flag1);
                }
        */
        if (flag1 != 0) {  // มี interrupt เข้ามา
          //if (flag1==1) {
          prog_qty_1c();
          //april05
          if (digitalRead(INT_CH1) == HIGH)
            flag1 = 0;  //clear INT Flag1
        }
        // stop and pause time
        if (rdm6300.update()) {
          tem1 = String(rdm6300.get_tag_id());
          if (strlen((const char*)tem1.c_str()) == 8)
            IDcard1 = String("00") + tem1;
          else if (strlen((const char*)tem1.c_str()) == 7)
            IDcard1 = String("000") + tem1;

          if (IDcard1 == IDcard) {
            confirmtime = 1;
            IDcard1 = "";
            tem1 = "";
            flag_count_down = 1;
            time_out = 0x05FF;
            dw_font_goto(&myfont, 0, LINE4);
            dw_font_print(&myfont, "* หยุดแพด    หยุดการทำงาน # ");
            display.display();
            rdm6300.update();
            while (!rdm6300.update()) {
              break;
            }
          } else {
            display.resetDisplay();
            dw_font_goto(&myfont, 15, LINE3);
            dw_font_print(&myfont, "ID card ไม่ตรง 1");
            IDcard1 = "";
            tem1 = "";
            f = 0;
            confirmtime = 0;
            display.display();
            delay(3000);
            display.resetDisplay();
          }
          rdm6300.update();
          while (!rdm6300.update()) {
            rdm6300.update();
            break;
          }
        }
        if (flag_count_down) {
          //Serial.println(confirmtime);
          if (time_out < 100) {
            flag_count_down = 0;
            confirmtime = 0;
            display.clear();
          }
          time_out--;
        }
        // confirm time stop and pause
        if (confirmtime == 1) {
          customKey1 = customKeypad.getKey();
          if (customKey1 != NO_KEY) {
            // stop
            if (customKey1 == '#') {
              //EEPROM.get(addr_mc_no, readmachine);
              //9 oct //18 jan //send when quit
              if (sendURL == 0)
                sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, dataqty);
              else
                sprintf(buff, "http://192.168.137.1/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=0&no_pulse2=0&no_pulse3=%d&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, dataqty);
              /*              
              #ifdef URL_BUNNAM
                sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty);
              #else  
                sprintf( buff , "http://192.168.40.21/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty);
              #endif
*/
              Serial.println();
              star_line2();
              Serial.println(buff);
              msg = httpPOSTRequest(buff, "");
              star_line1();
              Serial.print("msg: ");
              Serial.println(msg);
              /*
              Serial.println("msg count quit================");
              Serial.println("------------------------------");
              Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
              Serial.println("dst.id_staff       : "+String(dst.id_staff));
              Serial.println("dst.id_activity    : "+String(dst.id_activity));
              Serial.print(  "dst.activity_type  : ");  Serial.println(dst.activity_type);
              Serial.println("no_send/readcount  : "+String(readcount));
              Serial.println("no_pulse1/readqty  : "+String(readqty));
              Serial.println("==============================");
*/
              //- timer
              EEPROM.get(addr_ee_count, readcount);
              String h = String(readcount);
              // 9 oct
              String strqty = String(dataqty);
              timerDetachInterrupt(timer);
              /* DB4 */
              //EEPROM.get(addr_ee_qty, readqty);
              query_Quit_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str(), (char*)readmachine.c_str(), (char*)h.c_str(), (char*)strqty.c_str(), "0", "0");
              /* DB4 */

              display.clear();
              dw_font_goto(&myfont, 30, LINE2);
              dw_font_print(&myfont, "ออกการทำงาน");
              dw_font_goto(&myfont, 40, LINE3);
              sprintf(buff2, "%s", translate_hh_mm_cc(workCounter));
              dw_font_print(&myfont, buff2);
              IDcard = "";
              display.display();
              delay(2000);

              workCounter = 0;
              qty = 0;
              confirmRF = 0, RFstate = 0, count = 0;
              readcount = 0;
              confirmtime = 0;
              msg = "";
              f = 0;
              EEPROM.put(addr_ee_count, readcount);
              display.resetDisplay();
              display.clear();
              chack = 0;
              EEPROM.put(addr_chk_ee, chack);
              Qty_MC = 0;
              EEPROM.put(addr_ee_qty, Qty_MC);
              EEPROM.commit();

              customKey1 = NO_KEY;

              flag_count_down = flag_count_down ? 0 : 1;
              detachInterrupt(digitalPinToInterrupt(INT_CH1));
              detachInterrupt(digitalPinToInterrupt(INT_CH2));
            }
            //pause
            else if (customKey1 == '*') {
              customKey1 = NO_KEY;
              noInterrupts();
              confirmtime = 0;
              RFstate = 0;
              pauseset = 1;
              setsh = 1;
              interrupts();
              f = 0;
              flag_count_down = flag_count_down ? 0 : 1;
              display.clear();
              display.resetDisplay();
            }
          }
        }
      } else if (pauseset == 1 && RFstate != 1) {
        //---------- 13/11/64
        char pauseKey = customKeypad.getKey();
        if (interruptBreak && pauseKey != 'C') {
          if (setb == 1) {
            //24 oct
            display.clear();
            dw_font_goto(&myfont, 0, LINE1);
            sprintf(buff1, "mc/no. %s", readmachine);
            dw_font_print(&myfont, buff1);

            dw_font_goto(&myfont, 0, LINE2);
            sprintf(buff1, "ชื่อ: %s %s", dst.name_first, dst.name_last);
            dw_font_print(&myfont, buff1);

            dw_font_goto(&myfont, 25, LINE3);
            dw_font_print(&myfont, "พักเบรกเข้า/ห้องน้ำ");
            display.display();
          } else if (setb == 2) {
            dw_font_goto(&myfont, 0, LINE1);
            sprintf(buff1, "mc/no. %s", readmachine);
            dw_font_print(&myfont, buff1);

            dw_font_goto(&myfont, 0, LINE2);
            sprintf(buff1, "ชื่อ: %s %s", dst.name_first, dst.name_last);
            dw_font_print(&myfont, buff1);

            dw_font_goto(&myfont, 25, LINE3);
            dw_font_print(&myfont, "พักเบรกทานอาหาร");
            display.display();
          }
          portENTER_CRITICAL_ISR(&timerMux);
          interruptBreak = 0;
          portEXIT_CRITICAL_ISR(&timerMux);
          breakCounter++;
          dw_font_goto(&myfont, 40, LINE4);
          sprintf(buff2, "%s", translate_hh_mm_cc(breakCounter));
          dw_font_print(&myfont, buff2);
          display.display();
        }
        if (setsh == 1) {
          dw_font_goto(&myfont, 0, LINE1);
          sprintf(buff1, "mc/no. %s", readmachine);
          dw_font_print(&myfont, buff1);

          dw_font_goto(&myfont, 0, LINE2);
          dw_font_print(&myfont, "กด A พักเบรก/เข้าห้องน้ำ");

          dw_font_goto(&myfont, 0, LINE3);
          dw_font_print(&myfont, "กด B พักเบรกทานอาหาร");
          //----- 13/11/64
          dw_font_goto(&myfont, 0, LINE4);
          dw_font_print(&myfont, "กด C Downtime");
          display.display();
        }
        if (pauseKey != NO_KEY) {
          if (pauseKey == 'A') {
            display.clear();
            display.resetDisplay();
            timerAttachInterrupt(timer, &onTimerBreak, true);
            timerAlarmWrite(timer, 1000000, true);
            timerAlarmEnable(timer);
            setb = 1;
            /* DB3 */
            //16sep2022
            query_Break_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str(), (char*)readmachine.c_str(), "1", &dst);
            pauseKey = NO_KEY;
            setsh = 0;
            /*            
Serial.println("id_break==============================");
Serial.println("id_break         : "+String(dst.id_break));
Serial.println("id_break==============================");          
*/
          } else if (pauseKey == 'B') {
            display.clear();
            display.resetDisplay();
            timerAttachInterrupt(timer, &onTimerBreak, true);
            timerAlarmWrite(timer, 1000000, true);
            timerAlarmEnable(timer);
            setb = 2;
            /* DB3 */
            query_Break_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str(), (char*)readmachine.c_str(), "2", &dst);
            pauseKey = NO_KEY;
            setsh = 0;
          }
          //---------- 13/11/64
          else if (pauseKey == 'C') {
            display.clear();
            display.resetDisplay();
            DownTimeState = 1;

            timerAttachInterrupt(timer, &onTimerDown, true);
            timerAlarmWrite(timer, 1000000, true);
            timerAlarmEnable(timer);

            pauseKey = NO_KEY;
            setsh = 0;
          }
        }
        //----------------- 13/11/64
        if (DownTimeState == 1) {
          //09nov2022
          /*
if(dst.role == 2) {
Serial.println("dst.id_task         : "+String(dst.id_task));
Serial.println("readmachine.c_str() : "+String(readmachine.c_str()));
Serial.println("dst.id_staff        : "+String(dst.id_staff));
dst.activity_type= "3"; // 1=BF, 2=RW, 3=DT

Serial.print("dst.activity_type    : ");  Serial.println(dst.activity_type);
query_Activity_GetMethod(dst.id_task, (char*)readmachine.c_str(), dst.id_staff, dst.activity_type, &dst);
}
*/
          if (interruptDown) {
            portENTER_CRITICAL_ISR(&timerMux);
            interruptDown = 0;
            portEXIT_CRITICAL_ISR(&timerMux);
            DownCounter++;
          }
          dw_font_goto(&myfont, 0, LINE1);
          sprintf(buff1, "mc/no. %s ", readmachine);
          dw_font_print(&myfont, buff1);

          dw_font_goto(&myfont, 80, LINE1);
          sprintf(buff3, "%s", translate_hh_mm_cc(DownCounter));
          dw_font_print(&myfont, buff3);

          dw_font_goto(&myfont, 4, LINE2);
          dw_font_print(&myfont, "โปรดใส่รหัสการทำงาน");

          display.drawString(5, 28, "->  _ _ _");
          display.display();
          /*
            //u
            1  18  +9
            2 27
            3 36
            4 45
          */
          if (u == 45) {
            print_cf_cc_5line4();
          } else if (u >= 46) {
            dw_font_goto(&myfont, 0, LINE3);
            dw_font_print(&myfont, "เกิดข้อผิดพลาด");
            display.display();
            delay(3000);
            display.resetDisplay();
            code = "";
            customKeyset = "";
            u = 18;
          }
          customKey = customKeypad.getKey();
          if (customKey != NO_KEY) {
            if (customKey != NO_KEY && customKey != '*' && customKey != '#') {
              code += customKey;
              customKeyset = String(customKey);
              display.drawString(u, 28, customKeyset);
              u += 9;
              display.display();
              customKey = NO_KEY;
            } 
                       else if (customKey == '*') {
//09nov2022
              if(dst.role == 2) {
                Serial.println("dst.id_task         : "+String(dst.id_task));
                Serial.println("readmachine.c_str() : "+String(readmachine.c_str()));
                Serial.println("dst.id_staff        : "+String(dst.id_staff));
                dst.activity_type= "3"; // 1=BF, 2=RW, 3=DT
                
                Serial.print("dst.activity_type    : ");  Serial.println(dst.activity_type);
                query_Activity_GetMethod(dst.id_task, (char*)readmachine.c_str(), dst.id_staff, dst.activity_type, dst.code_downtime, &dst);
              }
              settingmenu = 1; customKey = NO_KEY;
              settingmachine = 0; setb = 3; DownTimeState = 0;
              EEPROM.get(addr_ee_count, readcount);
              String h = String(readcount);
// 9 oct
              String strqty = String(dataqty);
              timerDetachInterrupt(timer);
              Serial.println(code);
              query_quit_dt_op_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str(), (char*)readmachine.c_str() , (char*)h.c_str() , (char*)strqty.c_str() , "0", "0" , dst.multiplier, (char*)code.c_str());
              u = 18;
              code = "";
              display.resetDisplay();
            }
            else if (customKey == '#') {
              display.resetDisplay();
              u = 18; settingmenu = 1; customKey = NO_KEY;
              settingmachine = 0;  code = "";
            }
          }
        }
        if (setb == 3) {
          dw_font_goto(&myfont, 0, LINE1);
          sprintf(buff1, "mc/no. %s", readmachine);
          dw_font_print(&myfont, buff1);

          dw_font_goto(&myfont, 0, LINE2);
          sprintf(buff1, "ชื่อ: %s %s", dst.name_first, dst.name_last);
          dw_font_print(&myfont, buff1);

          dw_font_goto(&myfont, 25, LINE3);
          dw_font_print(&myfont, "แตะบัตรเพื่อออก");
          display.display();

          //dw_font_goto(&myfont, 40 , LINE4);
          //sprintf(buff2 , "%s", translate_hh_mm_cc(DownCounter));
          //dw_font_print(&myfont, buff2);
          display.display();
        }
        //-----------------------------------------------------
        if (rdm6300.update()) {
          tem1 = String(rdm6300.get_tag_id());
          if (strlen((const char*)tem1.c_str()) == 8)
            IDcard1 = String("00") + tem1;
          if (strlen((const char*)tem1.c_str()) == 7)
            IDcard1 = String("000") + tem1;
          if (IDcard1 == IDcard) {
            noInterrupts();
            RFstate = 1;
            setsh = 1;
            pauseset = 0;
            interrupts();
            //----------- 13/11/64
            DownTimeState = 0;
            DownCounter = 0;
            IDcard1 = "";
            tem1 = "";
            if (setb == 3) {
              Serial.println("----------> ออกทำงาน");
              display.clear();
              dw_font_goto(&myfont, 30, LINE3);
              dw_font_print(&myfont, "ออกการทำงาน");
              dw_font_goto(&myfont, 40, LINE4);
              sprintf(buff2, "%s", translate_hh_mm_cc(workCounter));
              dw_font_print(&myfont, buff2);
              display.display();
              delay(2000);
              RFstate = 0;
              settingmenu = 0;
              settingmachine = 0;
              confirmRF = 0;
              timerDetachInterrupt(timer);
              workCounter = 0;
              IDcard2 = "";
              tem1 = "";
              setb = 0;
              rdm6300.update();
              while (!rdm6300.update()) {
                break;
              }
            } else {
              timerAttachInterrupt(timer, &onTimerWork, true);
              timerAlarmWrite(timer, 1000000, true);
              timerAlarmEnable(timer);
              /* DB3 */
              //16sep2022
              query_Continue_GetMethod(dst.id_activity, dst.activity_type, (char*)IDcard.c_str(), (char*)readmachine.c_str(), dst.id_break);
              breakCounter = 0;
              setb = 0;
              display.clear();
              display.resetDisplay();
              //------------------------------------------------------------------------- เพิ่ม
              dw_font_goto(&myfont, 30, LINE3);
              dw_font_print(&myfont, "หยุดพักเบรค");
              display.display();
              delay(2000);
              display.clear();
              rdm6300.update();
              while (!rdm6300.update()) {
                rdm6300.update();
                break;
              }
            }
          } else {
            display.resetDisplay();
            dw_font_goto(&myfont, 15, LINE3);
            dw_font_print(&myfont, "ID card ไม่ตรง 2");
            IDcard1 = "";
            tem1 = "";
            f = 0;
            display.display();
            noInterrupts();
            rdm6300.update();
            while (!rdm6300.update()) {
              break;
            }
            delay(3000);
            display.resetDisplay();
          }
          rdm6300.update();
          while (!rdm6300.update()) {
            break;
          }
        }
        interrupts();
      }
    }
    // ID_task == 2 set machine
    //----------------------------------------------------
    // setting menu
    else if (settingmenu == 1 && numrole == 2) {
      if (interruptWork) {
        portENTER_CRITICAL_ISR(&timerMux);
        interruptWork = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        workCounter++;
        Serial.print("Work3: ");
        Serial.println(translate_hh_mm_cc(workCounter));
        //techtime 24oct
        sprintf(buff1, "mc/no. %s ", readmachine);
        dw_font_goto(&myfont, 0, LINE1);
        dw_font_print(&myfont, buff1);
        dw_font_goto(&myfont, 78, LINE1);
        sprintf(buff3, "%s", translate_hh_mm_cc(workCounter));
        dw_font_print(&myfont, buff3);
        display.display();
      }
      dw_font_goto(&myfont, 5, LINE2);
      dw_font_print(&myfont, "กด A ใส่รหัสการทำงาน");
      dw_font_goto(&myfont, 5, LINE3);
      dw_font_print(&myfont, "กด B ตั้งค่า รหัสเครื่อง");
      dw_font_goto(&myfont, 5, LINE4);
      dw_font_print(&myfont, "               หยุดการทำงาน #");
      display.display();
      // เพิ่ม นาฬิกา technician
      customKey2 = customKeypad.getKey();
      if (customKey2 != NO_KEY) {
        // setting machine
        if (customKey2 == 'B') {
          display.resetDisplay();
          settingmachine = 1;
          settingmenu = 0;
          customKey2 = NO_KEY;
        } else if (customKey2 == 'A') {
          display.resetDisplay();
          settingmachine = 2;
          settingmenu = 0;
          customKey2 = NO_KEY;
        } else if (customKey2 == '#') {
          display.resetDisplay();
          settingmachine = 3;
          settingmenu = 0;
          while (!rdm6300.update()) {
            customKey2 = NO_KEY;
            break;
          }
        }
      }
    }
    // setting machine
    else if (settingmachine == 1) {
      if (interruptWork) {
        portENTER_CRITICAL_ISR(&timerMux);
        interruptWork = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        workCounter++;
        Serial.print("Work1: ");
        Serial.print(translate_hh_mm_cc(workCounter));
        Serial.print(" u: ");
        Serial.print(u);
        Serial.print(" Key: ");
        Serial.println(customKeyset);
        dw_font_goto(&myfont, 80, LINE1);
        sprintf(buff3, "%s", translate_hh_mm_cc(workCounter));
        dw_font_print(&myfont, buff3);
      }
      sprintf(buff1, "mc/no. %s ", readmachine);
      dw_font_goto(&myfont, 0, LINE1);
      dw_font_print(&myfont, buff1);
      dw_font_goto(&myfont, 4, LINE2);
      dw_font_print(&myfont, "โปรดใส่เลขรหัสเครื่อง 4 หลัก");
      display.drawString(5, 28, "-> _ _ - _ _");
      display.display();
      /*
        //u
        1  18  +9
        2 27
        - 41
        3 50
        4 59
      */
      if (u == 36) {
        display.drawString(u, 28, "-");
        machine += "-";
        u = 41;
      } else if (u == 59) {
        print_cf_cc_5line4();
      } else if (u >= 60) {
        dw_font_goto(&myfont, 0, LINE3);
        dw_font_print(&myfont, "      เกิดข้อผิดพลาด");
        display.display();
        delay(3000);
        display.resetDisplay();
        machine = "";
        customKeyset = "";
        u = 18;
      }
      // put number machine
      customKey = customKeypad.getKey();
      if (customKey != NO_KEY) {
        if (customKey != NO_KEY && customKey != '*' && customKey != '#') {
          machine += customKey;
          customKeyset = String(customKey);
          display.drawString(u, 28, customKeyset);  // _ _-_ _ first u = 18,
          u += 9;
          display.display();
          customKey = NO_KEY;
        } else if (customKey == '*') {
          settingmenu = 1;
          customKey = NO_KEY;
          Qty_MC = 0;
          settingmachine = 0;
          Serial.println(machine);
          readmachine = machine;
          EEPROM.put(addr_mc_no, readmachine);
          EEPROM.commit();
          machine = "";
          u = 18;
          display.resetDisplay();
          mDNS();
        } else if (customKey == '#') {
          u = 18;
          settingmenu = 1;
          customKey = NO_KEY;
          settingmachine = 0;
          machine = "";
          display.resetDisplay();
        }
      }
    }
    //------------------------------- downtime code change to activity
    else if (settingmachine == 2) {
      if (interruptWork) {
        portENTER_CRITICAL_ISR(&timerMux);
        interruptWork = 0;
        portEXIT_CRITICAL_ISR(&timerMux);
        workCounter++;
        Serial.print("Work2: ");
        Serial.print(translate_hh_mm_cc(workCounter));
        Serial.print(" u2: ");
        Serial.print(u);
        Serial.print(" Key2: ");
        Serial.println(customKeyset);
        dw_font_goto(&myfont, 80, LINE1);
        sprintf(buff3, "%s", translate_hh_mm_cc(workCounter));
        dw_font_print(&myfont, buff3);
      }
      sprintf(buff1, "mc/no. %s ", readmachine);
      dw_font_goto(&myfont, 0, LINE1);
      dw_font_print(&myfont, buff1);
      dw_font_goto(&myfont, 4, LINE2);
      dw_font_print(&myfont, "โปรดใส่รหัสการทำงาน");
      display.drawString(5, 28, "-> _ _ _");
      display.display();
      /*
        //u
        1  18  +9
        2 27
        3 36
        4 45
      */
      if (u == 45) {
        print_cf_cc_5line4();
      } else if (u >= 46) {
        dw_font_goto(&myfont, 0, LINE3);
        dw_font_print(&myfont, "เกิดข้อผิดพลาด");
        display.display();
        delay(3000);
        display.resetDisplay();
        code = "";
        customKeyset = "";
        u = 18;
      }
      customKey = customKeypad.getKey();
      if (customKey != NO_KEY) {
        if (customKey != NO_KEY && customKey != '*' && customKey != '#') {
          code += customKey;
          customKeyset = String(customKey);
          display.drawString(u, 28, customKeyset);
          u += 9;
          display.display();
          customKey = NO_KEY;
        } else if (customKey == '*') {
          settingmenu = 1;
          customKey = NO_KEY;
          settingmachine = 0;
          Serial.print("Technician code: ");
          Serial.println(code);  // technician code
                                 //09nov2022 - tech: change dt to activity
          dst.activity_type = "3";
          dst.code_downtime = (char*)code.c_str();
          star_line2();
          Serial.print("dst.id_task                 : ");
          Serial.println(dst.id_task);
          Serial.print("(char*)readmachine.c_str()  : ");
          Serial.println((char*)readmachine.c_str());
          Serial.print("dst.id_staff                : ");
          Serial.println(dst.id_staff);
          Serial.print("dst.activity_type           : ");
          Serial.println(dst.activity_type);
          Serial.print("dst.code_downtime           : ");
          Serial.println(dst.code_downtime);
          query_Downtime_GetMethod((char*)IDcard.c_str(), dst.id_job, dst.operation, (char*)readmachine.c_str(), (char*)code.c_str());
          star_line2();
          u = 18;
          code = "";
          display.resetDisplay();
        } else if (customKey == '#') {
          display.resetDisplay();
          u = 18;
          settingmenu = 1;
          customKey = NO_KEY;
          settingmachine = 0;
          code = "";
        }
      }
    }
    //---------------------------------------------------- quit code
    else if (settingmachine == 3) {
      dw_font_goto(&myfont, 15, LINE3);
      dw_font_print(&myfont, "สแกนบัตรออกการทำงาน");
      display.display();
      if (rdm6300.update()) {
        tem1 = String(rdm6300.get_tag_id());
        if (strlen((const char*)tem1.c_str()) == 8)
          IDcard2 = String("00") + tem1;
        else if (strlen((const char*)tem1.c_str()) == 7)
          IDcard2 = String("000") + tem1;

        if (IDcard2 == IDcard) {
          //09nov2022
          dst.activity_type = "3";
          Serial.println("dst.id_task        : " + String(dst.id_task));
          Serial.println("readmachine.c_str(): " + String(readmachine.c_str()));
          Serial.println("dst.id_staff       : " + String(dst.id_staff));
          Serial.print("dst.activity_type    : ");
          Serial.println(dst.activity_type);
          Serial.print("dst.id_activity      : ");
          Serial.println(dst.id_activity);
          query_Quit_DT_GetMethod((char*)IDcard2.c_str(), dst.id_job, dst.operation, (char*)readmachine.c_str());
          display.clear();
          dw_font_goto(&myfont, 30, LINE3);
          dw_font_print(&myfont, "ออกการทำงาน");
          dw_font_goto(&myfont, 40, LINE4);
          sprintf(buff2, "%s", translate_hh_mm_cc(workCounter));
          dw_font_print(&myfont, buff2);
          display.display();

          delay(2000);

          RFstate = 0;
          settingmenu = 0;
          settingmachine = 0;
          confirmRF = 0;
          timerDetachInterrupt(timer);
          workCounter = 0;
          IDcard2 = "";
          tem1 = "";
          rdm6300.update();
          while (!rdm6300.update()) {
            break;
          }
        } else {
          display.resetDisplay();
          dw_font_goto(&myfont, 15, LINE3);
          dw_font_print(&myfont, "ID card ไม่ตรง 3");
          IDcard2 = "";
          tem1 = "";
          settingmachine = 3;
          display.display();
          delay(3000);
          display.resetDisplay();
          rdm6300.update();
          while (!rdm6300.update()) {
            break;
          }
        }
      }
    }
  }
}
String httpGETRequest(const char* serverName) {
  HTTPClient http;
  // Your IP address with path or Domain name with URL path
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "--";
  if (httpResponseCode > 0) {
    star_line1();
    Serial.print("HTTP GET Response code: ");
    Serial.println(httpResponseCode);
    star_line1();
    payload = http.getString();
    //19feb
    err_msg = httpResponseCode;
  }
  //else {
  //Serial.print("Error code: ");
  //Serial.println(httpResponseCode);
  //}
  // Free resources
  http.end();
  return payload;
}
String httpPOSTRequest(const char* serverName , const char* msg) {
  HTTPClient http;
  // Your IP address with path or Domain name with URL path
  http.begin(serverName);
  //  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Send HTTP POST request
  int httpResponseCode = http.POST(msg);
  String payload = "--";
  if (httpResponseCode > 0) {
    star_line1();
    Serial.print("HTTP POST Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  //else {
  //  Serial.print("Error code: ");
  //  Serial.println(httpResponseCode);
  //}
  // Free resources
  http.end();
  return payload;
}
int query_Touch_GetMethod(const char * id_mc, const char * id_rfid, employ_touch_TYPE * result) {
  String msg = " ";
  //char buff[300];
  // 6 sep 2021
  //26sep2022
  /*
#ifdef URL_BUNNAM
  sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/touch_v3.php?id_mc=%s&id_rfid=%s", id_mc, id_rfid );
#else
  sprintf( buff , "http://192.168.40.21/majorette_pp/update/touch_v3.php?id_mc=%s&id_rfid=%s", id_mc, id_rfid );
#endif
*/
  Serial.print("Send URL: "); Serial.println(sendURL);
  star_line2();
  if(sendURL==0)
    sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/touch_v3.php?id_mc=%s&id_rfid=%s", id_mc, id_rfid );
  else
    sprintf( buff , "http://192.168.40.21/majorette_pp/update/touch_v3.php?id_mc=%s&id_rfid=%s", id_mc, id_rfid );
  Serial.println(buff);
  msg = httpGETRequest(buff);
  if ( msg != "null" ) {
    //star_line1();
    Serial.print("msg: ");Serial.println( msg );
    //star_line1();
    Serial.print("msg substring  2-6: ");  Serial.println(msg.substring(2, 6));
    Serial.print("msg substring 9-12: ");  Serial.println(msg.substring(9, 12));
    star_line1();
    int tt_id = msg.indexOf("ID:");
    if (msg.substring(2, 6) == "code") {
      star_line2();
/*      
      if(msg.substring(9, 12) == "026") {
        display.clear();
        dw_font_goto(&myfont, 40, LINE1);
        sprintf( buff1 ,"Error:026");
        dw_font_print(&myfont, buff1);
        dw_font_goto(&myfont, 0, LINE2);
        sprintf( buff2 ,"This machine is currently");
        dw_font_print(&myfont, buff2);
        display.display();
        dw_font_goto(&myfont, 15, LINE3);
        sprintf( buff3 ,"in occupied by ID: ");
        dw_font_print(&myfont, buff3);
        display.display();
        String tt_msg= "";
        tt_msg+= msg[tt_id+4];
        tt_msg+= msg[tt_id+5];
        tt_msg+= msg[tt_id+6];
        tt_msg+= msg[tt_id+7];
        tt_msg+= msg[tt_id+8];
        tt_msg+= msg[tt_id+9];
        Serial.println("========"+tt_msg);
        //sprintf( buff4 ," "+tt_msg);
        //dw_font_goto(&myfont, 45, LINE3);
        display.drawString(48, 45, tt_msg);
        dw_font_print(&myfont, buff4);
        display.display();
        delay(3000);
        return -2;  
        
      }
*/
//      else 
      {
        dw_font_goto(&myfont, 5, LINE4);
        sprintf( buff1 , "Err code: %s ", msg.substring(9, 12));
        dw_font_print(&myfont, buff1);
        display.display();
        delay(1000);
        return -2;
      }
    }
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println(" : ID is not found !");
      result->flag_err = 0;
     
      return -1; // has error, 301, etc
    }
    else if (doc["code"]) {
      dw_font_goto(&myfont, 0 , LINE4);
      sprintf( buff1 , "%s ", error);
      dw_font_print(&myfont, buff2);
      return ((doc["code"]).as<int>());
    }
    else {
      result->id_staff = strdup(doc["id_staff"]);     // "000002"
      result->name_first = strdup(doc["name_first"]); // "Thanasin"
      result->name_last = strdup(doc["name_last"]);   // "Bunnam"
      result->role = doc["role"];                     // 1
      result->id_task = strdup(doc["id_task"]);       // "2395"
      result->id_job = strdup(doc["id_job"]);         // "6358802"
      result->item_no = strdup(doc["item_no"]);       // "Z00218C70101"
      result->operation = strdup(doc["operation"]);   // "542"
      //12 oct
      result->op_color = strdup(doc["op_color"]);     // "2"
      result->op_side = strdup(doc["op_side"]);       // "A"
      //
      result->op_name = strdup(doc["op_name"]);       // "BONNET 2C.AX2"
      result->qty_order = strdup(doc["qty_order"]);   // "10390"
      result->qty_comp = strdup(doc["qty_comp"]);     // "0"
      result->qty_open = strdup(doc["qty_open"]);     // "10390"
      result->flag_err = 0 ;
      //12 oct
      result->multiplier = strdup(doc["multiplier"]); // "1"
      //26 oct
      result->qty_per_tray = strdup(doc["qty_per_tray"]);       // "100"
      result->qty_shif_pulse1 = doc["qty_shif_pulse1"]; // "0"
      result->qty_shif_pulse2 = doc["qty_shif_pulse2"]; // "500"
      result->qty_shif_pulse3 = doc["qty_shif_pulse3"]; // "133"
      return 0;
    }
  }
  else {
    result->flag_err = 1;
    Serial.println("Error!");
    return -2;
  }
}
int query_Break_GetMethod(char * id_activiry, char * activity_type, char * id_rfid, char * id_mc, char * break_code, employ_touch_TYPE * result) {
  String msg = " ";
/*
  Serial.println();
  Serial.println("=============================="); 
  Serial.println("break_v3"); 
  Serial.println("------------------------------");
  Serial.println("dst.id_activity    : "+String(dst.id_activity));
  Serial.print(  "dst.activity_type  : ");  Serial.println(dst.activity_type);
  Serial.println("id_rfid            : "+String(id_rfid));
  Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
  Serial.println("break_code         : "+String(break_code));
  Serial.println("==============================");  
*/
  Serial.println();
  star_line2();
if(sendURL==0)
  sprintf( buff, "https://bunnam.com/projects/majorette_pp/update/break_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&break_code=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, break_code);
else
  sprintf( buff, "http://192.168.40.21/majorette_pp/update/break_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&break_code=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, break_code);
/*  
#ifdef URL_BUNNAM
  sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/break_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&break_code=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, break_code);
#else
  sprintf( buff , "http://192.168.40.21/majorette_pp/update/break_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&break_code=%s", dst.id_activity, dst.activity_type, id_rfid, id_mc, break_code);
#endif
*/
  //Serial.println();
  Serial.println(buff);
  msg = httpGETRequest(buff);
  if ( msg != "null" ) {
    //star_line1();
    //Serial.println("msg break==============================");
    Serial.print("msg: "); Serial.println( msg );
    //Serial.println( msg.length() );
    star_line1();
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    result->id_break = strdup(doc["id_break"]); // id_break
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("Error Break!");
      return -1;
    }
    if (doc["code"]) {
      return ((doc["code"]).as<int>());
    }
  }
  else {
      return 0;
  }
}
int query_Quit_GetMethod( char* id_activity, char * activity_type, char* id_rfid , char* id_mc , char* no_send , char* no_pulse1 , char* no_pulse2 ,char* no_pulse3 ){
  String msg = " ";
  char buff[300];
  //13sep2022
/*  
  Serial.println("==============================");
  Serial.println("id_activity   : "+String(id_activity));
  Serial.print("activity_type : ");  Serial.println(activity_type);
  Serial.println("id_rfid       : "+String(id_rfid));
  Serial.println("id_mc         : "+String(id_mc));
  Serial.println("no_send       : "+String(no_send));
  Serial.println("no_pulse1     : "+String(no_pulse1));
  Serial.println("no_pulse2     : "+String(no_pulse2));
  Serial.println("no_pulse3     : "+String(no_pulse3));
  Serial.println("==============================");
*/  
  star_line2();
if(sendURL==0)
  sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/quit_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=0&no_pulse2=0&no_pulse3=0", id_activity, activity_type, id_rfid, id_mc, no_send);
else
  sprintf(buff, "http://192.168.40.21/majorette_pp/update/quit_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=0&no_pulse2=0&no_pulse3=0", id_activity, activity_type, id_rfid, id_mc, no_send);
/*  
#ifdef URL_BUNNAM
  sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/quit_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=0&no_pulse2=0&no_pulse3=0", id_activity, activity_type, id_rfid, id_mc, no_send);
#else
  sprintf(buff, "http://192.168.40.21/majorette_pp/update/quit_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=0&no_pulse2=0&no_pulse3=0", id_activity, activity_type, id_rfid, id_mc, no_send);
#endif
*/
  Serial.println(buff);
  msg = httpGETRequest(buff);
  Serial.print("msg: "); Serial.println( msg );
  star_line1();
  Serial.println("\n\n\nOperation Stop<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n\n");
  if ( msg != "null" ) {
    //Serial.println( msg );
    //Serial.println( msg.length() );
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error)    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("Error Quit!");
      return -1;
    }
    if (doc["code"]) {
      return ((doc["code"]).as<int>());
    }
    if ( doc["time_work"] ) {
      Serial.println((const char *)(doc["time_work"]));
      return 0;
    }
  }
  else {
    Serial.println("Error!");
    return -2;
  }
}
int query_Downtime_GetMethod(  char* id_rfid , char * id_job , char * operation , char * id_machine , char * code_downtime ){
  String msg = " ";
  char buff[300];
  if(sendURL==0)
    sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/downtime.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&code_downtime=%s" , id_rfid, id_job, operation, id_machine, code_downtime );
  else
    sprintf(buff, "http://192.168.40.21/majorette_pp/update/downtime.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&code_downtime=%s" , id_rfid, id_job, operation, id_machine, code_downtime );
/*  
#ifdef URL_BUNNAM
  sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/downtime.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&code_downtime=%s" , id_rfid, id_job, operation, id_machine, code_downtime );
#else
  sprintf( buff , "http://192.168.40.21/majorette_pp/update/downtime.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s&code_downtime=%s" , id_rfid, id_job, operation, id_machine, code_downtime );
#endif
*/
  Serial.println(buff);
  msg = httpGETRequest(buff);
  if ( msg != "null" ) {
    Serial.println( msg );
    Serial.println( msg.length() );
    if ( msg == "OK" ) {
      return 0;
    }
    else {
      DynamicJsonDocument  doc( msg.length() + 256 ) ;
      DeserializationError error = deserializeJson(doc, msg);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        Serial.println("Error Break!");
        return -1;
      }
      if (doc["code"]) {
        return ((doc["code"]).as<int>());
      }
    }
  }
  else  {
    Serial.println("Error!");
    return -2;
  }
}
int query_Quit_DT_GetMethod( char * id_rfid, char * id_job , char * operation , char * id_mc ) {
  String msg = " ";
  char buff[300];
  //22feb
  if (old_mc_no == 1) {
    id_mc = "02-00";
    old_mc_no = 0;
    Serial.print("### id_mc= ");
    Serial.println(id_mc);
  }
if(sendURL==0)
  sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/quit_dt.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s", id_rfid, id_job, operation, id_mc );
else
  sprintf(buff, "http://192.168.40.21/majorette_pp/update/quit_dt.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s", id_rfid, id_job, operation, id_mc );
/*  
#ifdef URL_BUNNAM
  sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/quit_dt.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s" , id_rfid, id_job, operation, id_mc );
#else
  sprintf( buff , "http://192.168.40.21/majorette_pp/update/quit_dt.php?id_rfid=%s&id_job=%s&operation=%s&id_mc=%s" , id_rfid, id_job, operation, id_mc );
#endif
*/
  Serial.println(buff);
  msg = httpGETRequest(buff);
  if ( msg != "null" ) {
    Serial.println( msg );
    Serial.println( msg.length() );
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("Error Quit!");
      return -1;
    }
    if (doc["code"])
    {
      return ((doc["code"]).as<int>());
    }
    if ( doc["time_work"] )
    {
      Serial.println((const char *)(doc["time_work"]));
      return 0;
    }
  }
  else {
    Serial.println("Error!");
    return -2;
  }
}
void draw_pixel(int16_t x, int16_t y) {
  display.setColor(WHITE);
  display.setPixel(x, y);
}
void clear_pixel(int16_t x, int16_t y) {
  display.setColor(BLACK);
  display.setPixel(x, y);
}
//------------------ 13/11/64
int query_Activity_GetMethod(char * id_task, char * id_mc, char * id_staff, char * activity_type, char * code_downtime, employ_touch_TYPE * result) {
  String msg= " ";
  char buff[300];
  //star_line2();
if(sendURL==0)
  sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/activity_v3.php?id_task=%s&id_mc=%s&id_staff=%s&activity_type=%s&code_downtime=%s", id_task, id_mc, id_staff, activity_type, code_downtime);
else
  sprintf(buff, "http://192.168.40.21/majorette_pp/update/activity_v3.php?id_task=%s&id_mc=%s&id_staff=%s&activity_type=%s&code_downtime=%s", id_task, id_mc, id_staff, activity_type, code_downtime);
 
 Serial.println(buff);
  msg= httpGETRequest(buff);
  if(msg != "null") {  
    Serial.print("msg: "); Serial.println(msg); 
    //Serial.println( msg.length() );
    star_line1();
    DynamicJsonDocument  doc( msg.length() + 256 ) ;
    DeserializationError error = deserializeJson(doc, msg);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      Serial.println("Error Activity!");
      return -1;
    }
    else if (doc["code"]) {
      return ((doc["code"]).as<int>());
    }
    else {
      result->id_activity = strdup(doc["id_activity"]); // id_activity
      return 0;
    }
  }
}
int query_quit_dt_op_GetMethod(char* id_activity, char * activity_type, char* id_rfid, char* id_machine , char  * no_send , char * no_pulse1 , char * no_pulse2 , char * no_pulse3 , char * multiplier, char * code_downtime) {
  String msg= " ";
  char buff[300];
  if(sendURL==0)
  sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/quit_dt_op_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=%s&no_pulse2=%s&no_pulse3=%s&multiplier=%s&code_downtime=%s", id_activity, activity_type, id_rfid, id_machine, no_send, no_pulse1, no_pulse2, no_pulse3, multiplier, code_downtime);
else
  sprintf(buff, "http://192.168.40.21/majorette_pp/update/quit_dt_op_v3.php?id_activity=%s&activity_type=%s&id_rfid=%s&id_mc=%s&no_send=%s&no_pulse1=%s&no_pulse2=%s&no_pulse3=%s&multiplier=%s&code_downtime=%s", id_activity, activity_type, id_rfid, id_machine, no_send, no_pulse1, no_pulse2, no_pulse3, multiplier, code_downtime);

 Serial.println(buff);
  msg = httpGETRequest(buff);

  if(msg != "null") {
    Serial.println( msg );
    Serial.println( msg.length() );
    if ( msg == "OK" )
    {
      return 0;
    }
    else
    {
      DynamicJsonDocument  doc( msg.length() + 256 ) ;
      DeserializationError error = deserializeJson(doc, msg);
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        Serial.println("Error Quit Downtime!");
        return -1;
      }
      if (doc["code"])
      {
        return ((doc["code"]).as<int>());
      }
    }
  }
  else
  {
    Serial.println("Error!");
    return -2;
  }
}
void star_line1(void) {
  Serial.println("--------------------");
}
void star_line2(void) {
  Serial.println("====================");
}
void prog_qty_2c(void) {
  //if(result==1)  Serial.print("--------"); // ปั๊ม 1
  //if(result==0)  Serial.print("========"); // ปั๊ม 2
  //Serial.print("+");
  if (result == 1) { // ปั๊ม 1
    if (flag1 == 1) {             // count when flag1==1 only
      Serial.println("Qty: " + String(++dataqty));
      Serial.println("Qty: " + String(++Qty_MC));

      //pp_log_display();
      flag1++;                    // inc for greater than 1
    }
  }
  else if (result == 0) { // ปั๊ม 2
    if (flag1 == 1) {
      Serial.println("Qty: " + String(++dataqty));
      Serial.println("Qty: " + String(++Qty_MC));
      //pp_log_display();
      flag1++;
      //display.clear();
    }
  }
  if (result == 2) flag1 = 0; // 2=> จุ่มสี = clear flag1
}
void prog_qty_1c(void) {
  //if(result==1)  Serial.print("--------"); // ปั๊ม 1
  if (result == 1) { // ปั๊ม 1
    //pp_log_display();
    if (flag1 == 1) {
      Serial.println("Qty: " + String(++dataqty));
      Serial.println("Qty: " + String(++Qty_MC));
      flag1++;
      //display.clear();
    }
  }
}
//dec 21
/*22nov2022
void pp_log_display() {
  //EEPROM.get(addr_ee_qty, readqty);
  //str_log= String(f_multiplier) + "|" + String(readqty);
  str_log = String(f_multiplier) + "|" + String(dataqty);
  Serial.println();
  Serial.println("str_log: " + String(str_log));
  //sprintf( buff4 , "http://192.168.40.21/majorette_pp/update/log.php?log=%s", str_log);
  sprintf( buff4 , "https://bunnam.com/projects/sroysawan/update/log.php?log=%s", str_log);
  Serial.println("buffer4: " + String(buff4));
  msg = httpPOSTRequest(buff4, "");
  //Serial.println("msg pp_log====================");
  //Serial.println( msg );
  //Serial.println("msg pp_log====================");
}
*/
//void count_sender (void*) {
// void count_sender (void) {
//   //while (true) 
//   {
//     //vTaskDelay(1 / portTICK_PERIOD_MS);
//     //if (IDcard != "")
//       //if ( millis() - last_time_get > period_get) 
//       {
//         //last_time_get = millis();
//         if (datacount != count) {
//           int chack = EEPROM.read(addr_chk_ee);
//           //Serial.println(chack);
//           if (chack == 1) {
//             EEPROM.get(addr_ee_qty, readqty);
//             EEPROM.get(addr_ee_count, readcount);
//             Serial.print("Upload count:");
//             Serial.println(readcount);
//             datacount = readcount + 1;
//             dataqty = readqty;
//             EEPROM.put(addr_ee_qty, dataqty);
//             EEPROM.put(addr_ee_count , datacount);
//             EEPROM.commit();
//           }
//         }
//         count++;
//         EEPROM.get(addr_ee_count, readcount);
//         EEPROM.get(addr_ee_qty, readqty);
// /*        
//         star_line2();
//         Serial.println("count_v3 : white sw"); 
//         star_line1();
//         Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
//         Serial.println("dst.id_staff       : "+String(dst.id_staff));
//         Serial.println("dst.id_activity    : "+String(dst.id_activity));
//         Serial.print(  "dst.activity_type  : ");  Serial.println(dst.activity_type);
//         Serial.println("no_send/readcount  : "+String(readcount));
//         Serial.println("no_pulse1/readqty  : "+String(readqty));
//         Serial.println("no_pulse2/white_sw : 1");
//         Serial.println("==============================");
// */        
//         display.clear();
//         dw_font_goto(&myfont, 10, LINE2);
//         //sprintf( buff1 , "ส่งค่า 1 ถาด : %d", 80);
//         sprintf( buff1 , "ส่งค่า 1 ถาด : %s", dst.qty_per_tray);
//         digitalWrite(white_led, LOW);
//         Serial.print("qty_per_tray: ");
//         Serial.println(dst.qty_per_tray);
//         dw_font_print(&myfont, buff1);
//        // display.drawString(5, 28, "-> _ _ - _ _");
//         display.display();

//         if (sendURL == 0)
//           sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=1&no_pulse3=0&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty);
//         else
//           //sprintf( buff , "http://192.168.40.21/majorette_pp/update/count.php?id_task=%s&id_mc=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=0", dst.id_task, readmachine.c_str() , readcount, readqty);
//           sprintf( buff , "http://192.168.40.21/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=1&no_pulse3=0&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty);

//         Serial.println( buff );
//         WebSerial.print(buff);
//         msg = httpPOSTRequest(buff, "");  
//         star_line2();
//         ///Serial.println("=============================="); 
//         Serial.println( msg );
//         WebSerial.print(msg);
// // oled print / white sw
//         dw_font_goto(&myfont, 10, LINE3);
//         sprintf( buff1 , "done.");
        
//         dw_font_print(&myfont, buff1);
//        // display.drawString(5, 28, "-> _ _ - _ _");
//         display.display();
        
//         delay(1000);       
//         digitalWrite(white_led, HIGH);     
//     }
//   }
// }
// int read_no_pulse3(){
//   char customKey= NO_KEY;
//   String no_pulse3= "", tt="";
//   digitalWrite(yellow_led, LOW);
//   display.clear();
//   dw_font_goto(&myfont, 10, LINE1);
//   sprintf( buff1 , "กรอกจำนวนชิ้นงาน: ");
//   dw_font_print(&myfont, buff1);
    
//   dw_font_goto(&myfont, 5, LINE4);
//   dw_font_print(&myfont, "* ย้อนกลับ              ยืนยัน #");
//   display.display();
  
//   //Serial.println();
//   //Serial.println("===============into read no_pulse3");
//   while(customKey != '*' && customKey != '#'){
//     customKey = customKeypad.getKey();
//     if(customKey=='*'){
//       display.clear();
//       return 0;
//     }
//     if(customKey=='#'){
      
//       dw_font_goto(&myfont, 10, LINE2);
//       sprintf( buff1 , "upload %s...", no_pulse3);
//       dw_font_print(&myfont, buff1);
//       display.display();
// /*
//       Serial.println("======================= submit");
//       Serial.print("no_pulse3: ");
//       Serial.println(no_pulse3);
//       Serial.println("=============================="); 
//       Serial.println("count_v3 : yellow sw"); 
//       Serial.println("------------------------------");
//       Serial.println("readmachine.c_str(): "+String(readmachine.c_str()));
//       Serial.println("dst.id_staff       : "+String(dst.id_staff));
//       Serial.println("dst.id_activity    : "+String(dst.id_activity));
//       Serial.print(  "dst.activity_type  : ");  Serial.println(dst.activity_type);
//       Serial.println("no_send/readcount  : "+String(readcount));
//       Serial.println("no_pulse1/readqty  : "+String(readqty));
//       Serial.println("no_pulse2/white_sw : "+String(0));
//       Serial.println("no_pulse3          : "+String(no_pulse3));
//       Serial.println("==============================");  
// */
// if(sendURL==0)
//   sprintf(buff, "https://bunnam.com/projects/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=%s&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty, no_pulse3);
// else
//   sprintf(buff, "http://192.168.40.21/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=%s&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty, no_pulse3);
// /*
// #ifdef URL_BUNNAM
//   sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=%s&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty, no_pulse3);
// #else
//   sprintf( buff , "http://192.168.40.21/majorette_pp/update/count_v3.php?id_mc=%s&id_staff=%s&id_activity=%s&activity_type=%s&no_send=%d&no_pulse1=%d&no_pulse2=0&no_pulse3=%s&multiplier=0", readmachine.c_str(), dst.id_staff, dst.id_activity, dst.activity_type, readcount, readqty, no_pulse3);
// #endif
// */
//       Serial.println( buff );
//       msg = httpPOSTRequest(buff, "");   
//       star_line2();
//       Serial.println( msg ); 
//       dw_font_goto(&myfont, 70, LINE2);
//       sprintf( buff1 , "done.");
      
//       dw_font_print(&myfont, buff1);
//       display.display();
//       delay(2000);
//       display.clear();
//       digitalWrite(yellow_led, HIGH);
//       return 0;
//     }  
//     else if(customKey != NO_KEY){
//       no_pulse3 += customKey;
//       if(tt!=no_pulse3) { 
//         display.clear();
//         dw_font_goto(&myfont, 10, LINE1);
//         sprintf( buff1 , "กรอกจำนวนชิ้นงาน %s", no_pulse3);
//         dw_font_print(&myfont, buff1);
//         display.display();
//         dw_font_goto(&myfont, 5, LINE4);
//         dw_font_print(&myfont, "* ย้อนกลับ              ยืนยัน #");
//         display.display();
//         Serial.print("no_pulse3: "); Serial.println(no_pulse3); 
//         tt= no_pulse3;
//       }     
//       customKey= NO_KEY;     
//     }
//   }
// }
void print_cf_cc_5line4() {
  dw_font_goto(&myfont, 5, LINE4);
  dw_font_print(&myfont, "* ยืนยัน              ย้อนกลับ #");
}
void easter_eggs() {
//------------ easter_eggs()
      customKey1 = customKeypad.getKey();
      //customKey3 = customKeypad.getKey();
      if (customKey1 != NO_KEY) {
        Serial.print("customKey1 : ");
        Serial.print(customKey1);
        Serial.print(" + Count : ");
        Serial.println(counter_);

        if (customKey1 == 'A' && !flag_count_down) {
          Serial.println("A");
          flag_count_down = 1;
          counter_++;
          //counter__++;
        }
        if (customKey1 == 'C' && counter_ == 1) {
          Serial.println("C");
          counter_++;
          //counter__++;
        }
        if (customKey1 == 'A' && counter_ == 2) {
          Serial.println("A");
          counter_++;
          Serial.println("A + C + A !!");
          Serial.println("Success !");
        
          WiFi.disconnect();
          EEPROM.begin(EEPROM_SIZE);
          EEPROM.put(addr_ssid , 0);
          EEPROM.put(addr_pass, 0);
          EEPROM.put(addr_ee_count, 0);
          readmachine = "02-00";
          old_mc_no = 1;
          EEPROM.put(addr_mc_no, readmachine);      // Fill machine no.
          EEPROM.commit();
          
          con = 0;
          //time out
          time_out = 0x05FF;
          flag_count_down = 0;
          counter_ = 0;
          display.clear();
          dw_font_goto(&myfont, 5, 36);
          dw_font_print(&myfont, "โปรดเลือก A เพื่อเลือก WiFi");
          display.display();
        }
        if (customKey1 == 'B' && counter_ == 2) {
          Serial.println("B");
          counter_++;
          sendURL= !sendURL;
          Serial.println("A + C + B !!");
          Serial.print("sendURL: ");
          Serial.print(sendURL);
          EEPROM.put(send_url , sendURL);
          EEPROM.commit();
          Serial.print(" - switch to ");
          if(sendURL==0)
            Serial.println("Public URL.");
          else
            Serial.println("Local URL.");
          con = 0;
          //time out
          time_out = 0x05FF;
          flag_count_down = 0;
          counter_ = 0;
          delay(2000);
        }
      }
      else {
        if (flag_count_down) {
          if (time_out < 100) {
            Serial.println("Time Out !");
            time_out = 0x05FF;
            flag_count_down = 0;
            counter_ = 0;
          }
          time_out--;
        }
      }
}      
//----------- easter_eggs()
void log_mac_address_when_start(int x) {
  String str_log;
  char buff[300];
  str_log= WiFi.macAddress();
  sprintf( buff , "https://bunnam.com/projects/majorette_pp/update/log.php?log=mc_no=%s,mac=%s,%d", readmachine.c_str(),str_log.c_str(),x);
  Serial.print("buffer= ");  Serial.println(buff);
  msg = httpPOSTRequest(buff, "");
  Serial.print("msg= "); Serial.println( msg );
}