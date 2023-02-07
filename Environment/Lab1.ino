////////////////////////////
//
// V21.1 Skeleton.ino
// 
// Adapted to the physical and simulated environments
//
// 2022-12-17 Jens Andersson
//
////////////////////////////

//
// Select library
//
// uncomment for the physical environment
//#include <datacommlib.h>
//
// uncomment for the simulated environment
#include <datacommsimlib.h>

//
// Prototypes
//

//predefined
void l1_send(unsigned long l2frame, int framelen);
boolean l1_receive(int timeout);	
void l1_shift(unsigned long frame, int framelen, unsigned long mask);

//
// Runtime
//

//////////////////////////////////////////////////////////
//
// Add your global constant and variabel declaretions here
//

int state = NONE;
Shield sh; // note! no () since constructor takes no arguments
Transmit tx;
Receive rx;
int led_choice = 0; 
//////////////////////////////////////////////////////////

//
// Code
//
void setup() {
	sh.begin();

	//////////////////////////////////////////////////////////
	//
	// Add init code here
	//

	state = APP_PRODUCE;

	// Set your development node's address here

	//////////////////////////////////////////////////////////
}

void loop() {

	//////////////////////////////////////////////////////////
	//
	// State machine
	// Add code for the different states here
	//

	switch(state){

		case L1_SEND:
			Serial.println("[State] L1_SEND");
			l1_send(tx.frame, LEN_FRAME);
			state = L1_RECEIVE;

			break;

		case L1_RECEIVE:
			Serial.println("[State] L1_RECEIVE");
			l1_receive(20000);
			state = L2_FRAME_REC;
			break;

		case L2_DATA_SEND:
			Serial.println("[State] L2_DATA_SEND"); 
			tx.frame_payload = led_choice;
			tx.frame_generation();

			state = L1_SEND;

			break;

		case L2_RETRANSMIT:
			Serial.println("[State] L2_RETRANSMIT");
			// +++ add code here 

			// ---
			break;

		case L2_FRAME_REC:
			Serial.println("[State] L2_FRAME_REC");
			rx.frame_decompose();
			Serial.println(rx.frame_payload);
			state = APP_PRODUCE;
			break;

		case L2_ACK_SEND:
			Serial.println("[State] L2_ACK_SEND");
			// +++ add code here 

			// ---
			break;

		case L2_ACK_REC:
			Serial.println("[State] L2_ACK_REC");
			// +++ add code here

			// ---
			break;

		case APP_PRODUCE: 
			Serial.println("[State] APP_PRODUCE");
			
			led_choice = sh.select_led();
			Serial.println(led_choice);
			state = L2_DATA_SEND;

			// ---
			break;

		case APP_ACT:
			Serial.println("[State] APP_ACT");
			// +++ add code here 

			// ---
			break;

		case HALT:  
			Serial.println("[State] HALT");
			sh.halt();
			break;

		default:
			Serial.println("UNDEFINED STATE");
			break;
	}

	//////////////////////////////////////////////////////////

}
//////////////////////////////////////////////////////////
//
// Add code to the predfined functions
//
void l1_send(unsigned long frame, int framelen) {
	l1_shift(PREAMBLE_SEQ, LEN_PREAMBLE, 0x80);
	l1_shift(SFD_SEQ, LEN_SFD, 0x80);
	l1_shift(frame, framelen, 0x80000000);
	 


	//digitalWrite(PIN_TX, LOW);
}

void l1_shift(unsigned long frame, int framelen, unsigned long mask) {

	for (int i = 0; i<framelen; i++) {
		byte msb = ((frame << (framelen - (i+1))) & mask) == 0 ? 0 : 1;
		Serial.println(msb);
		digitalWrite(PIN_TX, msb);
		delay(T_S);
	}
}

boolean l1_receive(int timeout)
{
  long start_time = millis();

  while (!sh.sampleRecCh(PIN_RX))
  {
    if (millis() - start_time > timeout)
    {
      return false;
    }
  }

  byte buffer = 0;
  delay(T_S / 2);

  // check that we have recieved the SFD sequence
  while (buffer != SFD_SEQ)
  {
    if (millis() - start_time > timeout)
    {
      return false;
    }

    buffer = (buffer << 1) | sh.sampleRecCh(PIN_RX);
    delay(T_S);
  }

  // read the frame
  long frame = 0;

  for (int i = 0; i < LEN_FRAME; i++)
  {
    int rx_bit = sh.sampleRecCh(PIN_RX);
    frame = (frame << 1) | rx_bit;
    digitalWrite(DEB_1, rx_bit);
    delay(T_S);
  }

  rx.frame = frame;

  return true;
}

//////////////////////////////////////////////////////////
//
// Add your functions here
//
