// RFID Code with proper documentation


//===============================================================================
//  Header Files
//===============================================================================

//===============================================================================
//  Constants
//===============================================================================
// Here is where we save valid tags. I will first show the invalid tag. Then I will comment this 
// line and uncomment the valid tag. This shows you how to manually set your own
// valid tag.

// Bogus tag
char tag1[10] = {'X','X','X','X','X','X','X','X','X','X'};

// Good tag UNCOMMENT THIS TAG WHILE USE
//char tag1[10] = {'3','6','0','0','6','6','0','0','5','C'};

//===============================================================================
//  Variables
//===============================================================================
char ourTag[10];        // We will use this to hold the interrogated tag's data.
boolean tagDetected;    // We can use this to continue program action if our
                         // reading seems like a real tag was detected.
//===============================================================================
//  Pin Declarations
//===============================================================================
//Inputs:
// Serial1 (pin 19) will be used to grab serial data from buffer which is given
// by the ID12LA

//Outputs:
// Serial (pin 2) will be used to output serial data as user feedback to see
// what is going on

//===============================================================================
//  Initialization
//===============================================================================
void setup() 
{
  // Initialize serial port speed for the serial terminal.
  // We will use Serial1 on our MEGA for RFID data and the Serial for messages
  // to the terminal.
  Serial.begin(9600);
  Serial1.begin(9600);

  // Initialize data flags
  tagDetected = false;
}

//===============================================================================
//  Main
//===============================================================================
void loop() 
{
  // This function call will return the interrogated tag's data in form of a char
  // array. If there was an error in the integrity of the transmission, it will
  // return "0000000000". Of course, this only happens unless there is something
  // in the serial buffer. All the while, we also
  // set a flag if there was data there. This comes into use later on.
  if (Serial1.available() > 0)
  {
    // Give some time for all data to arrive safe and sound into the buffer.
    delay(250);
    
    // The if statement below ensures that the beginning of a tag is seen.
    // Remember that a Start of Text is the decimal value of 2. If we do not
    // see this, all bets are off in even continuing to look further into the
    // buffer. I use peek simply because I don't like touching data until I
    // decide to process it.
    if (Serial1.peek() != 2)
    {
      // Log that we did not have a true tag detected.
      tagDetected = false;
      
      // Flush the buffer to bring it back to an initial, known state.
      flushSerial1Buffer();
    }  
    else
    {
      // Looks like the serial data starts with a valid Start of Text character,
      // let mark that we detected a potential tag.
      tagDetected = true;
     
      // Go and process the tag in the serial1 buffer.
      fetchTagData(ourTag);
      // NOTE: The fetchTagData function actually alters the ourTag array
      // declared earlier before. Nothing is returned because if an array's name
      // pass along into a function, it is actually passing by reference, not by
      // value. That means we are changing the array's contents in the function
      // so nothing needs to be returned! There are other spots in this program
      // where this happens so please keep this in mind.
      
      // While we're at it, why not print out the tag's ID.
      Serial.print("Your tag says it is: ");
      Serial.flush();
      printTag(ourTag);
    }
  }
  else
  {
    // We don't flush the buffer here since we know the buffer is zero.
    tagDetected = false;
  }
  
  // If no tag was detected, then the below code will never execute. But, if
  // there is a tag, then we will see if it belongs to our database.
  if (tagDetected)
  {
    // Now here is the part where we do the database comparison with our handy
    // isValidTag function. And this where you may perform an action
    // if it is or is not valid.
    if (isValidTag(ourTag))
    {
      Serial.println("Come on in and have some freshly baked cookies!!!\n");
      Serial.flush();
    }
    else
    {
      Serial.println("No idea who you are but I have released the hounds!!!\n");
      Serial.flush();
    }
  }
}

//===============================================================================
//  Functions
//===============================================================================
////////////////////////
// flushSerial1Buffer //
////////////////////////
void flushSerial1Buffer() 
{  
  // Now there is a function on the Arduino that is called Serial1.flush
  // but it does not really flush the incoming buffer in recent versions, please
  // check arduino.cc for more information on why. So instead, we will just
  // keep on plucking data off of the serial buffer until it is empty!
  while (Serial1.available() > 0)
  {
    Serial.read();
  }
}

//////////////////
// fetchTagData //
////////////////// 
void fetchTagData(char tempTag[]) 
{  
  // First, pluck off the Start of Text character
  Serial.read();
  
  // Second, read off the tag's actual ID data
  for (int counter = 0; counter < 10; counter++)
  {
    tempTag[counter] = Serial1.read();
  }
 
  // Third, pluck off two checksum, one CR, and one LF characters
  Serial1.read();
  Serial1.read();
  Serial1.read();
  Serial1.read();
  
  // Fourth, pluck off what should be the End of Text character. And
  // while we are plucking, why not throw in a sanity check (mentioned in
  // the article)
  if (Serial1.read() != 3)
  {
    // If for some odd reason the transmission was faulty and we only read
    // in partial information, just throw in dummy data for the tag
    for (int counter = 0; counter < 10; counter++)
    {
      tempTag[counter] = '0';
    } 
  }  
  else
  {
    // But if it all looks good, flush the buffer and keep the previously
    // acquired data
    flushSerial1Buffer();
  }
}

////////////////
// isValidTag //
////////////////
boolean isValidTag(char tempTag[]) 
{  
  boolean result;

  // Compare all of the tags by OR-ing all of the compared tag results. If at
  // least one matches, then it is a valid tag.
  result = compareTags(tempTag, tag1);
      
  return result;
}

/////////////////
// compareTags //
/////////////////
boolean compareTags(char tagA[], char tagB[])
{
  boolean result = true;
  // Basically, we will just compare each character in corresponding array
  // cells until we hit something that does not match. But if it does all
  // match, then our initial state of result will be true.
  for (int counter = 0; counter < 10; counter++)
  {
    if (tagA[counter] != tagB[counter])
    {
      result = false;
      break;
    }
  }
  
  return result;
}

/////////////////
// printTag //
/////////////////
void printTag(char tag[])
{
  // This function just helps identify what the tag ID is so that you
  // may initially read this in, hard code into your program, compile,
  // and then run to have a valid tag in your database.
  for (int counter = 0; counter < 10; counter++)
  {
    Serial.print(tag[counter]);
  }
  Serial.println("");
}


//===============================================================================
//  Header Files
//===============================================================================

//===============================================================================
//  Constants
//===============================================================================
// Here is where we save valid tags. When you see the example video
// of this working, I will first show the invalid tag. Then I will comment this 
// line and uncomment the valid tag. This shows you how to manually set your own
// valid tag. Only difference is you will need to manually type this in. I 
// was able to uncomment because I have done this prior to making the video.

// Bogus tag
char tag1[10] = {'X','X','X','X','X','X','X','X','X','X'};

// Good tag
//char tag1[10] = {'3','6','0','0','6','6','0','0','5','C'};

//===============================================================================
//  Variables
//===============================================================================
char ourTag[10];        // We will use this to hold the interrogated tag's data.
boolean tagDetected;    // We can use this to continue program action if our
                         // reading seems like a real tag was detected.
//===============================================================================
//  Pin Declarations
//===============================================================================
//Inputs:
// Serial1 (pin 19) will be used to grab serial data from buffer which is given
// by the ID12LA

//Outputs:
// Serial (pin 2) will be used to output serial data as user feedback to see
// what is going on

//===============================================================================
//  Initialization
//===============================================================================
void setup() 
{
  // Initialize serial port speed for the serial terminal.
  // We will use Serial1 on our MEGA for RFID data and the Serial for messages
  // to the terminal.
  Serial.begin(9600);
  Serial1.begin(9600);

  // Initialize data flags
  tagDetected = false;
}

//===============================================================================
//  Main
//===============================================================================
void loop() 
{
  // This function call will return the interrogated tag's data in form of a char
  // array. If there was an error in the integrity of the transmission, it will
  // return "0000000000". Of course, this only happens unless there is something
  // in the serial buffer. All the while, we also
  // set a flag if there was data there. This comes into use later on.
  if (Serial1.available() > 0)
  {
    // Give some time for all data to arrive safe and sound into the buffer.
    delay(250);
    
    // The if statement below ensures that the beginning of a tag is seen.
    // Remember that a Start of Text is the decimal value of 2. If we do not
    // see this, all bets are off in even continuing to look further into the
    // buffer. I use peek simply because I don't like touching data until I
    // decide to process it.
    if (Serial1.peek() != 2)
    {
      // Log that we did not have a true tag detected.
      tagDetected = false;
      
      // Flush the buffer to bring it back to an initial, known state.
      flushSerial1Buffer();
    }  
    else
    {
      // Looks like the serial data starts with a valid Start of Text character,
      // let mark that we detected a potential tag.
      tagDetected = true;
     
      // Go and process the tag in the serial1 buffer.
      fetchTagData(ourTag);
      // NOTE: The fetchTagData function actually alters the ourTag array
      // declared earlier before. Nothing is returned because if an array's name
      // pass along into a function, it is actually passing by reference, not by
      // value. That means we are changing the array's contents in the function
      // so nothing needs to be returned! There are other spots in this program
      // where this happens so please keep this in mind.
      
      // While we're at it, why not print out the tag's ID.
      Serial.print("Your tag says it is: ");
      Serial.flush();
      printTag(ourTag);
    }
  }
  else
  {
    // We don't flush the buffer here since we know the buffer is zero.
    tagDetected = false;
  }
  
  // If no tag was detected, then the below code will never execute. But, if
  // there is a tag, then we will see if it belongs to our database.
  if (tagDetected)
  {
    // Now here is the part where we do the database comparison with our handy
    // isValidTag function. And this where you may perform an action
    // if it is or is not valid.
    if (isValidTag(ourTag))
    {
      Serial.println("Come on in and have some freshly baked cookies!!!\n");
      Serial.flush();
    }
    else
    {
      Serial.println("No idea who you are but I have released the hounds!!!\n");
      Serial.flush();
    }
  }
}

//===============================================================================
//  Functions
//===============================================================================
////////////////////////
// flushSerial1Buffer //
////////////////////////
void flushSerial1Buffer() 
{  
  // Now there is a function on the Arduino that is called Serial1.flush
  // but it does not really flush the incoming buffer in recent versions, please
  // check arduino.cc for more information on why. So instead, we will just
  // keep on plucking data off of the serial buffer until it is empty!
  while (Serial1.available() > 0)
  {
    Serial.read();
  }
}

//////////////////
// fetchTagData //
////////////////// 
void fetchTagData(char tempTag[]) 
{  
  // First, pluck off the Start of Text character
  Serial.read();
  
  // Second, read off the tag's actual ID data
  for (int counter = 0; counter < 10; counter++)
  {
    tempTag[counter] = Serial1.read();
  }
 
  // Third, pluck off two checksum, one CR, and one LF characters
  Serial1.read();
  Serial1.read();
  Serial1.read();
  Serial1.read();
  
  // Fourth, pluck off what should be the End of Text character. And
  // while we are plucking, why not throw in a sanity check (mentioned in
  // the article)
  if (Serial1.read() != 3)
  {
    // If for some odd reason the transmission was faulty and we only read
    // in partial information, just throw in dummy data for the tag
    for (int counter = 0; counter < 10; counter++)
    {
      tempTag[counter] = '0';
    } 
  }  
  else
  {
    // But if it all looks good, flush the buffer and keep the previously
    // acquired data
    flushSerial1Buffer();
  }
}

////////////////
// isValidTag //
////////////////
boolean isValidTag(char tempTag[]) 
{  
  boolean result;

  // Compare all of the tags by OR-ing all of the compared tag results. If at
  // least one matches, then it is a valid tag.
  result = compareTags(tempTag, tag1);
      
  return result;
}

/////////////////
// compareTags //
/////////////////
boolean compareTags(char tagA[], char tagB[])
{
  boolean result = true;
  // Basically, we will just compare each character in corresponding array
  // cells until we hit something that does not match. But if it does all
  // match, then our initial state of result will be true.
  for (int counter = 0; counter < 10; counter++)
  {
    if (tagA[counter] != tagB[counter])
    {
      result = false;
      break;
    }
  }
  
  return result;
}

/////////////////
// printTag //
/////////////////
void printTag(char tag[])
{
  // This function just helps identify what the tag ID is so that you
  // may initially read this in, hard code into your program, compile,
  // and then run to have a valid tag in your database.
  for (int counter = 0; counter < 10; counter++)
  {
    Serial.print(tag[counter]);
  }
  Serial.println("");
}


**********************************

//Robot Control

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
pinMode(2,1);  
pinMode(3,1);
pinMode(4,1);
pinMode(5,1);
}
 char a; int i=0;
void loop() {
  // put your main code here, to run repeatedly:


if(Serial.available())
{a=Serial.read();


if(a=='f')
{digitalWrite(2,0);
digitalWrite(3,1);
digitalWrite(4,0);
digitalWrite(5,1);
}

if(a=='b')
{digitalWrite(2,1);
digitalWrite(3,0);
digitalWrite(4,1);
digitalWrite(5,0);
  
}

if(a=='l')
{digitalWrite(2,1);
digitalWrite(3,1);
digitalWrite(4,1);
digitalWrite(5,0);
  
}

  if(a=='r')
{digitalWrite(2,1);
digitalWrite(3,0);
digitalWrite(4,1);
digitalWrite(5,1);
  
}

if(a=='s')
{digitalWrite(2,1);
digitalWrite(3,1);
digitalWrite(4,1);
digitalWrite(5,1);
  
}
}





}

**********************
//WiFi Code

#include "WiFiEsp.h"
#include <SoftwareSerial.h>

SoftwareSerial Serial1(2,3);
char ssid[]="vivo Y21L"; //wifi name
char pass[] ="durgesh5" ; //password
int status= WL_IDLE_STATUS;    //by default return status
int reqCount=0;

WiFiEspServer server(80);   //class object port no



void setup()
{
  Serial.begin(9600); //hardware
  Serial1.begin(9600); //software

  WiFi.init(&Serial1); //initialize port 

  if(WiFi.status()==WL_NO_SHIELD)  //no hardware connected 
  {
    Serial.println("Wifi shield is not available");
    while(true); //if not connected then program will be paused - hang
  }

  while(status!=WL_CONNECTED)
  {Serial.println("Attempting to connect");
  Serial.println(ssid);

  status=WiFi.begin(ssid,pass);
  }
  Serial.println("you are connected");
  
  printWiFiStatus();
  server.begin();
  
  }



void loop() {
  // put your main code here, to run repeatedly:
  //to check wifi client available

  WiFiEspClient client = server.available();

  if(client)
  {
    Serial.println("New Client");
    boolean currentLineIsBlank=true;

    while(client.connected())
    {
    if(client.available())
    {
      char c= client.read();

      Serial.write(c);

      if(c=='\n' && currentLineIsBlank)
      {
        Serial.println("Sending Response");

        client.print("HTTP/1.1 200 OK \r\n" "Content- Type : text/html \r\n" " Connection :close \r\n"  "Refresh: 20 \r\n" "\r\n");
        //200 for connection like 404   refresh code so that page can be refreshed in itnerval of 20

        client.print("<!DOCTYPE HTML> \r\n");
        client.print("<html>\r\n");
        client.print("<h1> Hello WORLD</H1>\r\n");
        client.print("Request recceived:");
        client.print("++reqCount");
        client.print("<br>\r\n");
        client.print("</html>\r\n");
        break;

      }
      if(c=='\n')
      {
        currentLineIsBlank =true;
      }

      else if(c!='\r')
      {
        currentLineIsBlank = false;
      }

      
      }
        
        }
    delay(10);  //as browser take time to receive data
    client.stop();
    Serial.println("Client disconnected");
    }
  }

void printWiFiStatus()
{
  Serial.print("SSID:  ");
  Serial.println(WiFi.SSID());

  IPAddress ip= WiFi.localIP();

  Serial.print("SSID:");
  Serial.println(WiFi.SSID());

  //IPAddress ip= WiFi.localIP();

  Serial.print("IP Adress: ");
  Serial.println(ip);
  
}
