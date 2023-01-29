/*
 * ADC.c
 *
 *  Created on: 04/11/2019
 *      Author: ravenelco
 */
#include "lib/include.h"

extern void Configura_Reg_ADC0(void)
{
    /*
    Habilitar el modulo 0 del ADC con seis canales analogicos 
    en los puertos E, D Y B a una velocidad de conversion de 2Msps con evento de procesador 
    con los siguientes secuenciadores: sec3,sec0y sec2 
    */
     //Pag 396 para inicializar el modulo de reloj del adc RCGCADC
    SYSCTL->RCGCADC = (1<<0); //modulo 0 del ADC 
    //Pag 382 (RGCGPIO) Puertos base habilitación del reloj
    //                     Q      P        N       M       L      K      J       H      G      F     E      D       C      B     A
    SYSCTL->RCGCGPIO |= (0<<14)|(0<<13)|(0<<12)|(0<<11)|(0<<10)|(0<<9)|(0<<8)|(0<<7)|(0<<6)|(0<<5)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(0<<0);
    //Pag 760 (GPIODIR) Habilta los pines como I/O un cero para entrada y un uno para salida
    GPIOE_AHB->DIR = (0<<1) | (0<<4); //PE1 y PE4
    GPIOD_AHB->DIR = (0<<7) | (0<<5)|(0<<4); //PD7 PD5 PD4
    GPIOB_AHB->DIR = (0<<5); //PB5
    //(GPIOAFSEL) pag.770 Enable alternate función para que el modulo analógico tenga control de esos pines
    GPIOE_AHB->AFSEL = (1<<1) | (1<<4 );
    GPIOD_AHB->AFSEL = (1<<7) | (1<<5) | (1<<4); 
    GPIOB_AHB->AFSEL = (1<<5);
    //(GPIODEN) pag.781 desabilita el modo digital
    GPIOE_AHB->DEN = (0<<4) | (0<<5 );
    GPIOD_AHB->DEN = (0<<7) | (0<<5) | (0<<4); 
    GPIOB_AHB->DEN = (0<<5);
    //Pag 787 GPIOPCTL registro combinado con el GPIOAFSEL y la tabla pag 1808
    GPIOE_AHB->PCTL = GPIOE_AHB->PCTL & (0xFFF00FFF);
    GPIOD_AHB->PCTL = GPIOD_AHB->PCTL & (0xF0F00FFF);
    GPIOB_AHB->PCTL = GPIOB_AHB->PCTL & (0xFFF0FFFF);
    //(GPIOAMSEL) pag.786 habilitar analogico
    GPIOE_AHB->AMSEL = (1<<1) | (1<<4);
    GPIOD_AHB->AMSEL = (1<<4) | (1<<5) | (1<<7);
    GPIOB_AHB->AMSEL = (1<<5);
    //Pag 1159 El registro (ADCPC) establece la velocidad de conversión por segundo
    ADC0->PC = 0x7;//2Msps 
    //Pag 1099 Este registro (ADCSSPRI) configura la prioridad de los secuenciadores
    ADC0->SSPRI = 0x0231;//secuenciador0 prioridad más alta y el secuenciador 3 prioridad más baja
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS  =   (0<<0) | (0<<2) | (0<<3) | (0<<1);
    //Pag 1091 Este registro (ADCEMUX) selecciona el evento que activa la conversión (trigger)
    ADC0->EMUX  = (0x0000); //por procesador 
    //Pag 1129 Este registro (ADCSSMUX2) define las entradas analógicas con el canal y secuenciador seleccionado
    ADC0->SSMUX2 = 0x0089;//recibe a los canales 8 y 9 
    //pag 868 Este registro (ADCSSCTL2), configura el bit de control de muestreo y la interrupción
    ADC0->SSCTL2 = (1<<6) | (1<<5) ;//interrumpe
    /* Enable ADC Interrupt */
    ADC0->IM |= (1<<2); /* Unmask ADC0 sequence 2 interrupt pag 1082*/ //interrumpe conversion 
    //NVIC_PRI4_R = (NVIC_PRI4_R & 0xFFFFFF00) | 0x00000020;
    //NVIC_EN0_R = 0x00010000;
    //Pag 1077 (ADCACTSS) Este registro controla la activación de los secuenciadores
    ADC0->ACTSS = (1<<3) | (1<<2) | (0<<1) | (1<<0);//habilitar secuenciadores 
    ADC0->PSSI |= (1<<2);
}
extern void ADC0_InSeq2(uint16_t *Result, uint16_t*duty){

    //ADC Processor Sample Sequence Initiate (ADCPSSI)
       ADC0->PSSI = 0x00000004;
       while((ADC0->RIS&0x04)==0){}; // espera al convertidor
       Result[1] = ADC0->SSFIFO2&0xFFF; //  Leer  el resultado almacenado en la pila2
       Result[0] = ADC0->SSFIFO2&0xFFF;
       duty[0] = (Result[0]*20000)/4096; 
       duty[1] = (Result[1]*20000)/4096; 
       printChar('A');
       ADC0->ISC = 0x0004;  //Conversion finalizada

}


