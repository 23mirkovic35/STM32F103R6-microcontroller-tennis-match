/*
 * display.c
 *
 *  Created on: Jan 31, 2022
 *      Author: Miroslav
 */




#include "display.h"

uint8_t seven_seg[]=
{ 0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84 };

extern uint32_t volatile state;
extern uint16_t volatile player1_point;
extern uint16_t volatile player2_point;
extern uint8_t volatile player1_gem;
extern uint8_t volatile player2_gem;

void display(){
	HAL_TIM_Base_Start_IT(&htim1);
}

uint16_t display_number[4] = { 0, 0, 0, 0 };
uint16_t curr = 0;

uint32_t volatile counter = 0;
uint32_t volatile timer_sec = 0;
uint32_t volatile timer_minut = 0;

#define ARR 9
uint32_t volatile overflow = 0;
uint16_t volatile start = 0;
uint16_t volatile end = 0;
uint32_t volatile hold_time = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		if( ++counter == 100 ){
			counter = 0;
			if( ++timer_sec == 60 ){
				timer_sec = 0;
				if( ++timer_minut == 60 ){
					timer_minut = 0;
				}
			}
		}

		if( state == 0 ){
			display_number[2] = timer_sec / 10;
			display_number[3] = timer_sec % 10;
			display_number[0] = timer_minut / 10;
			display_number[1] = timer_minut % 10;
		}else if(state == 1){
			display_number[1] = player1_point % 10;
			display_number[0] = (player1_point / 10) % 10;
			display_number[3] = player2_point % 10;
			display_number[2] = (player2_point / 10) % 10;
		}else if( state == 2 ){
			display_number[1] = player1_gem % 10;
			display_number[0] = (player1_gem / 10) % 10;
			display_number[3] = player2_gem % 10;
			display_number[2] = (player2_gem / 10) % 10;
		}

		curr = ( curr + 1 ) % 4;
		GPIOC->ODR &= ~0xF00;
		GPIOC->ODR &= ~0xFF;
		GPIOC->ODR |= seven_seg[display_number[curr]];
		GPIOC->ODR |= ( 0x1 ) << ( 8 + curr );


	}
	if( htim->Instance == TIM3 ){
		++overflow;
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM3){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			start = TIM3->CCR1;
			overflow = 0;
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
			end = TIM3->CCR2;
			hold_time = ( end + ( overflow * ( ARR +  1 ))) - start;
			if(hold_time >= 2000){
				player1_gem = player1_point = player2_gem = player2_point = 0;
			}
			overflow = 0;
		}
	}
}
