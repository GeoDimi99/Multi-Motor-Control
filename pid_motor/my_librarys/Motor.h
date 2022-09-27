
#pragma once

#include <stdlib.h>
#include "digital_io.h"
#include "pwm_signal.h"
#include "encoder.h"
#include "timer.h"

#define LOOP_TIMING 100


typedef enum {OPEN_LOOP, CLOSE_LOOP} ctrtype_t;



typedef struct{
	
	uint8_t dira, dirb;
	
	// Variabili di stato
	int16_t angular_position;
	int16_t angular_velocity;
	int16_t desired_velocity;
	int16_t current_pwm;
	
	
	
	// Variabili di controllo
	float error;
	float error_acc;

	
	// Variabile scelta tipo controllore
	ctrtype_t type_controller;
	float Kp;
	float Ki;
	float Kd;
	float max_error;
	float max_error_integral;
	
} Motor;


// Motor_init : inizializzazione del motore e dei vari tools necessari. Riceve come parmetri:
// - Kp = Costante proporzionale del controllore PID
// - Ki = Costante integrativa del controllore PID
// - Kd = Costante derivata del controllore PID
Motor* Motor_init(float K_p, float K_i, float K_d);


// Motor_detach: rilascio dell'oggetto motore allocato precedentemente
void Motor_detach(Motor* mtr);

// set_type_controller: setta il tipo di controllore, può assumere i valori:
// - OPEN_LOOP: controllore ad anello aperto
// - CLOSE_LOOP: controllore ad anello chiuso PID
void set_type_controller(Motor* mtr, ctrtype_t type_controller);

// set_desired_velocity: setta la velocità desiderata e la direzione in particolare assume i parametri:
// - desired_velocity : che può assumere diversi valori in base alla alla modalità del controllore:
// 			--- OPEN_LOOP : si regola l'intensità da 0 a 255;
//		 	--- CLOSE_LOOP: si regola la velocità da 0 a 390;
void set_desired_velocity(Motor* mtr, uint16_t desired_velocity);


// spin_once: funzione che esegue i vari calcoli di controllo e va inserita nell' ISR
void spin_once(Motor* mtr);


