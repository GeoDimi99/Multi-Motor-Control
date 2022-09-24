
#include "Motor.h"

// Definizione del comportamento del vettore dei interrupt, cosa deve fare l'ISR:



// Definizione delle funzioni riguardanti il motore:

Motor* Motor_init(float K_p, float K_i, float K_d){
	
	//Inizzializzazione di tools indispensabili per il motore
	digit_init();		// Controllo della direzione del motore
	pwm_init();			// Controllo generatore d'onda PWM
	encoder_init();		// Lettura dello stato corrente
	timer5_init(LOOP_TIMING);	// Controllo di eventi con intervalli di tempo regolari
	
	// Allocazione dell'oggetto Motor
	Motor* mtr = (Motor*) malloc(sizeof(Motor));
	
	// Inizializzazione dei parametri del controllore PID
	mtr->Kp = K_p;
	mtr->Ki = K_i;
	mtr->Kd = K_d;
	
	// Inizializzazione dell'intensità a zero
	mtr->u_d = 0;
	mtr->u_i = 0;
	mtr->u_p = 0;
	mtr->current_pwm = 0;
	mtr->error = 0; 
	
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
	uint16_t curr_pos = encoder_read();				// Lettura posizione corrente
	uint16_t prev_pos = mtr->angular_position;		// Lettura posizione precedente
    int16_t delta = curr_pos - prev_pos; 
	float curr_vel = delta / LOOP_TIMING;
	float des_vel = mtr->desired_velocity;
	
	float curr_err = des_vel - curr_vel;			// Calcolo dell'errore
	//printf("curr err %d\n", (int)curr_err);
	mtr->angular_position = curr_pos;				// Setto posizione attuale
	mtr->angular_velocity = curr_vel; 				// Setto velocita attuale
	
	
	uint8_t dir_pin = (mtr->direction) ? (1 << 4) : (1 << 5);
	
	
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
			mtr->u_d = (curr_err - mtr->error) / LOOP_TIMING; 	//Calcolo della componente derivativa
			mtr->integral_error += curr_err;						//Calcolo della componente integrativa
			mtr->u_p = curr_err;									//Calcolo della componente proporzionale
			 
			float u_tot = mtr->current_pwm;
			u_tot += mtr->u_d + mtr->integral_error + mtr->u_p; 						//Settiamo l'intensità della pwm corrente
			
			//Controllo del range dell'ingresso dell'onda quadra
			//if      (u_tot < 0.0f) mtr->current_pwm = 0;
			//else if (u_tot > 255.0f) mtr->current_pwm = 255;
			//else                     mtr->current_pwm = (int) u_tot;
			mtr->current_pwm = (int) u_tot ; 
			mtr->error = curr_err;
			 
			digit_write(dir_pin,1);							//Settiamo la direzione motore
			pwm_set_intensity(mtr->current_pwm);			//Settiamo l'intensitàd di velocita 
			 
			break;
	}
	
}

int16_t clamp(int16_t input_val, int16_t range_max, int16_t range_min){
	if(input_val > range_max) return range_max;
	else if(input_val < range_min) return range_min;
	else return input_val;
}

float get_Kp(Motor* mtr){ return mtr->Kp; }

float get_Kd(Motor* mtr){ return mtr->Kd; }

float get_Ki(Motor* mtr){ return mtr->Ki; }

uint16_t get_angular_position(Motor* mtr) {return mtr->angular_position; }

uint16_t get_angular_velocity(Motor* mtr) {return mtr->angular_velocity; }

uint16_t get_desired_velocity(Motor* mtr) {return mtr->desired_velocity; }

uint16_t get_current_pwm(Motor* mtr) {return mtr->current_pwm; }

float get_error(Motor* mtr) {return mtr->error; }

