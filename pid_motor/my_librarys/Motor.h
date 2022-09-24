
#pragma once

#include <stdlib.h>
#include "digital_io.h"
#include "pwm_signal.h"
#include "encoder.h"
#include "timer.h"

#define LOOP_TIMING 100


typedef enum {OPEN_LOOP, CLOSE_LOOP} ctrtype_t;

typedef enum {RIGHT, LEFT} dir_t;

typedef struct{
	// Variabili di stato
	uint16_t angular_position;
	uint16_t angular_velocity;
	uint16_t desired_velocity;
	uint8_t current_pwm;
	float error;
	dir_t direction;
	float integral_error;
	
	// Variabili di controllo
	float Kp;
	float Ki;
	float Kd;
	float u_p;
	float u_i;
	float u_d;
	
	// Variabile scelta tipo controllore
	ctrtype_t type_controller;
	
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
//		 	--- CLOSE_LOOP: si regola la velocità da 0 a 300;
// - direction: che può assumere i valori:
//			--- LEFT : gira a sinistra 
//			--- RIGHT: gira a destra
void set_desired_velocity(Motor* mtr, uint16_t desired_velocity, dir_t direction);


// spin_once: funzione che esegue i vari calcoli di controllo e va inserita nell' ISR
void spin_once(Motor* mtr);



// Metodi comuni per poter leggere i campi del Motore
float get_Kp(Motor* mtr);
float get_Kd(Motor* mtr);
float get_Ki(Motor* mtr);
uint16_t get_angular_position(Motor* mtr);
uint16_t get_angular_velocity(Motor* mtr);
uint16_t get_desired_velocity(Motor* mtr);
uint16_t get_current_pwm(Motor* mtr);
float get_error(Motor* mtr);
