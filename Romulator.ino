v//--------------------------------------------------------------------------------------------
// ROMulator 3

// Compiled using Arduino 1.65 or later

// Revisted March 2024

// A customised vesrion of SIMPL to allow RAM and Z80 bus to be exercised using SIMPL.

// Address is sent using a pair of 74HC595 shift registers
// Data is passed through a 74HC245 bi-directional buffer
// TRISTATE line when raised isolates the address and data registers from the Z80 bus - allowing it to run
// RESET - active low is used to put the Z80 into RESET so that the RAM can be loaded

// Other active low  bus control lines are READ, WRITE, RAM_SEL (/CS) and IORQ


//  Basic memory r/w Commands are as follows  - values are integer decimal

//  170 55w      write the value of 170 to address 55
//  32760 44w    write the value 44 to the RAM address 32760

//  55r          read and print the value stored at address 55
//  132n         output 132 on the ROmulator data port
//  240q         output an 8 bit address 240 on the lower address lines (for I/O operations)

// -------------------------------------------------------------------------------------------

  #define bufRead(addr)      (*(unsigned char *)(addr))
  #define bufWrite(addr, b)   (*(unsigned char *)(addr) = (b))

   unsigned char bite;
   unsigned int x = 0;
   unsigned int y = 0;
   int len = 48;
   
   
   // Define the I?O pins to be used with the shift registers
   
   int sinPin = 5;      // Serial input pin from 74HC165
   int sclkPin = 6;     // Clock pin to 74HC165
   int loadPin = 7;     // parallel load pin of 74HC165   
   int latchPin = 8;    // Pin connected to ST_CP of 74HC595  - white
   int dataPin = 12;    // Pin connected to DS of 74HC595 - green
   int clockPin = 11;   // Pin connected to SH_CP of 74HC595  - blue


// Z80 Bus Control lines - all active low Digital Outputs

   int RAM_SEL=14;
   int READ = 15;
   int WRITE = 16;
   int TRISTATE = 17;
   int IORQ = 18;
   int RESET = 19;


   
  
   char array[26][48] = {                                  // Define a 26 x 64 array for the colon definitions
                         {"6d75{1o708u0o708u}"},
                         {"9rP8rP12rP4rP6rP2rP3rP1rP"},    // A backward step on a stepper motor
                         {"6d91{1o585u0o585u}"},
                         {"6d100{1o532u0o532u}"},
                         {"6d110{1o484u0o484u}"},
                         {"1rP3rP2rP6rP4rP12rP8rP9rP"},    // A forward step on a stepper motor
                         {"6d133{1o484u0o484u}"},
                         {"_Hello World, and welcome to SIMPL_"},
                         {"5{ABC}"},
                         {" "},
                         {" "},
                         {" "},
                         {" "},
                         {" "},
                         {" "},
                         {"100m"},    // Time delay to see stepper motor action
                         
                         };
                         
  

    int d = 5;
    char name;
    
    char* parray;
    
    char buf[64];
    
    char* addr;

//------------------------------------------------------------------------------   
  
void setup()
{
    Serial.begin(115200);

// Set up the various port lines for the ROMulator

// Data bus
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
// Bus control

    pinMode(RAM_SEL,OUTPUT);
    pinMode(READ,OUTPUT);
    pinMode(WRITE,OUTPUT);
    pinMode(TRISTATE,OUTPUT);
    pinMode(IORQ,OUTPUT);
    pinMode(RESET,OUTPUT);
    
// Shift register control
    pinMode(loadPin, OUTPUT);          // Set up the pins needed for the shift registers
    pinMode(sclkPin, OUTPUT);
//    pinMode(sinPin, INPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitalWrite(sclkPin,HIGH);    // initialise the clock HIGH


      x = 170;  // Test data - alternate 1's and 0's 

      if(x>=128){digitalWrite(10,HIGH); x =  x- 128;} else {digitalWrite(10,LOW);}
      if(x>=64){digitalWrite(9,HIGH); x =  x- 64;} else {digitalWrite(9,LOW);}
      if(x>=32){digitalWrite(7,HIGH); x =  x- 32;} else {digitalWrite(7,LOW);}
      if(x>=16){digitalWrite(6,HIGH); x =  x- 16;} else {digitalWrite(6,LOW);}
      if(x>=8){digitalWrite(5,HIGH); x =  x- 8;} else {digitalWrite(5,LOW);}
      if(x>=4){digitalWrite(4,HIGH); x =  x- 4;} else {digitalWrite(4,LOW);}
      if(x>=2){digitalWrite(3,HIGH); x =  x- 2;} else {digitalWrite(3,LOW);}
      if(x>=1){digitalWrite(2,HIGH); x =  x- 1;} else {digitalWrite(2,LOW);}





  digitalWrite(READ,HIGH);
  digitalWrite(WRITE,HIGH);
  digitalWrite(RAM_SEL,HIGH);


    Serial.println("Type H for Welcome or ? for Help");
    
    parray = &array[0][0];                // parray is the pointer to the first element 

    
}

// -----------------------------------------------------------------------------------------------------

void loop()             // The SIMPL interpreter is just the following 3 functions executed within a loop
{

  txtRead(buf, 64);      // Get the next character from the buffer
  txtChk(buf);           // check if it is a colon definition 
  txtEval(buf);          // Evaluate the character and execute the code associated with it
}

void txtRead (char *p, byte n) {
  byte i = 0;
  while (i < (n-1)) {
    while (!Serial.available());
    char ch = Serial.read();
    if (ch == '\r' || ch == '\n') break;
    if (ch >= ' ' && ch <= '~') {
      *p++ = ch;
      i++;
    }
  }
  *p = 0;
}


void txtChk (char *buf) {       // Check if the text starts with a colon and if so store in temp[]
 
  if (*buf == ':')  {  
  char ch;
  int i =0;
  while ((ch = *buf++)){
    
  if (ch == ':') {
  Serial.println(*buf);   // get the name from the first character
  name = *buf ;
  buf++;
  }
 
  bufWrite((parray + (len*(name-65) +i)),*buf);
   
  i++;

}

}

}
  
  void txtEval (char *buf) {
  unsigned int k = 0;

  char *loop;
  char ch;
  while ((ch = *buf++)) {
    

    switch (ch) {
    case '0':                  // Ennumerate it to a variable x if the characters are digits
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      x = ch - '0';
      while (*buf >= '0' && *buf <= '9') {
        x = x*10 + (*buf++ - '0');
      }
      break;
      
    case 'p':                        // Print out the value of x
      Serial.println(x);
      break;
      
      case '=':                    // more familiar for maths
      Serial.println(x);
      break;
      
    case 'd':
      d = x;
      break;
      
      
      
      case 'A':            // Point the interpreter to the array containing the words
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
          
      name = ch - 65;
      addr = parray + (len*name);      
      txtEval(addr);
      break;


      case ' ':

      y = x;
      
      case 'y':
      y = x;

 
      case '!':     // store
      y = x;
      break;
      
      case '@':     // fetch
      x = y;
      break;
      
      case '+':     // Add
      x = x+y;
      break;
      
      case '-':     // Subtract
      x = y-x;
      break;
      
      case '*':     // Multiply
      x = x*y;
      break;
      
      case '/':    // Divide 
      x = y/x;
      break;
      
      
      case '?':                             // Print out all the RAM
      parray = &array[0][0];                // reset parray to the pointer to the first element 
      
      for (int j = 0; j<26; j++) {      
      Serial.write(j+65);                  // print the caps word name
      Serial.write(20);                    // space   
      for (int i=0; i<len; i++) {
      bite = bufRead( parray +  (j *len )+i);          // read the array
      Serial.write(bite);                           // print the character to the serial port
    }
     Serial.println();
    
   }
      
   for(int i = 0; i <11; i++) {                 // Print 12 free lines so it looks better on Arduino serial screen
   Serial.println();
   
    }      
    break;
      
      
    case 'i':
      x = digitalRead(d);
      break;
    case 'o':
      digitalWrite(d, x%2);
      break;
    case 'm':
      delay(x);
      break;
    case 'u':
      delayMicroseconds(x);
      break;
    case '{':
      k = x;
      loop = buf;
      while ((ch = *buf++) && ch != '}') {
      }
    case '}':
      if (k) {
        k--;
        buf = loop;
      }
      break;
    case 'k':
      x = k;
      break;
    case '_':
      while ((ch = *buf++) && ch != '_') {
        Serial.print(ch);
      }
      Serial.println();
      break;
    case 's':
      x = analogRead(x);
      break;
      
      case 'q':            // Send an 8 bit byte to the shift register using shiftOut
      
      PORTB &= ~_BV(0);
      shiftOut(dataPin, clockPin, MSBFIRST, x);  // shift out the bits:   
      PORTB |= _BV(0);
      break;



        case 'n':                  
        
        // Output an 8 bit value on I/O Dig 2  - Dig 9
        // Can be extended to 12 bits on Dig 2 - Dig 13


      // Make D2 to D9 OUTPUTS

      
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);    
      
      if(x>=128){digitalWrite(10,HIGH); x =  x- 128;} else {digitalWrite(10,LOW);}
      if(x>=64){digitalWrite(9,HIGH); x =  x- 64;} else {digitalWrite(9,LOW);}
      if(x>=32){digitalWrite(7,HIGH); x =  x- 32;} else {digitalWrite(7,LOW);}
      if(x>=16){digitalWrite(6,HIGH); x =  x- 16;} else {digitalWrite(6,LOW);}
      if(x>=8){digitalWrite(5,HIGH); x =  x- 8;} else {digitalWrite(5,LOW);}
      if(x>=4){digitalWrite(4,HIGH); x =  x- 4;} else {digitalWrite(4,LOW);}
      if(x>=2){digitalWrite(3,HIGH); x =  x- 2;} else {digitalWrite(3,LOW);}
      if(x>=1){digitalWrite(2,HIGH); x =  x- 1;} else {digitalWrite(2,LOW);}
      
      break;
 //-------------------------------------------------------------------------------------------     
      
      case 'r':            // read the value at the data port D2 - D10

      // Set up the address  - write the 16 bits to the address registers

      PORTB &= ~_BV(0);
      shiftOut(dataPin, clockPin, MSBFIRST, x>>8);  // shift out the high bits: 
      shiftOut(dataPin, clockPin, MSBFIRST, x);  // shift out the low bits:   
      PORTB |= _BV(0);
  

      pinMode(2, INPUT) ;
      pinMode(3, INPUT);
      pinMode(4, INPUT);
      pinMode(5, INPUT);
      pinMode(6, INPUT);
      pinMode(7, INPUT);
      pinMode(9, INPUT);
      pinMode(10, INPUT);

      // Assert RAM_SEL

      digitalWrite(RAM_SEL, LOW);


      // Take the buffer out of tri-state

     digitalWrite(TRISTATE,LOW);


      // Assert READ

      digitalWrite(READ, LOW);

    //  Get the data

    x = 0;

    if (digitalRead(2)){ x =  x+1;}
    if (digitalRead(3)){ x =  x+2;}
    if (digitalRead(4)){ x =  x+4;}
    if (digitalRead(5)){ x =  x+8;}
    if (digitalRead(6)){ x =  x+16;}
    if (digitalRead(7)){ x =  x+32;}
    if (digitalRead(9)){ x =  x+64;}
    if (digitalRead(10)){ x =  x+128;} 

//   delay(100);
 
      digitalWrite(READ, HIGH);
      digitalWrite(RAM_SEL, HIGH);     // Deselect the RAM
      digitalWrite(TRISTATE,HIGH);    // Put the buffer into tri-state

      Serial.println(x);

      

      break;

//----------------------------------------------------------------------
      
      
      
      
      // Send an 16 bit byte to the shift register using shiftOut


      



  case 'w':


  
// Write Cycle

// Connect DIR to /WR as a write cycle is from B to A and a read is from A to B




// Set up the address  - write the 16 bits to the address registers

      PORTB &= ~_BV(0);
      shiftOut(dataPin, clockPin, MSBFIRST, x>>8);  // shift out the high bits: 
      shiftOut(dataPin, clockPin, MSBFIRST, x);  // shift out the low bits:   
      PORTB |= _BV(0);
     


      x = y;        // get the data byte

      // Assert WRITE   - sets the direction of the bi-directional buffer

      digitalWrite(WRITE, LOW);

     // Take the buffer out of tri-state

     digitalWrite(TRISTATE,LOW);


// Set up the data  - output the 8 bits D0-D7 on Digital pins D2-D9

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);    


      if(x>=128){digitalWrite(10,HIGH); x =  x- 128;} else {digitalWrite(10,LOW);}
      if(x>=64){digitalWrite(9,HIGH); x =  x- 64;} else {digitalWrite(9,LOW);}
      if(x>=32){digitalWrite(7,HIGH); x =  x- 32;} else {digitalWrite(7,LOW);}
      if(x>=16){digitalWrite(6,HIGH); x =  x- 16;} else {digitalWrite(6,LOW);}
      if(x>=8){digitalWrite(5,HIGH); x =  x- 8;} else {digitalWrite(5,LOW);}
      if(x>=4){digitalWrite(4,HIGH); x =  x- 4;} else {digitalWrite(4,LOW);}
      if(x>=2){digitalWrite(3,HIGH); x =  x- 2;} else {digitalWrite(3,LOW);}
      if(x>=1){digitalWrite(2,HIGH); x =  x- 1;} else {digitalWrite(2,LOW);}


// Select the RAM

 // Assert RAM_SEL

  digitalWrite(RAM_SEL, LOW);

   
     


    //  Send the data

//    delay(100);


// Assert /WR

// Deselct RAM
digitalWrite(RAM_SEL,HIGH);


// De-assert /WR
digitalWrite(WRITE, HIGH);


 // Put the buffer into tri-state

     digitalWrite(TRISTATE,HIGH);



break;


// Tristate registers and data buffer


// Read Cycle

// Set up the address
// Set up the data 
// Select the RAM
// Assert /RD
// Read the data from the port into memory
// De-assert /RD
// Deselct RAM
// Tristate registers and data buffer

      
      
//      PORTB &= ~_BV(0);
//      shiftOut(dataPin, clockPin, MSBFIRST, x>>8);  // shift out the high bits: 
//      shiftOut(dataPin, clockPin, MSBFIRST, x);  // shift out the low bits:   
//      PORTB |= _BV(0);
//      break;
      
      case 't':     // test the inputs from 74HC165 shift registers
     
     // Read incoming word from 74HC165

      digitalWrite(loadPin, LOW);
      digitalWrite(loadPin, HIGH);
      x = shiftIn(sinPin, sclkPin, MSBFIRST);  
      digitalWrite(sclkPin,HIGH); 
      
      break;
      
      
    }
  }
}