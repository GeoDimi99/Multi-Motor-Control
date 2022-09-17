
#include "Motor.h"



Motor* Motor_init(float K_p, float K_i, float K_d){
	
	//Inizzializzazione di tools indispensabili per il motore
	digit_init();		// Controllo della direzione del motore
	pwm_init();			// Controllo generatore d'onda PWM
	encoder_init();		// Lettura dello stato corrente
	timer5_init(10);	// Controllo di eventi con intervalli di tempo regolari
	
	// Allocazione dell'oggetto Motor
	Motor* mtr = (Motor*) malloc(sizeof(Motor));
	
	// Inizializzazione dei parametri del controllore PID
	mtr->Kp = K_p;
	mtr->Ki = K_i;
	mtr->Kd = K_d;
	
	return mtr;
	
}


void Motor_detach(Motor* mtr){
	
	//Rilascio lo spazio dedicato al motore; 
	free(mtr);
}



void set_type_controller(Motor* mtr, ctrtype_t type_controller){
	
	//Setto il tipo di controller
	mtr->type_controller = type_controller;
}



void set_desired_velocity(Motor* mtr, uint16_t desired_velocity, dir_t direction){
	
	//Controllo del verso della velocità
	mtr->direction = direction;
	
	//Imposto la velocità desiderata
	mtr->desired_velocity = desired_velocity;
	
}

void spin_once(Motor* mtr){
	printf("entriamo1 con questo tipo %d\n",mtr->type_controller);
	uint16_t curr_pos = encoder_read();				// Lettura posizione corrente
	uint16_t prev_pos = mtr->angular_position;		// Lettura posizione precedente
	
	uint16_t curr_vel = curr_pos - prev_pos;
	
	mtr-> angular_position = curr_pos;				// Setto posizione attuale
	mtr->angular_velocity = curr_vel; 				// Setto velocita attuale
	
	printf("direzione %d\n",mtr->direction);
	uint8_t dir_pin = (mtr->direction) ? (1 << 4) : (1 << 5);
	uint16_t des_vel = mtr->desired_velocity;
	
	switch(mtr->type_controller){
		case OPEN_LOOP:
			// Caso : Controllore ad anello aperto
			digit_write(dir_pin,1);					//Settiamo la direzione
			pwm_set_intensity(des_vel);				//Settiamo l'intensità di velocita
			mtr->current_pwm = des_vel;				//Settiamo la pwm come l'intensita di velocita desiderata
			mtr->error = 0;							//Imponiamo l'errore pari a zero
			break;
		case CLOSE_LOOP:
			// Caso : Controllore ad anello chiuso
			//TODO
			break;
	}
	
}


float get_Kp(Motor* mtr){ return mtr->Kp; }

float get_Kd(Motor* mtr){ return mtr->Kd; }

float get_Ki(Motor* mtr){ return mtr->Ki; }

uint16_t get_angular_position(Motor* mtr) {return mtr->angular_position; }

uint16_t get_angular_velocity(Motor* mtr) {return mtr->angular_velocity; }

uint16_t get_desired_velocity(Motor* mtr) {return mtr->angular_velocity; }

uint16_t get_current_pwm(Motor* mtr) {return mtr->current_pwm; }

float get_error(Motor* mtr) {return mtr->error; }

