
// This code emulates a Blaupunkt CDC-A08
// Protocol: 9-databits, no parity, 1 stopbit, 0-5V TTL (inverted) serial (requires a modified HardwareSerial)
// Tested with: Blaupunkt Travelpilot DX-R52 (Warning: Blaupunkt may use different protocols across generations of radio's)
// Arduino boards: Mega 2560 R3, Micro
// Arduino has to be reset before radio is turned on (use the switched 12v line from the CDC connector with a relay)

// Status:
// Initialization and mag scan: Fully implemented, except for mag present/not present status (not required)
// Playback controls (play/pause/track change/disk change): Fully implemented
// Special controls (mix/repeat/fast forward/rewind): Not implemented
// Position report (time): Implemented, but counting up goes wrong (gives strange times but doesn't affect anything)
// Serial sync/ping: Implemented, but doesn't actually check if pongs are received
// Cold/Warm start: Fully implemented
// Far from pretty, we're using the send-and-hope-for-the-best approach here...

////////////////////////
//  CD CHANGER SETUP  //
////////////////////////

//Disk Magazine (always contains 10 disks thanks to laziness)
int numTracks = 0x99; //Number of tracks per disk (1-99)
int runtimeMin = 0x25; //Disk Runtime: Minutes
int runtimeSec = 0x1; //Disk Runtime: Seconds
int trackLength = 0x05; //Length of each track in minutes

//Currently Playing
int currentDisk = 0x1; //Disk currently playing
int currentTrack = 0x1; //Track currently playing
int currentMinutes = 0x0; //Position: minutes
int currentSeconds = 0x0; //Position: Seconds
int playStatus = 0xA;

void setup(){
  //Serial.begin(115200); //Pc, for debugging
  Serial1.begin(4800, SERIAL_9N1); //start with 4800 baud, later we switch to 9600
  UCSR1B |= ( 1 << UCSZ11 );
}

//Command buffer and states
boolean c = false;
int cmdCapture[16];
int cmdCaptureIndex=0;
long m = 0;
long g = 0;
boolean huLastWord = false;

void loop()
{
  ////////////////////////////
  //  CD CHANGER FUNCTIONS  //
  ////////////////////////////
  if (Serial1.available() > 0) {
    int incoming = Serial1.read();
    //if(incoming != 0x14F){tx(incoming);}
    //capture command; read every byte back to the HU, except for the end of command byte
    if (incoming == 0x180){c=true;delayMicroseconds(400);tx(incoming);g = millis();} //start of command
    else if (c){
      if(incoming == 0x14F){c=false;interpret(cmdCapture, cmdCaptureIndex);cmdCaptureIndex=0;} //end of command
      else{cmdCapture[cmdCaptureIndex]=incoming;cmdCaptureIndex++;tx(incoming);} //store the value
    }
    
  }
  else if(playStatus == 0x9 && millis()-m > 1500){
    m = millis();
    currentSeconds += 0x1;
    if (currentSeconds == 0x60){currentSeconds = 0x0; currentMinutes += 0x1;}
    int t[]={0x109,currentMinutes,currentSeconds,0x14F}; transmit(t,4); //transmit the current playback position, counting goes wrong but that doesn't matter, as long as the HU gets two numbers at regular intervals it's ok
    
    if(currentMinutes >= trackLength){ //reset the timer when this track ends, we aren't going to actually load the next track
        currentSeconds = 0x0;
        currentMinutes = 0x0;
    }
  }

////////////////////////////
//  CD CHANGER FUNCTIONS  //
////////////////////////////

//TX Routine
void tx(int value){
  if(value < 256){
    UCSR1B = 0b10011100; //Turn 9th bit off
    Serial1.write(value);
  }
  else if(value > 255){
    UCSR1B = 0b10011101; //Turn 9th bit on
    Serial1.write(value-256);
  }
}
//Send a formatted command sequence
void transmit(int args[], int length){
  if(huLastWord){
    tx(args[0]);
    huLastWord = false;
    delay(46);
  }
  for (int i=0; i<length; i++){
    delay(8);
    tx(args[i]); //transmit the byte
  }
}
//Process a received command
void interpret(int args[], int length){
  huLastWord = true;
  // ====================== PLAYBACK ====================== //
  if(args[0] == 0x99){ //track change
    int prevDisk = currentDisk;
    int prevTrack = currentTrack;
    
    currentDisk = args[1];
    if(args[1] > 0x9){currentDisk = 0x1;} //catch a disk select to an impossible slot, happens from time to time
    currentTrack = args[2];
    if(args[2] == 0x0){currentTrack = 0x1;} //catch a track select to track 0, happens from time to time
    currentMinutes = 0x0;
    currentSeconds = 0x0;
    Serial.print("Loading Disk: "); Serial.print(currentDisk,HEX); Serial.print(" Track: "); Serial.println(currentTrack,HEX);
    delay(30);
      int t1[]={0x101,currentDisk,currentTrack,0x14F}; transmit(t1,4); //tell the HU that we're loading the selected track
    delay(30);
      int t2[]={0x10D,0x1,numTracks,runtimeMin,runtimeSec,0x14F}; transmit(t2,6); //give the HU some info about the current disk
    delay(30);
      int t3[]={0x109,currentMinutes,currentSeconds,0x14F}; transmit(t3,4); //set the timer to 0:00
    delay(30);
      int t4[]={0x101,currentDisk,currentTrack,0x14F}; transmit(t4,4); //tell the HU that we've loaded the selected track
    delay(30);
    //  int t5[]={0x10B,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x14F}; transmit(t5,10); //Do the timing/ping thing (not really needed)
    //delay(30);

   // Do something with the play/pause/next/previous command here
    
  }
  else if(args[0] == 0x21){ //Pause
    Serial.println("Pause");
    playStatus = 0xA;
    delay(30);
    int t1[]={0x103,0x20,playStatus,0x20,0x0,0x14F}; transmit(t1,6); delay(40); transmit(t1,6);//tell the HU that we've paused playback, twice (sometimes bytes get lost when switching to FM radio, the HU doesn't care how many times you send this)
    delay(30);
  }
  else if(args[0] == 0xA5){ //Play
    Serial.println("Play");
    playStatus = 0x9;
    delay(30);
    int t1[]={0x101,currentDisk,currentTrack,0x14F}; transmit(t1,4); //tell the HU what track we're playing
    delay(32);
    int t2[]={0x103,0x20,playStatus,0x20,0x0,0x14F}; transmit(t2,6); //tell the HU that we've resumed playback
    delay(30);
    int t4[]={0x10B,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x14F}; transmit(t4,10); //Do the timing/ping thing
    delay(30);
    int t3[]={0x10D,0x1,numTracks,runtimeMin,runtimeSec,0x14F}; transmit(t3,6); //give the HU some info about the current disk
    delay(30);

  }
  // ====================== MISC ====================== //
  else if(args[0] == 0x48){ //Baud rate change on startup
    if(args[1] == 0x1){ //4800
      Serial.println("Baudrate: 4800");
      delay(30);
      int t1[]={0x10F,0x48,0x1,0x14F}; transmit(t1,4);
    }
    else if(args[1] == 0x2){ //4800
      Serial.println("Baudrate: 9600");
      Serial1.end();
      Serial1.begin(9600, SERIAL_9N1);
    }
  }
  else if(args[0] == 0xA7){ //Status report
      Serial.println("Status report");
      delay(30);
      int t1[]={0x10E,0x8,0x1,0x14F}; transmit(t1,4); //No idea, HU expects this response...
  }
  else if(args[0] == 0xA9){ //Report the magazine status
      Serial.println("MAG status report");
      delay(30);
      int t1[]={0x10C,0x0,0x14F}; transmit(t1,3); //Mag status 0x0 (scanned), 0x1 (mag just inserted, needs to be scanned), 0x2 (no mag inserted), HU always scans the mag on cold start
  }                  
                     // ???             //Mix mag/off      //Repeat track     //Repeat disk      //Repeat off       //Mix disk         // ???
  else if(args[0] == 0x11 || args[0] == 0x96 || args[0] == 0x93 || args[0] == 0x94 || args[0] == 0x95 || args[0] == 0xA3 || args[0] == 0x87){ //All of these expect a playback and disk status report
      Serial.println("Playback status report");
      delay(30);
      int t1[]={0x103,0x20,playStatus,0x20,0x0,0x14F}; transmit(t1,6); //tell the HU the play status
      delay(30);
      int t2[]={0x10B,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x14F}; transmit(t2,10); //Do the timing/ping thing
      delay(30);
      int t3[]={0x10D,0x1,numTracks,runtimeMin,runtimeSec,0x14F}; transmit(t3,6); //give the HU some info about the current disk
      delay(30);
  }
  else{
     Serial.print("Unknown: ");
     for(int i=0; i < sizeof(args); i++){
       Serial.print(args[i],HEX);
       Serial.print(", ");
     }
     Serial.println(";");
  }
}