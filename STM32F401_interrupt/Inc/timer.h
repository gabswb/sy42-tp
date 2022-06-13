/*
 * timer.h
 *
 *  Created on: 4 avr. 2022
 *      Author: gschwab
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "stm32f401xe.h"
#include "stm32f4xx.h"
#include <stdint.h>
#include "pwm.h"
#include "usart.h"

/**clock en sortie du prescaler réglée à 10kHz**/
#define CK_CNT_F 10000

/*index of the luminosity of the led*/
int led_luminsosity;


char buffer[256];//stock la donnée qui contient le temps d'attente mesuré
float duration;//durée d'appuie sur le bouton mesurée
uint16_t begin;//début de l'appui (front descendant)
uint16_t end;//fin de l'appui (front montant)
uint16_t nb_overflows;//compte le nombre d'overflows du timer pendant la durée de la pression sur le bouton
int tim_freq;//frequence du timer


/**
 * Initialise le TIM3 pour générer des requêtes d’interruption toutes les ms milliseconde
 */
void TIM3_set_periodic_IRQ(uint32_t ms);

/**
 * Routine de traitement des interruptions du tim3 appelée à chaque interruption
 */
void TIM3_IRQHandler(void);

/**
 * Initialise le TIM4 pour générer des requêtes d’interruption toutes les ms milliseconde
 */
void TIM4_set_periodic_IRQ(uint32_t ms);

/**
 * Routine de traitement des interruptions du TIM4 appelée à chaque interruption
 */
void TIM4_IRQHandler(void);

/**
 * Calcul et renvoie la durée de la pression sur le bouton en seconde
 */
float compute_duration(uint16_t begin, uint16_t end, uint16_t nb_overflows, int arr, int periode);

/**
 * Initialise le TIM passé en paramètre à une fréquence de ms secondes
 */
void TIM_set_periodic_event(uint32_t ms, TIM_TypeDef* tim);

#endif /* TIMER_H_ */
