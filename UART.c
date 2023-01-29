#include "lib/include.h"
//UART5 con fclk 40MHZ Baud-rate 28800
extern void Configurar_UART5(void)
{
    SYSCTL->RCGCUART  = (1<<5);   //Paso 1 habilitar (RCGCUART) para uart5 corresponde a PC6 Y PC7 pag.1164 
    SYSCTL->RCGCGPIO |= (1<<2);     //Paso 2 (RCGCGPIO) habilitar el perifierico  pag.382  Enable clock port A
    //(GPIOAFSEL) pag.671 Habilitar la funcion alternativa 
    GPIOC_AHB->AFSEL = (1<<6) | (1<<7); //Pin C6 Y C7 
    //GPIO Port Control (GPIOPCTL) PC6-> U5Rx PC7-> U5Tx pag.688
    GPIOC_AHB->PCTL = (GPIOA_AHB->PCTL&0x11FFFFFF) | 0x11000000;// (1<<0) | (1<<4);//0x00000011

    // GPIO Digital Enable (GPIODEN) pag.682
    GPIOC_AHB->DEN = (1<<6) | (1<<7);//PC6 Y PC7 
    //UART5 UART Control (UARTCTL) pag.918 DISABLE!!
    UART5->CTL = (0<<9) | (0<<8) | (0<<0);

    // UART Integer Baud-Rate Divisor (UARTIBRD) pag.914
    /*
    BRD = 20,000,000 / (16 * 9600) = 130.2 
    BRD0 40,000,000 / (16*28800)=86.80555556 
    UARTFBRD[DIVFRAC] = integer(0.2 * 64 + 0.5) = 14
    UARTFBRD[DIVFRAC] = integer(0.80555556* 64 + 0.5) = 52.05
    */
    UART5->IBRD = 87;
    // UART Fractional Baud-Rate Divisor (UARTFBRD) pag.915
    UART5->FBRD = 52;
    //  UART Line Control (UARTLCRH) pag.916
    UART5->LCRH = (0x3<<5)|(1<<4);
    //  UART Clock Configuration(UARTCC) pag.939
    UART5->CC =(0<<0);//para el reloj interno 
    //Disable UART0 UART Control (UARTCTL) pag.918
    UART5->CTL = (1<<0) | (1<<8) | (1<<9); //habilitar los pines



}

extern char readChar(void)
{
    //UART FR flag pag 911
    //UART DR data 906
    int v;
    char c;
    while((UART0->FR & (1<<4)) != 0 );
    v = UART0->DR & 0xFF;
    c = v;
    return c;
}
extern void printChar(char c)
{
    while((UART0->FR & (1<<5)) != 0 );
    UART0->DR = c;
}
extern void printString(char* string)
{
    while(*string)
    {
        printChar(*(string++));
    }
}

extern char * readString(char delimitador)
{

   int i=0;
   char *string = (char *)calloc(10,sizeof(char));
   char c = readChar();
   while(c != delimitador)
   {
       *(string+i) = c;
       i++;
       if(i%10==0)
       {
           string = realloc(string,(i+10)*sizeof(char));
       }
       c = readChar();
   }

   return string;

}
//Experimento 2

//El envio es su nombre  (rave) 

// invertirlo y regresarlo con numeros consecutivos
// entre letras (e1v2a3r) 

