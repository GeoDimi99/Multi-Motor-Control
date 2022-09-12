#ifndef TWILIB_H_
#define TWILIB_H_

#define TWI_FREQ 100000 //frequenza SCL TWI
#define TWI_STATUS	(TWSR & 0xF8) //prende dal 3° al 7° bit dello stato

//buffer globali di trasmissione e ricezione
#define TRANSMIT_BUFLEN 20
#define RECEIVE_BUFLEN 20
uint8_t Transmit_Buffer[TRANSMIT_BUFLEN];
volatile uint8_t Receive_Buffer[RECEIVE_BUFLEN];     //volatile-->può cambiare ad ogni istante
volatile int TB_Index;                               //indicizza il buffer di trasmissione--> può cambiare ad ogni istante
int RB_Index;                                        //indicizza il buffer di ricezione
int transmit_len;                                    //Lunghezza della trasmissione (minore di TRANSMIT_BUFLEN)
int receive_len;                                     //Lunghezza della ricezione (minore di RECEIVE_BUFLEN)

//modalità operative TWI
typedef enum {
	Ready,
	Initializing,
	Repeated_Start,
	Master_Transmitter,
	Master_Receiver,
	Slave_Transmitter,
	Slave_Reciever
} TWI_mode;

//struttura dati info TWI

typedef struct TWI_info{
	TWI_mode mode;
	uint8_t error_code;
	uint8_t repeated_start;
}TWI_info;

//macros per il controllo

#define TWI_Send_Start() (TWCR = (1<<TWINT)|(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Send_Start() [TWCR=10100101]:
 *  [7] TWINT=1 si è finito di lavorare in background e ci si aspetta un'interrupt
 *  [6] TWEA=0 disabilitato il controllo per l'ACK
 *  [5] TWSTA=1 si vuole trasmettere una condizione di START
 *  [4] TWSTO=0 NON si vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

#define TWI_Send_Stop() (TWCR = (1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Send_Stop() [TWCR=10010101]:
 *  [7] TWINT=1 si è finito di lavorare in background e ci si aspetta un'interrupt
 *  [6] TWEA=0 disabilitato il controllo per l'ACK
 *  [5] TWSTA=0 NON si vuole trasmettere una condizione di START
 *  [4] TWSTO=1 si vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

#define TWI_Send_Transmit() (TWCR = (1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Send_Transmit() [TWCR=10000101]:
 *  [7] TWINT=1 si è finito di lavorare in background e ci si aspetta un'interrupt
 *  [6] TWEA=0 disabilitato il controllo per l'ACK
 *  [5] TWSTA=0 NON si vuole trasmettere una condizione di START
 *  [4] TWSTO=0 NON vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

#define TWI_Send_ACK()  (TWCR = (1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Send_ACK() [TWCR=11000101]:
 *  [7] TWINT=1 si è finito di lavorare in background e ci si aspetta un'interrupt
 *  [6] TWEA=1 abilitato il controllo per l'ACK
 *  [5] TWSTA=0 NON si vuole trasmettere una condizione di START
 *  [4] TWSTO=0 NON vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

#define TWI_Send_NACK() (TWCR = (1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Send_NACK() [TWCR=10000101]:
 *  [7] TWINT=1 si è finito di lavorare in background e ci si aspetta un'interrupt
 *  [6] TWEA=0 disabilitato il controllo per l'ACK
 *  [5] TWSTA=0 NON si vuole trasmettere una condizione di START
 *  [4] TWSTO=0 NON vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

//Definizione dei codici di errore
#define TWI_M_LOST_ARBIT		0x38     // Il controllo di TWI è stato perso in SLA+W/R oppure nella trasmissione dei dati
#define TWI_SR_LOST_ARBIT       0x68     // Il controllo di TWI è stato perso dal master in SLA+W/R; ricevuto SLA+W, inviato un ACK
#define TWI_SR_BR_LOST_ARBIT    0x78     // Il controllo di TWI è stato perso dal master in SLA+W/R; ricevuto indirizzo broadcast +W, inviato un ACK
#define TWI_ST_LOST_ARBIT       0xB0     // Il controllo di TWI è stato perso dal master in SLA+W/R; ricevuto SLA+R, inviato un ACK
#define TWI_NO_RELEVANT_INFO	0xF8     // Non ci sono informazioni rilevanti
#define TWI_ILLEGAL_START_STOP	0x00     // Rilevata condizione di START/STOP illegale
#define TWI_SUCCESS				0xFF     // Trasferimento avvenuto con successo

//TWI STATUS CODE PER ISR
#define TWI_START_SENT			0x08 	// Inviata condizione di start 
#define TWI_REP_START_SENT		0x10 	// Inviata repeated start

//master trasmitter
#define TWI_MT_SLAW_ACK			0x18 	// Inviato indirizzo dello slave + W bit, ricevuto un ACK
#define TWI_MT_SLAW_NACK		0x20 	// Inviato indirizzo dello slave + W bit, ricevuto un NACK
#define TWI_MT_DATA_ACK			0x28 	// Inviato un dato e ricevuto un ACK
#define TWI_MT_DATA_NACK		0x30 	// Inviato un dato e ricevuto un NACK 

//master receiver
#define TWI_MR_SLAR_ACK			0x40	// Inviato indirizzo dello slave + R bit, ricevuto un ACK
#define TWI_MR_SLAR_NACK		0x48 	// Inviato indirizzo dello slave + R bit, ricevuto un NACK
#define TWI_MR_DATA_ACK			0x50 	// Ricevuto un dato e inviato un ACK
#define TWI_MR_DATA_NACK		0x58 	// Ricevuto un dato e inviato un NACK

//slave trasmitter
#define TWI_ST_SLAW_ACK			0xA8 	// Ricevuto indirizzo dello slave + R bit, inviato un ACK
#define TWI_ST_SLAW_NACK		0x20 	// NON icevuto indirizzo dello slave + W bit, invio un NACK
#define TWI_ST_DATA_ACK			0xB8 	// Inviato un dato e ricevuto un ACK
#define TWI_ST_DATA_NACK		0xC0 	// Inviato un dato e ricevuto un NACK 
#define TWI_ST_LAST_DATA_ACK	0xC8 	// Inviato l'ultimo dato e ricevuto un ACK

//slave receiver
#define TWI_SR_SLAW_ACK			0x60 	// Ricevuto indirizzo dello slave + W bit, inviato un ACK
#define TWI_SR_BRAW_ACK			0x70 	// Ricevuto indirizzo broadcast + W bit, inviato un ACK
#define TWI_SR_DATA_ACK			0x80 	// Ricevuto un dato, precedentemente ricevuto SLA+W, inviato un ACK
#define TWI_SR_BR_DATA_ACK		0x90 	// Ricevuto un dato, precedentemente ricevuto indirizzo broadcast, inviato un ACK
#define TWI_SR_DATA_NACK		0x88 	// Ricevuto un dato, precedentemente ricevuto SLA+W, inviato un NACK
#define TWI_SR_BR_DATA_NACK		0x98 	// Ricevuto un dato, precedentemente ricevuto indirizzo broadcast, inviato un NACK
#define TWI_SR_R_START_STOP		0xA0 	// Uno stop o una repeated start è stata ricevuta mentre è ancora indirizzato come slave


//dichiarazione delle funzioni
uint8_t is_TWI_ready(void);
void TWI_Init(void);
uint8_t TWI_Transmit_Data(void *const TR_data, uint8_t data_len, uint8_t repeated_start);
uint8_t TWI_Read_Data(uint8_t TWI_addr, uint8_t bytes_to_read, uint8_t repeated_start);

#endif


