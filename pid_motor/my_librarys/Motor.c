
#include "Motor.h"


float clamp(float input_val, float range_max){
	if(input_val > range_max) 		  
		return  range_max;
	else if(input_val < -range_max) 
		return -range_max;
	else 													  
		return input_val;
}

// Definizione delle funzioni riguardanti il motore:

Motor* Motor_init(float K_p, float K_i, float K_d){
	
	//Inizzializzazione di tools indispensabili per il motore
	pwm_init();			// Controllo generatore d'onda PWM
	encoder_init();		// Lettura dello stato corrente
	timer5_init(LOOP_TIMING);	// Controllo di eventi con intervalli di tempo regolari
	
	// Allocazione dell'oggetto Motor
	Motor* mtr = (Motor*) malloc(sizeof(Motor));
	
	// Inizializzazione dei parametri del controllore PID
	mtr->Kp = K_p;
	mtr->Ki = K_i;
	mtr->Kd = K_d;
	mtr->max_error = 20.f;
	mtr->max_error_integral = 50.f;

	// Inizializzazione di parametri 
	mtr->angular_velocity = 0;
	mtr->angular_position = 0;
	mtr->desired_velocity = 0;
	mtr->error = 0;
	mtr->dira = 4;
	mtr->dirb = 5;
	
	// Inizializzazione dell'intensità a zero
	mtr->current_pwm = 0;
	mtr->error_acc = 0;
	
	//Setto in ouput i registri per la direzione del motore
	DigIO_REGE_setDirection(mtr->dira,1);
	DigIO_REGE_setDirection(mtr->dirb,1);
	
	
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



void set_desired_velocity(Motor* mtr, uint16_t desired_velocity){
	
	//Imposto la velocità desiderata
	mtr->desired_velocity = desired_velocity;
	
}


void spin_once(Motor* mtr){
	uint16_t curr_pos = encoder_read();						// Lettura posizione corrente
	uint16_t prev_pos = mtr->angular_position;		// Lettura posizione precedente
	
	//Calcoliamo la velocità misurata
	int16_t curr_vel = curr_pos - prev_pos;
	int16_t des_vel = mtr->desired_velocity;
	
	
	mtr->angular_position = curr_pos;				// Setto posizione attuale
	mtr->angular_velocity = curr_vel; 				// Setto velocita attuale
	
	switch(mtr->type_controller){
		
			
		case CLOSE_LOOP:;
			// Caso : Controllore ad anello chiuso
			
			// Calcolo dell'errore e lo limitiamo in un intervallo per fare un ingresso a rampa
			// per evitare che il controllore sia instabile all'inizio. 
			float prev_err = mtr->error;
			float curr_err = curr_vel - des_vel;
				curr_err = clamp(curr_err, mtr->max_error);
				mtr->error = curr_err;
			
			//Calcolo dell'ingresso integrale, con un range massimo per anti wind up
			mtr->error_acc = clamp(mtr->error_acc + mtr->Ki * curr_err, mtr->max_error_integral);
			float u_i = mtr->error_acc;
			
			//Calcoliamo ingresso derivato4
			float u_d = mtr->Kd * (curr_err - prev_err);
			
			//Calcoliamo ingresso proporzionale
			float u_p = mtr->Kp * curr_err;
			
			// Limitiamo l' onda quadra tra +- 255
			mtr->current_pwm =(int16_t) clamp(mtr->current_pwm + u_i + u_d + u_p, 255.); //Settiamo l'intensità della pwm corrente 
			
			break;
			
			 
		case OPEN_LOOP:
			// Caso : Controllore ad anello aperto
			mtr->current_pwm = (int16_t) des_vel;
												
			break;	
	}
	
	//  Controlo comand per il controllo dell'hardware
	if(mtr->current_pwm < 0){
		DigIO_REGE_setValue(mtr->dira, 1);
		DigIO_REGE_setValue(mtr->dirb, 0);
	}
	else{
		DigIO_REGE_setValue(mtr->dira, 0);
		DigIO_REGE_setValue(mtr->dirb, 1);
	}
	
	pwm_set_intensity(abs(mtr->current_pwm));
}




