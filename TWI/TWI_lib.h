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
volatile int RB_Index;                               //indicizza il buffer di ricezione --> può cambiare ad ogni istante
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
	Slave_Receiver
} TWI_mode;

//struttura dati info TWI

typedef struct TWI_info_struct{
	volatile TWI_mode mode;
	uint8_t error_code;
	uint8_t repeated_start;
}TWI_info_struct;

TWI_info_struct TWI_info;

//macros per il controllo
#define TWI_Set_Address() (TWCR= (0<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(1<<TWIE))

/*
 * TWI_Set_Address() [TWCR=01000101]:
 *  [7] TWINT=0 NON si è finito di lavorare in background e NON ci si aspetta un'interrupt
 *  [6] TWEA=1 abilitato il controllo per l'ACK
 *  [5] TWSTA=0 NON si vuole trasmettere una condizione di START
 *  [4] TWSTO=0 NON si vuole trasmettere una condizione di STOP
 *  [3] TWWC=0 non è avvenuto un tentativo illegale di scrittura (write collision)
 *  [2] TWEN=1 abilita TWI, che prende il controllo dei pins SCL ed SDA
 *  [1] 0 bit inutilizzato
 *  [0] TWIE=1 abilita le interruzioni e la loro gestione passando per l'interrupt vector
*/

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

//Definizione status code
#define TWI_SUCCESS                  0xFF                //trasmissione avvenuta con successo
#define TWI_NO_RELEVANT_INFO         0xF8                //non ci sono informazioni rilevanti
#define TWI_ILLEGAL_START_STOP       0x00                //condizione di START/STOP ricevuta illegalmente

//MASTER TRASMITTER AND MASTER RECEIVER COMMON//

#define START_TRANSMITTED            0x08                //è stata trasmessa unacondizione di START
#define REP_START_TRANSMITTED        0x10                //è stata trasmessa una REPEATED START
#define ARBITRATION_LOST             0x38                //arbitrato perso nel SLA+W o nell'invio di un dato (se MT)
														 //arbitrato perso nel SLA+R o nell'invio del NACK   (se MR)

//MASTER TRANSMITTER//
#define SLAW_TR_ACK_RV               0x18                //SLA+W è stato trasmesso, si è ricevuto un ACK  
#define SLAW_TR_NACK_RV              0X20                //SLA+W è stato trasmesso, si è ricevuto un NACK 
#define M_DATA_TR_ACK_RV             0x28                //un dato è stato trasmesso, si è ricevuto un ACK 
#define M_DATA_TR_NACK_RV            0x30                //un dato è stato trasmesso, si è ricevuto un NACK 

//MASTER RECEIVER//
#define SLAR_TR_ACK_RV               0x40                //SLA+R è stato trasmesso, si è ricevuto un ACK 
#define SLAR_TR_NACK_RV              0x48                //SLA+R è stato trasmesso, si è ricevuto un NACK 
#define DATA_RV_ACK_TR               0x50                //si è ricevuto un dato, è stato trasmesso un ACK
#define DATA_RV_NACK_TR              0x58                //si è ricevuto un dato, è stato trasmesso un NACK


//SLAVE RECEIVER//
#define SLAW_RV_ACK_TR               0x60                //si è ricevuto il proprio SLA + W, è stato trasmesso un ACK
#define BRDW_RV_ACK_TR               0x70                //si è ricevuto l'indirizzo broadcast, è stato trsamesso un ACK
#define DATA_SLA_RV_ACK_TR           0x80                //si è ricevuto un dato, precendentemente indirizzato con SLA, è stato trasmesso un ACK
#define DATA_SLA_RV_NACK_TR          0x88                //si è ricevuto un dato, precendentemente indirizzato con SLA, è stato trasmesso un NACK
#define DATA_BRD_RV_ACK_TR           0x90                //si è ricevuto un dato in broadcast, è stato trasmesso un ACK
#define DATA_BRD_RV_NACK_TR          0x98                //si è ricevuto un dato in broadcast, è stato trasmesso un NACK
#define START_STOP_FOR_SLAVE         0xA0                //si è ricevuta una condizione di STOP o di REPEATED START come slave
#define ARBITRATION_LOST_SR_ADDR     0x68                //il master ha perso l'arbitrato, precendentemente indirizzato con SLA
#define ARBITRATION_LOST_SR_BRD      0x78                //il master ha perso l'arbitrato, precendentemente indirizzato con broadcast

//SLAVE TRANSMITTER//
#define SLAR_RV_ACK_TR               0xA8                //si è ricevuto il proprio SLA + R, è stato trasmesso un ACK
#define S_DATA_TR_ACK_RV             0xB8                //un dato è stato trasmesso, si è ricevuto un ACK
#define S_DATA_TR_NACK_RV            0xC0                //un dato è stato trasmesso, si è ricevuto un NACK
#define ARBITRATION_LOST_ST          0xB0                //il master ha perso l'arbitrato
#define S_LDATA_TR_ACK_RV            0xC8                //l'ultimo dato è stato trasmesso,si è ricevuto un ACK

//dichiarazione delle funzioni
uint8_t is_TWI_ready(void);
void TWI_Init(void);
void Slave_Addr_init(uint8_t SL_addr, uint8_t brd);
uint8_t TWI_Transmit_Data(void *const TR_data, uint8_t data_len, uint8_t repeated_start);
uint8_t TWI_Read_Data(uint8_t SL_addr, uint8_t bytes_to_read, uint8_t repeated_start);
uint8_t TWI_Slave_Transmit_Data(void *const TR_data, uint8_t data_len);
uint8_t TWI_Slave_Receive_Data();

#endif


