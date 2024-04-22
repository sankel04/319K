// Lab9Main.c
// Runs on MSPM0G3507
// Lab 9 ECE319K
// Your name
// Last Modified: 12/31/2023

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"

void PLL_Init(void){ // set phase lock loop (PLL)
  //Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

typedef enum {dead,alive} status_t;
struct sprite{
   uint32_t x; // x coordinate
   uint32_t y; // y coordinate
   //uint32_t vx;
   //uint32_t vy;
   uint32_t oldX; // old x coordinate
   uint32_t oldY; // old y coordinate
   const uint16_t *image; // ptr->image
   status_t life; // dead/alive
};
typedef struct sprite sprite_t;

sprite_t player = {0,150,0,150, PlayerShip0, alive};

sprite_t Enemys[6];

uint32_t bulletIndex = 0;
sprite_t Bullets[20];

uint32_t playerBulletIndex = 0;
sprite_t playerBullets[20];

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

uint32_t NeedToDraw = 0;
uint32_t playerCNT;

void Move(void){
  uint32_t i;
  for(i=0; i<6; i++){
      if(Enemys[i].life == alive){
         NeedToDraw = 1; // mark as changed
         if(Enemys[i].y < 142){
            Enemys[i].oldY = Enemys[i].y;
            Enemys[i].oldX = Enemys[i].x;
            Enemys[i].y += 2; // move down
         }
         else{
            player.life = dead; // at bottom
         }
         for(uint8_t cnt = 0; cnt<playerCNT;cnt++){ // iterate through all player bullets and check against all alive enemies
             if(playerBullets[cnt].y == (Enemys[i].y-2)){ // if player bullet made it to enemy y
                 //if(((playerBullets[cnt].x+2) > (Enemys[i].x + 5)) && ((playerBullets[cnt].x+2) < (Enemys[i].x +14))){
                     Enemys[i].life = dead;

                 //}
             }
         }
      }
  }
  for(uint8_t i=0;i<20;i++){ // iterate through all bullets and move them down / up
      NeedToDraw = 1;
      if(Bullets[i].y >= 142){ // checks if bullet at bottom
          if(((Bullets[i].x + 2) > (player.x - 7)) && ((Bullets[i].x + 2) < (player.x +18))){ // checks if bullets in range of player
              player.life = dead;
          }
      }
      Bullets[i].oldY = Bullets[i].y;
      Bullets[i].oldX = Bullets[i].x;
      Bullets[i].y += 4;
      playerBullets[i].oldY = playerBullets[i].y;
      playerBullets[i].oldX = playerBullets[i].x;
      playerBullets[i].y -= 4;
  }
}
uint32_t enemyCNT;
void genBullets(void){
    enemyCNT++;
    uint8_t ran = Random(6); // gets random 0-5
    // adds random placed bullet to bullet array
    Bullets[bulletIndex].x = Enemys[ran].x;
    Bullets[bulletIndex].y = Enemys[ran].y;
    Bullets[bulletIndex].oldX = Enemys[ran].oldX;
    Bullets[bulletIndex].oldY = Enemys[ran].oldY;
    Bullets[bulletIndex].image = Bullet;
    Bullets[bulletIndex].life = alive;
    bulletIndex = (bulletIndex+1)%20; // keeps index in range 0-19
}

void genPlayerBullet(void){
    playerCNT++;
    playerBullets[playerBulletIndex].x = player.x;
    playerBullets[playerBulletIndex].y = player.y;
    playerBullets[playerBulletIndex].oldX = player.x; //playerBullets[playerBulletIndex].x;
    playerBullets[playerBulletIndex].oldY = player.y; //playerBullets[playerBulletIndex].y;
    playerBullets[playerBulletIndex].image = playerBullet;
    playerBullets[playerBulletIndex].life = alive;
    playerBulletIndex = (playerBulletIndex+1)%20; // keeps index in range 0-19
}
void EdgeTriggered_Init(void){
    //LaunchPad_Init(); // PB21 is input with internal pull up resistor
    GPIOB->POLARITY15_0 = 0x08000000; // falling
    // bits value action
    // 31,30 00 PB31 none
    // ...
    // 13-12 00 PB22 none
    // 11-10 10 PB21 falling
    // ...
    // 1-0 00 PB16 none
    GPIOB->CPU_INT.ICLR = 0x02000; // clear RIS bit 13
    GPIOB->CPU_INT.IMASK = 0x02000; // arm PB13
    NVIC->IP[0] = (NVIC->IP[0]&(~0x0000FF00))|2<<14; // priority=2
    NVIC->ISER[0] = 1 << 1; // Group1 interrupt
}

void GROUP1_IRQHandler(void){
    LED_Toggle(2);
    GPIOB->CPU_INT.ICLR = 0x002000; // clear bit 13
    genPlayerBullet();
}



uint8_t Flag = 0; // global semaphore

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    if(player.life == dead){
        ST7735_FillScreen(ST7735_BLACK);
        ST7735_SetCursor(1, 1);
        ST7735_OutString("GAME OVER");
        ST7735_SetCursor(1, 2);
        ST7735_OutString("Nice try,");
        ST7735_SetCursor(1, 3);
        ST7735_OutString("Earthling!");
        ST7735_SetCursor(2, 4);
        while(1){} // infinite loop
    }
    uint32_t x = (120*ADCin())/4095; // 1) sample slide pot 0 to 120
    uint8_t switchState = Switch_In(); // 2) read input switches
    // do an if pause/play statement here
    Move(); // 3) move sprites
    if(x != player.x){
        player.oldX = player.x;
        player.oldY = player.y;
        player.x = x; // set players x value to what was read at slide pot
    }
    // 4) start sounds

    Flag = 1; // 5) set semaphore
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint32_t counter = 0;
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
  Move(); // moves the enemy sprites
  if(counter == 6){
      genBullets();
  }
  counter = (counter+1)%7;
}

uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE, WELCOME,INSTRUCTION1, INSTRUCTION2} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Welcome_English[] ="Welcome to \n Space Invaders!";
const char Welcome_Spanish[] ="Bienvenidos a las \n Space Invaders!";
const char Instruction1_English[] ="> fire missile";
const char Instruction2_English[] ="< play/pause";
const char Instruction1_Spanish[] ="> dispara el misil";
const char Instruction2_Spanish[] ="< jugar/pausa";
const char *Phrases[6][2]={
  {Hello_English,Hello_Spanish},
  {Goodbye_English,Goodbye_Spanish},
  {Language_English,Language_Spanish},
   {Welcome_English,Welcome_Spanish},
   {Instruction1_English,Instruction1_Spanish},
   {Instruction2_English,Instruction2_Spanish}
};

void Enemy_Init(void){ int i;
    for(i=0; i<6; i++){
        Enemys[i].x = 20*i+12;
        Enemys[i].y = 10; // along the top
        Enemys[i].image = SmallEnemy30pointA;
        Enemys[i].life = alive;
    }
    /*
    for(i=0; i<6; i++){
         Bullets[i].x = 20*i+16;
         Bullets[i].y = 10; // along the top
         Bullets[i].image = Bullet;
         Bullets[i].life = alive;
    }
*/
    Enemys[0].oldX = 0; Enemys[0].oldY = 0;
    Enemys[1].oldX = 0; Enemys[1].oldY = 0;
    Enemys[2].oldX = 0; Enemys[2].oldY = 0;
    Enemys[3].oldX = 0; Enemys[3].oldY = 0;
    Enemys[4].oldX = 0; Enemys[4].oldY = 0;
    Enemys[5].oldX = 0; Enemys[5].oldY = 0;
/*
    Bullets[0].oldX = 0; Bullets[0].oldY = 0;
    Bullets[1].oldX = 0; Bullets[1].oldY = 0;
    Bullets[2].oldX = 0; Bullets[2].oldY = 0;
    Bullets[3].oldX = 0; Bullets[3].oldY = 0;
    Bullets[4].oldX = 0; Bullets[4].oldY = 0;
    Bullets[5].oldX = 0; Bullets[5].oldY = 0;
*/
}



// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_OutString((char *)Phrases[2][0]);
  ST7735_OutChar(30);
  ST7735_OutChar(13);
  ST7735_OutString((char *)Phrases[2][1]);
  ST7735_OutChar(31);
  ST7735_OutChar(13);
  Switch_Init();
  while(1){
      if(Switch_In() == 3){ // up
          myLanguage = English;
          break;
      }
      if(Switch_In() == 4){ // down
          myLanguage = Spanish;
          break;
      }
  }
  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(10,10);
  ST7735_OutString((char *)Phrases[HELLO][myLanguage]);
  ST7735_FillScreen(0x0000);
    ST7735_SetCursor(0,0);
    ST7735_OutString((char *)Phrases[HELLO][myLanguage]);
    ST7735_OutChar(13);
    ST7735_OutString((char *)Phrases[WELCOME][myLanguage]);
    Clock_Delay1ms(3000);
    ST7735_FillScreen(0x0000);
    ST7735_SetCursor(0,0);
    ST7735_OutString((char *)Phrases[INSTRUCTION1][myLanguage]);
    ST7735_OutChar(13);
    ST7735_OutString((char *)Phrases[INSTRUCTION2][myLanguage]);
    while(1){
        if(Switch_In() == 0x01000000){
            break;
        }
    }
return 0;
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs
      Clock_Delay1ms(250);
      LED_Toggle(1);
      Clock_Delay1ms(250);
      LED_Toggle(1);
      LED_Toggle(2);
      Clock_Delay1ms(250);
      LED_Toggle(2);
      LED_Toggle(3);
      Clock_Delay1ms(250);
      LED_Toggle(3);
      if(Switch_In() == 1){
          LED_Toggle(3); // if left, tog green
          Clock_Delay1ms(250);
      }
      else if(Switch_In() == 2){ // if right, tog yellow
          LED_Toggle(2);
      }
  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ADCinit();     //PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  Enemy_Init(); // fill the enemy arr
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(80000000/30,2);
  EdgeTriggered_Init();
  // initialize all data structures
  ST7735_InitPrintf();
   ST7735_FillScreen(0x0000);            // set screen to black
   ST7735_OutString((char *)Phrases[2][0]);
   ST7735_OutChar(30);
   ST7735_OutChar(13);
   ST7735_OutString((char *)Phrases[2][1]);
   ST7735_OutChar(31);
   ST7735_OutChar(13);
   Switch_Init();
   while(1){
       if(Switch_In() == 3){ // up
           myLanguage = English;
           break;
       }
       if(Switch_In() == 4){ // down
           myLanguage = Spanish;
           break;
       }
   }
   ST7735_FillScreen(0x0000);            // set screen to black
   ST7735_SetCursor(10,10);
   ST7735_OutString((char *)Phrases[HELLO][myLanguage]);
   ST7735_FillScreen(0x0000);
     ST7735_SetCursor(0,0);
     ST7735_OutString((char *)Phrases[HELLO][myLanguage]);
     ST7735_OutChar(13);
     ST7735_OutString((char *)Phrases[WELCOME][myLanguage]);
     Clock_Delay1ms(3000);
     ST7735_FillScreen(0x0000);
     ST7735_SetCursor(0,0);
     ST7735_OutString((char *)Phrases[INSTRUCTION1][myLanguage]);
     ST7735_OutChar(13);
     ST7735_OutString((char *)Phrases[INSTRUCTION2][myLanguage]);
     while(1){
         if(Switch_In() == 1){
             break;
         }
     }
     while(1){
            // write code to test switches and LEDs
              Clock_Delay1ms(250);
              LED_Toggle(1);
              Clock_Delay1ms(250);
              LED_Toggle(1);
              LED_Toggle(2);
              Clock_Delay1ms(250);
              LED_Toggle(2);
              LED_Toggle(3);
              Clock_Delay1ms(250);
              LED_Toggle(3);
              break;
         }
  __enable_irq();
  ST7735_FillScreen(ST7735_BLACK);
  while(1){
    // wait for semaphore
      if(Flag == 1){
          ST7735_DrawBitmap(player.oldX, player.oldY, BlankPlayerShip, 18,8);
          ST7735_DrawBitmap(player.x, player.y, PlayerShip0, 18,8);
          if(NeedToDraw == 1){
              NeedToDraw = 0;
              for(uint8_t i = 0; i<6;i++){
                  ST7735_DrawBitmap(Enemys[i].oldX, Enemys[i].oldY, BlankSmallEnemy30pointA, 16,10);
                  if(Enemys[i].life == alive){
                      ST7735_DrawBitmap(Enemys[i].x, Enemys[i].y, SmallEnemy30pointA, 16,10);
                  }

              }
              for(uint8_t i = 0; i<=playerCNT; i++){

                  ST7735_DrawBitmap(playerBullets[i].oldX, playerBullets[i].oldY, BlankBullet, 5,8);
                  ST7735_DrawBitmap(playerBullets[i].x, playerBullets[i].y, playerBullet, 5,8);
              }
              for(uint8_t i = 0; i<=enemyCNT; i++){
                  ST7735_DrawBitmap(Bullets[i].oldX, Bullets[i].oldY, BlankBullet, 5,8);
                  ST7735_DrawBitmap(Bullets[i].x, Bullets[i].y, Bullet, 5,8);
              }

          }
          Flag = 0; // clear semaphore
      }
  }
}

