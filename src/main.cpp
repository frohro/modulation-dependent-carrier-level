#define DEBUG

#include <Arduino.h>
#include <SPI.h>
#include <QNEthernet.h>
#include <globals.h>

using namespace qindesign::network;

// Define the IP address, subnet mask, and gateway
IPAddress ip(10, 7, 1, 12);     // Change to your desired IP
IPAddress sn(255, 255, 255, 0); // Subnet mask
IPAddress gw(10, 7, 1, 71);     // Default gateway

// Create a TCP server on port 5100
EthernetServer tcpServer(5100); // We probably don't want to use 5100.

const int L1Pin = 37;          // GPIO pin for L1, which indicates valid data for serial input to Teensy 4.1
const int LCPin = 36;          // GPIO pin for LC, which indicates when I can send data to the serial register for the next cycle
const int chipSelectPin0 = 10; // Chip select pin for SPI0
const int chipSelectPin1 = 38; // Chip select pin for SPI1
const int SSBOnOffPin = 32;    // Chip select pin for SPI0
const int NFBereitPin = 30;    // Chip select pin for SPI0
const int DCCOnOffPin = 29;    // Chip select pin for SPI0
const int powerPin = 19;       // pin connected to LED to show that board has power
const int connectedPin = 18;   // pin to show connected with the server
const int AMCPin = 16;         // used to activate LED to show AMC is being used
const int DCCPin = 17;         // used to activate LED to show DCC is being used
const int L5Pin = 34;          // GPIO pin for L5, which indicates when the Teensy 4.1 can send data to the transmitter

SPISettings spiSettings(80000000, MSBFIRST, SPI_MODE0); // 30 MHz, MSB first, SPI mode 0
// We can go to 80 MHz and the output shows it works.
// declare functions
void receive2ByteWord();
void send2ByteWord();
void getAndProcessMDCLTCPCommand(EthernetServer &tcpServer);
uint16_t processMDCL(uint16_t one_plus_m_t);
#ifdef DEBUG
// Debugging functions are here.  You can remove them later.
uint16_t generate_triangle_wave(uint16_t amplitude, uint32_t period_us);
void print_for_plot(uint16_t N, uint16_t variable1, const char* label1, 
                    uint16_t variable2 = 0, const char* label2 = "", 
                    uint16_t variable3 = 0, const char* label3 = "");
#endif 

// global variables
uint16_t receivedWord = 2048; // stores the 16 bit word coming from serial in
uint16_t wordToSend;          // stores word when it is ready to be sent to the transmitter
int samplesPerDecay = 10;      // Out of 4095, how much decay is there per sample for the slow decay of m_p(t)  1 is way to fast!
int validWord = 0;            // indicates the 16 bit word is ready to process 0 = not valid, 1 = valid
int currentMDCLNumber = 1;    // 0 = dcc, 1 = amc, 2 = other
float percentAdjust = 1;                                         ;      // range of 0 to 1 ... used to adjust overall voltage or power output of transmitter.
uint16_t processedWord;

// timer for sign wave
unsigned long startTime = micros();
unsigned long endTime;
unsigned long elapsedTime;
unsigned long timeForCycles = 1;
int count = 0;

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);    // Serial port for debugging
  Serial3.begin(9600); // Extra debug port (/dev/ttyUSB0 or /dev/ttyUSB1 on linux)
#endif
  if (!Ethernet.begin(ip, sn, gw)) // Start Ethernet with a static IP
  {
#ifdef DEBUG
    Serial.println("Failed to start Ethernet");
#endif
    while (true)
      ; // Halt if Ethernet fails to start
  }
  tcpServer.begin(); // Start the TCP server
#ifdef DEBUG
  Serial.print("TCP Server is at ");
  Serial.println(Ethernet.localIP());
#endif
  // Initialize SPI0
  SPI.setMOSI(11); // Set MOSI pin for SPI0
  SPI.setMISO(12); // Set MISO pin for SPI0
  SPI.setSCK(13);  // Set SCK pin for SPI0
  SPI.begin();

  // Initialize SPI1
  SPI1.setMOSI(26); // Set MOSI pin for SPI1
  SPI1.setMISO(39); // Set MISO pin for SPI1
  SPI1.setSCK(27);  // Set SCK pin for SPI1
  SPI1.begin();

  pinMode(L1Pin, INPUT); // Set pin as input for L1
  pinMode(LCPin, INPUT); // Set pin as input for LC  // Not used presently.
  pinMode(L5Pin, INPUT); // Set pin as input for L5

  attachInterrupt(L1Pin, receive2ByteWord, FALLING); // Attach interrupt to pin, triggered on falling edge
  attachInterrupt(L5Pin, send2ByteWord, FALLING);    // Attach interrupt to pin, trigger on falling edge
  // Using L5 instead of LC because it has the data ready quicker for the transmitter.
  // L1 indicates the transmitter is ready, but this makes the data ready for sooner than L1, but still after
  // L1.
  // Set high priority for the interrupts
  // NVIC_SET_PRIORITY(IRQ_GPIO2_16_31, 0); // Highest priority for GPIO interrupts (L1Pin and LCPin)

  pinMode(AMCPin, INPUT);// Rob change this when 0 Ohm resistors are replaced with 470
  pinMode(DCCPin, INPUT);// Rob change this when 0 Ohm resistors are replaced with 470
  pinMode(powerPin, INPUT);// Rob change this when 0 Ohm resistors are replaced with 470
  pinMode(connectedPin, INPUT);  // Rob change this when 0 Ohm resistors are replaced with 470
  pinMode(chipSelectPin0, OUTPUT);
  pinMode(chipSelectPin1, OUTPUT);
  pinMode(SSBOnOffPin, INPUT);        //
  pinMode(NFBereitPin, INPUT);        //
  pinMode(DCCOnOffPin, INPUT);        //
  pinMode(22, OUTPUT);                // Debugging
  pinMode(23, OUTPUT);               // Debugging
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(chipSelectPin0, HIGH); // Ensure CS0 is high  (not really needed as we are not using it)
  digitalWrite(chipSelectPin1, HIGH); // Ensure CS1 is high
  digitalWrite(powerPin, HIGH);       // Show that power is on for the board
  // sei(); // Enable global interrupts
}

void loop() // This loop should take < 10 us to run or we are in trouble.
{
  if (validWord == 1) // check if new 16 bit word has been received from SPI0
  {
#ifdef DEBUG 
    //  wordToSend = 648;  // DC we can go to 700, but 512 is too low
    // The screen current trips.
    // wordToSend = processMDCL((uint16_t)(generate_triangle_wave(1000,1000))); // Process the received word (standard DSB)
    wordToSend = processMDCL(receivedWord); // Process the received word 
    if (wordToSend < 648) wordToSend = 648; // This is the minimum for the screen current to trip.
    // wordToSend = processMDCL((generate_triangle_wave(2000, 1000))); // Process the received word  
#endif
    validWord = 0; // wait until new word is received again
  }
  getAndProcessMDCLTCPCommand(tcpServer);
#ifdef DEBUG
  // digitalWrite(22, !digitalRead(22)); // Debugging
#endif
}

// ISR function called by the interrupt that is triggered by L5 to send the
// processed word to the transmitter (DCC board used LC, but L5 is faster)
void send2ByteWord()
{
  SPI1.beginTransaction(spiSettings);

  digitalWrite(chipSelectPin1, LOW); //  We are not using CS1.
  SPI1.transfer16(wordToSend);       // Send the 16-bit word to the transmitter
  digitalWrite(chipSelectPin1, HIGH);
  SPI1.endTransaction();
}

// ISR function called by the interrupt that is triggered by L1
// to process the new word from the transmitter
void receive2ByteWord() // Receive 16-bit word from SPI0
{                       // Select the slave device
  SPI.beginTransaction(spiSettings);
  digitalWrite(chipSelectPin0, LOW);
  // Send a dummy byte to initiate the SPI read and receive the 16-bit word
  uint16_t dummy = 0x00;
  receivedWord = SPI.transfer16(dummy); // Deselect the slave device
  digitalWrite(chipSelectPin0, HIGH);
  validWord = 1; // Indicates that the new 16-bit word is received and ready to process
  SPI.endTransaction();
}

uint16_t processMDCL(uint16_t one_plus_m_t)
// See the MDCL.ipynb for the theory on this.
{ // This function takes a 16-bit word whose lower 12 bits is 1+m(t) and processes it according to the MDCL algorithm
  // We could put a check for overmodulation here if wanted (Check for repeated 0's)
  if (digitalRead(SSBOnOffPin) == LOW && digitalRead(NFBereitPin) == HIGH && digitalRead(DCCOnOffPin) == HIGH)
  { // if MCDL is on
    uint16_t m_p;
    static uint16_t m_pPrevious = 0;     // store the previous m_p(t) value
    static uint16_t samplesUntilDecay = samplesPerDecay; // This is the number of samples until the decay starts

    if (one_plus_m_t < 2048)           // Find |m(t)| value where 1 + m(t) scaled to 0 to 4095
    {
      m_p = 2048 - one_plus_m_t; // This should not overflow.
    }
    else
    {
      m_p = one_plus_m_t - 2048; // This should not overflow.
    }
    #ifdef DEBUG
    uint16_t m_p_a;
    m_p_a = m_p;  // mpa is for debugging.  It is m_p without the decay.
    #endif
    // m_p(t) needs to be a fast attack, slow decay function of time
    if (m_p <= m_pPrevious) // m_p is decreasing
    { 
      m_p = m_pPrevious; // m_p(t) should not decrease
      if (samplesUntilDecay > 0) // Will decay down to close to 0 (within samplesPerDecay) if m_p(t) is not increasing
      {
        samplesUntilDecay--;
      }
      else // sammples until decay is 0, so decay one step
      {
        if (m_p > 0) m_p--; // m_p should not be decreased below zero
        samplesUntilDecay = samplesPerDecay; // Reset the decay counter
      }
    }
    else // m_p in increasing so don't decay m_p(t).  It needs to rise instantly, but samplesPerDecay needs to be reset so it is ready for the next decay.
    {
      samplesUntilDecay = samplesPerDecay; // Reset the decay counter
    }
    m_pPrevious = m_p; // Store the current m_p(t) value for the next cycle
    if (currentMDCLNumber == 0) // DCC
    {
      if (digitalRead(DCCPin) != HIGH) // Change indicator LEDs if needed
      {
        digitalWrite(DCCPin, HIGH);
        digitalWrite(AMCPin, LOW);
      }
      uint16_t dccK; // See the graph of DCC k as a function of m_p(t).  We may want to pass the 430 and 1024 parameters in someday
      if (m_p > 1024)
      {
        dccK = m_p;
      }
      else if (m_p < 430) // Using curve as used for ABB high power transmitters (> 100 kW)
      {
        dccK = 1454 - m_p;  // 1454 = 1024 + 430
      }
      else
      {
        dccK = 1024;
      }
      return percentAdjust * ((dccK + one_plus_m_t)-2048); // percentAdjust is to turn the power down
    }
    else if (currentMDCLNumber == 1) // AMC
    {
      if (digitalRead(AMCPin) != HIGH) // Change the indicator LEDs if needed
      {
        digitalWrite(DCCPin, LOW);
        digitalWrite(AMCPin, HIGH);
      }
      {
        digitalWrite(DCCPin, LOW);
        digitalWrite(AMCPin, HIGH);
      }
      float AMC_REL = (2048 - m_p / 2)/2048.0;                        // This should not overflow.
      return percentAdjust * ((AMC_REL * one_plus_m_t)); // percentAdjust < 1, is to turn the power down
    }
  }
  return percentAdjust * one_plus_m_t; // If MDCL is off, just pass the word through adjusted for desired power output
}
