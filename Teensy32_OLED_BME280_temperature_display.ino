/* Teensy CAN-Bus with OLED 128x64 demo
 *  
 * www.skpang.co.uk
 * 
 * V1.0 Aug 2018
 *  
 * For use with Teensy CAN-Bus Breakout board:
 * http://skpang.co.uk/catalog/teensy-canbus-breakout-board-include-teensy-32-p-1507.html
 * requires OLED display
 * http://skpang.co.uk/catalog/oled-128x64-display-for-teensy-breakout-board-p-1508.html
 * 
 * *  
 */
#include <FlexCAN.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeSansBold18pt7b.h>

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

static CAN_message_t msg,rxmsg;
volatile uint32_t count = 0;
IntervalTimer data_timer;
String CANStr(""); 
volatile uint32_t can_msg_count = 0;
float converted_data;
uint16_t  CAN_ID =  0x700;
unsigned char data[4];

uint8_t no_data1 = 0;   // No data counter for channel 1
uint8_t no_data2 = 0;   // No data counter for channel 2


void setup(){
  
  Can0.begin(500000); //Set CAN speed to 500kbps
  
  display.begin(SSD1306_SWITCHCAPVCC);
  // Clear the buffer.
  display.clearDisplay();

  Serial.println(F("CAN Bus test"));

  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0,15);
  display.println(" Teensy CAN-Bus demo");
  display.println(" ");
  display.println(" skpang.co.uk");
  display.println(" ");
  display.println(" 08/18");
  display.display();

  delay(1000);
  
  display.clearDisplay();

  data_timer.begin(data_count, 1000000);   // Start no data interrupt counter at 1s interval
 
}

/* From Timer Interrupt */
void data_count(void)
{
  no_data1++;
  no_data2++;
}

void loop() {
  int i;
  
  while(Can0.read(rxmsg)) //Read data from CAN-bus
  { 
     String CANStr(""); 
     for (int i=0; i < 8; i++) {     

         CANStr += String(rxmsg.buf[i],HEX);
         CANStr += (" ") ;
     }
     
     if(rxmsg.id == CAN_ID)
     {
      data[0] = rxmsg.buf[0];
      data[1] = rxmsg.buf[1];
      data[2] = rxmsg.buf[2];
      data[3] = rxmsg.buf[3];
      
      memcpy(&converted_data, data, 4);   //Convert data back to float
      
      display.setFont(&FreeSansBold18pt7b); 
      display.fillRect(0,0,120, 30,BLACK);  //Erase old value
      display.setCursor(0,25);
      display.print(converted_data,2);      //Display temperature with 2 decimal places
      display.print(" C");
      display.display();

      Serial.print(rxmsg.id,HEX); 
      Serial.print(' '); 
      Serial.print(rxmsg.len,HEX); 
      Serial.print(' ');
      Serial.print(CANStr); 

      Serial.print("Ch1 Temperature : ");
      Serial.println(converted_data,4);
      
      no_data1 = 0;  //Clear no data counter
    }

    if(rxmsg.id == CAN_ID + 1)  // Indoor temperature
    {
      data[0] = rxmsg.buf[0];
      data[1] = rxmsg.buf[1];
      data[2] = rxmsg.buf[2];
      data[3] = rxmsg.buf[3];

      memcpy(&converted_data, data, 4);     //Convert data back to float
      display.setFont(&FreeSansBold18pt7b);
      display.fillRect(0,30,120, 60,BLACK); //Erase old value
      display.setCursor(0,60);
      display.print(converted_data,2);       //Display temperature with 2 decimal places
      display.print(" %");
      display.display();
      
      Serial.print(rxmsg.id,HEX); 
      Serial.print(' '); 
      Serial.print(rxmsg.len,HEX); 
      Serial.print(' ');
      Serial.print(CANStr); 

      Serial.print("Ch2 Temperature : ");
      Serial.println(converted_data,4);
      
      no_data2 = 0;   //Clear no data counter
    }

     
  }
   if(no_data1 >2)  //Check data still coming in within 2 second
   {
      display.setFont(&FreeSansBold18pt7b);
      display.fillRect(0,0,128, 60,BLACK);
      display.setCursor(0,25);
      display.print("------");
      display.display();
      no_data1 = 3;  // Prevent counter rollover
    }
   
   if(no_data2 >2)  //Check data still coming in within 2 second
   {
      display.setFont(&FreeSansBold18pt7b);
      display.fillRect(0,30,128, 60,BLACK);
      display.setCursor(0,60);
      display.print("------");
      display.display();
      no_data2 = 3;  // Prevent counter rollover
    }
}



