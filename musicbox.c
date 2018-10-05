asm(" .length 10000");
asm(" .width 132");
// Oct 2015
// TimerTone0 == produces a 1 Khz tone using TimerA, Channel 0
// Toggled on and off with a button
// Using the timer in up mode with NO INTERRUPT
//
// Sound is turned on and off by directly manipulating
// the TACCTL0 register.  The half period is not dynamically
// updated (though it can be changed in the debugger by
// changing TACCR0.
// The pushbutton is not debounced in any way!

#include "msp430g2553.h"
//-----------------------
// The following definitions allow us to adjust some of the port properties
// for the example:

// define the bit mask (within P1) corresponding to output TA0
#define SPEAKER 0x02

// define the location for the button (this is the built in button)
// specific bit for the button
#define BUTTON_BIT 0x08
#define TEMPOUP_BUTTON 0x04
#define TEMPODOWN_BUTTON 0x80
#define RESTART_BUTTON 0x20
#define SWITCH_SONG_BUTTON 0x10
//-------CREATING OUR NOTES-------------
#define R      (1)//cant hear this frequency so it is used as our rest
#define C4     (1911)
#define C4s    (1803)
#define D4     (1703)
#define E4b    (1607)
#define E4     (1517)
#define F4     (1432)
#define F4s    (1351)
#define G4     (1276)
#define G4s    (1204)
#define A4     (1136)
#define B4b    (1073)
#define B4     (1012)
#define C5     (956)
#define C5s    (902)
#define D5     (851)
#define E5b    (803)
#define E5     (758)
#define F5     (716)
#define F5s    (676)
#define G5     (638)
#define G5s    (602)
#define A5     (568)
#define B5b    (536)
#define B5     (506)
#define C6     (478)
//-----------------------------

//********CREATING OUR NOTE LENGTH********
#define B 1//break between identical notes
#define S 8//sixteenth note
#define E 16//eigth
#define DE 24//dotted eigth
#define Q 32//quarter
#define DQ 48//dotted quarter
#define H 64//half
#define DH 96//dotted half
#define F 128//full note
//****************************************


//***********GLOBAL VARIABLES*********************
unsigned int note_length_counter=0;
unsigned int note_counter=0;
float tempo=2.25;
unsigned char state='A';//A=song1, B=song2, C=nothing
//**************************************************

void init_timer(void); // routine to setup the timer
//*****NOTES AND REST IN ORDER FOR JOY TO THE WORLD*****************
#define JTW_LENGTH 72//length of joy to the world arrays
unsigned const int jtw_notes[JTW_LENGTH]={
		 C5,
		 B4,
		 A4,
		 G4,
		 F4,
		 E4,
		 D4,
		 C4,
		 G4,
		 A4,
		 R,//REST
		 A4,
		 B4,
		 R,//REST
		 B4,
		 C5,
		 R,//REST
		 C5,
		 R,//REST
		 C5,
		 B4,
		 A4,
		 G4,
		 R,//REST
		 G4,
		 F4,
		 E4,
		 C5,
		 R,//REST
		 C5,
		 B4,
		 A4,
		 G4,
		 R,//REST
		 G4,
		 F4,
		 E4,
		 R,//REST
		 E4,
		 R,//REST
		 E4,
		 R,//REST
		 E4,
		 R,//REST
		 E4,
		 R,//REST
		 E4,
		 F4,
		 G4,
		 F4,
		 E4,
		 D4,
		 R,//REST
		 D4,
		 R,//REST
		 D4,
		 R,//REST
		 D4,
		 E4,
		 F4,
		 E4,
		 D4,
		 E4,
		 C5,
		 A4,
		 G4,
		 F4,
		 E4,
		 F4,
		 E4,
		 D4,
		 C4
};
// ******************************************************

//******LENGTH OF NOTES FOR JOY TO THE WORLD IN ORDER*****
unsigned const char jtw_notelengths[JTW_LENGTH]={
		Q,
		DE,
		S,
		DQ,
		E,
		Q,
		Q,
		DQ,
		E,
		DQ,
		B,
		E,
		DQ,
		B,
		E,
		DQ,
		B,
		E,
		B,
		E,
		E,
		E,
		E,
		B,
		DE,
		S,
		E,
		E,
		B,
		E,
		E,
		E,
		E,
		B,
		DE,
		S,
		E,
		B,
		E,
		B,
		E,
		B,
		E,
		B,
		E,
		B,
		S,
		S,
		DQ,
		S,
		S,
		E,
		B,
		E,
		B,
		E,
		B,
		S,
		S,
		DQ,
		S,
		S,
		E,
		Q,
		E,
		DE,
		S,
		E,
		E,
		Q,
		Q,
		H
};
//********************************************************

//************FAKE SONG*******************************
#define FAKESONG_LENGTH 73
unsigned volatile int fakesong_notes[FAKESONG_LENGTH]={
		B4,
		R,
		B4,
		R,
		B4,
		R,
		B4,
		R,
		B4,
		R,
		B4,
		F4,
		C5,
		B4,
		F4,
		G5,
		A5,
		B5,
		C4,
		D5,
		B4,
		F5,
		F4,
		F5,
		F4,
		F5,
		F4,
		A4,
		A5,
		C4,
		C5,
		E4,
		E5,
		G5,
		B4,
		C4,
		D4,
		E4,
		F4,
		G4,
		A5,
		B5,
		C5,
		D5,
		E5,
		F5,
		G4,
		C4,
		C5,
		G4,
		G5,
		B4,
		B5,
		C4,
		C5,
		D4,
		D5,
		G4,
		G5,
		F4,
		F5,
		D4,
		F4,
		G4,
		A4,
		A5,
		B5,
		B4,
		C4,
		C5,
		G4,
		F4,
		F5

};
//****************************************************

//*********FAKE SONG NOTE LENGTH**********************
unsigned volatile char fakesong_note_length[FAKESONG_LENGTH]={
		E,
		B,
		E,
		B,
		E,
		B,
		E,
		B,
		E,
		B,
		DQ,
		Q,
		H,
		H,
		E,
		E,
		E,
		E,
		E,
		E,
		E,
		E,
		DE,
		Q,
		Q,
		DQ,
		H,
		E,
		E,
		DE,
		DE,
		S,
		S,
		S,
		S,
		DE,
		Q,
		Q,
		Q,
		H,
		S,
		S,
		S,
		S,
		S,
		E,
		E,
		S,
		E,
		S,
		S,
		S,
		S,
		S,
		S,
		S,
		DQ,
		Q,
		Q,
		Q,
		DQ,
		E,
		E,
		E,
		E,
		DE,
		DE,
		DE,
		E,
		E,
		S,
		S,
		Q

};
//***************************************************

//***********************************
void restart(void);
void init_button_and_speaker(void);
void init_timer(void);
void raise_tempo(void);
void lower_tempo(void);
void song_switch(void);
//***********************************
void main(){
	WDTCTL = (WDTPW + WDTTMSEL+WDTCNTCL+0+1);
	IE1 |= WDTIE;
	BCSCTL1 = CALBC1_1MHZ;    // 1Mhz calibration for clock
	DCOCTL  = CALDCO_1MHZ;

	init_timer();  // initialize timer
	init_button_and_speaker(); // initialize the button and speaker
	restart();
	_bis_SR_register(GIE+LPM0_bits);// enable general interrupts and power down CPU

}

//*****************************
// Sound Production System
void init_timer(){              // initialization and start of timer
	TA0CTL |= TACLR;            // reset clock
	TA0CTL = TASSEL_2+ID_0+MC_1;// clock source = SMCLK
	                            // clock divider=1
	                            // UP mode
	                            // timer A interrupt off
	TA0CCTL0=0; // compare mode, output mode 0, no interrupt enabled

}

// *****************************
// Button input System
// Button toggles the state of the sound (on or off)
// action will be interrupt driven on a downgoing signal on the pin
// no debouncing (to see how this goes)

void init_button_and_speaker(){
// All GPIO's are already inputs if we are coming in after a reset

	//PAUSE/PLAY BUTTON
	P1OUT |= BUTTON_BIT; // pullup
	P1REN |= BUTTON_BIT; // enable resistor
	P1IES |= BUTTON_BIT; // set for 1->0 transition
	P1IFG &= ~BUTTON_BIT;// clear interrupt flag
	P1IE  |= BUTTON_BIT;// enable interrupt

	//INCREASE TEMPO BUTTON
	P1OUT |= TEMPOUP_BUTTON; // pullup
	P1REN |= TEMPOUP_BUTTON; // enable resistor
	P1IES |= TEMPOUP_BUTTON; // set for 1->0 transition
	P1IFG &= ~TEMPOUP_BUTTON;// clear interrupt flag
	P1IE  |= TEMPOUP_BUTTON;// enable interrupt

	//DECREASE TEMPO BUTTON
	P1OUT |= TEMPODOWN_BUTTON; // pullup
	P1REN |= TEMPODOWN_BUTTON; // enable resistor
	P1IES |= TEMPODOWN_BUTTON; // set for 1->0 transition
	P1IFG &= ~TEMPODOWN_BUTTON;// clear interrupt flag
	P1IE  |= TEMPODOWN_BUTTON;// enable interrupt

	//RESTART SONG BUTTON
	P1OUT |= RESTART_BUTTON; // pullup
	P1REN |= RESTART_BUTTON; // enable resistor
	P1IES |= RESTART_BUTTON; // set for 1->0 transition
	P1IFG &= ~RESTART_BUTTON;// clear interrupt flag
	P1IE  |= RESTART_BUTTON;// enable interrupt

	//SWITCH SONG BUTTON
	P1OUT |= SWITCH_SONG_BUTTON; // pullup
	P1REN |= SWITCH_SONG_BUTTON; // enable resistor
	P1IES |= SWITCH_SONG_BUTTON; // set for 1->0 transition
	P1IFG &= ~SWITCH_SONG_BUTTON;// clear interrupt flag
	P1IE  |= SWITCH_SONG_BUTTON;// enable interrupt

	//INITIALIZE SPEAKER
	P1SEL|=SPEAKER; // connect timer output to pin
	P1DIR|=SPEAKER;
}
void restart(){//restart song...
	TA0CTL|=TACLR;
	TACCTL0 &=~OUTMOD_4;
	note_length_counter=0;
	note_counter=0;
	TA0CCR0=jtw_notes[0];
}
void lower_tempo(){//lower the speed of the song
	tempo+=.25;
}
void raise_tempo(){//increase the speed of the song
if(tempo>.25){
		tempo-=.25;
}

}
void playsong2(){//play the fake song
	if(FAKESONG_LENGTH>note_counter){
			  		if(note_length_counter<(fakesong_note_length[note_counter])*tempo){//if you have not finished the note
			  			note_length_counter++;//counting the note length
			  		}
			  		else{
			  			note_counter++;//move to the next note in the song
			  			TA0CCR0= fakesong_notes[note_counter];//play the note the note_counter indexes
			  			note_length_counter=0;//set to the start of the note
			  		}
			  	}
			  	else{
			  		TACCTL0 &=~OUTMOD_4;//stop playing if song is finished
			  	}
}
void playsong1(){//play joy to the world
	if(JTW_LENGTH>note_counter){
	  		if(note_length_counter<(jtw_notelengths[note_counter])*tempo){//if you have not finished the note
	  			note_length_counter++;//counting the note length
	  		}
	  		else{
	  			note_counter++;//move to the next note in the song
	  			TA0CCR0= jtw_notes[note_counter];//play the note the note_counter indexes
	  			note_length_counter=0;//set to the start of the note
	  		}
	  	}
	  	else{
	  		TACCTL0 &=~OUTMOD_4;//stop playing if song is finished
	  	}
}

void song_switch(){
	if(state=='A'){
		state='B';
		note_counter=0;
		note_length_counter=0;
	}
	else if (state=='B'){
		state='A';
		note_counter=0;
		note_length_counter=0;
	}
	else{
		TACCTL0 &=~OUTMOD_4;
	}
}

interrupt void button_handler(){
// check interrupt comes from the desired bit...
// (if not, just ignore -- cannot happen in this case)
	if (P1IFG & BUTTON_BIT){
		P1IFG &= ~BUTTON_BIT; // reset the interrupt flag
		TACCTL0 ^= OUTMOD_4; // toggle outmod between 0 and 4 (toggle)
	}
	else if (P1IFG & TEMPOUP_BUTTON){
		P1IFG &= ~TEMPOUP_BUTTON;
		raise_tempo();//RAISE TEMPO BUTTON
	}
	else if (P1IFG & TEMPODOWN_BUTTON){
			P1IFG &= ~TEMPODOWN_BUTTON;
			lower_tempo();//LOWER TEMPO BUTTON
		}
	else if (P1IFG & RESTART_BUTTON){
			P1IFG &= ~RESTART_BUTTON;
			restart();//RESTART SONG
		}
	else if (P1IFG & SWITCH_SONG_BUTTON){
			P1IFG &= ~SWITCH_SONG_BUTTON;
			song_switch();//SWITCHSONG
		}
}
ISR_VECTOR(button_handler,".int02") // declare interrupt vector
// +++++++++++++++++++++++++++
// ===== Watchdog Timer Interrupt Handler =====
// This event handler is called to handle the watchdog timer interrupt,
//    which is occurring regularly at intervals of about 8K/1.1MHz ~= 7.4ms.

interrupt void WDT_interval_handler(){
	switch(state){
	case 'A':
		playsong1();
		break;
	case 'B':
		playsong2();
		break;
	case 'C':
		TACCTL0 &=~ OUTMOD_4;
		break;
	default:
		break;
	}

}
// DECLARE function WDT_interval_handler as handler for interrupt 10
// using a macro defined in the msp430g2553.h include file
ISR_VECTOR(WDT_interval_handler, ".int10")
