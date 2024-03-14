/*  Exam2CPart.c
 *****Sanjita Kelkar*******
 
 This is ECE319K/ECE319H Exam2, Spring 2023
 This is the C Part of the Quiz (See Exam2ASMPart.s for the assembly part)
 The C part is not related to the assembly part
 You MUST edit line 11 to specify your EID 

*/   
#include <ti/devices/msp/msp.h>
#include "LaunchPad.h"
#include "Clock.h"
#include "UART.h"
#include <stdint.h>
#include <string.h>
const char EID[16] = "suk264";   // replace abc123 with your EID

// (40) Question 1: Calculate square root distance between two points
struct Point{
  int32_t x;    // x-coordinate of point
  int32_t y;    // y-coordinate of point
};
typedef struct Point Point_t;

// Inputs: t1 is the first point
//         t2 is the second point
// Output: calculate isqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
// Case1: t1={10,12} t2={7,16} 
//      return isqrt((10-7)*(10-7)+(12-16)*(12-16)=sqrt(25)=5
// Case2: t1={4,9} t2={10,8} 
//      return isqrt((4-10)*(4-10)+(9-8)*(9-8))=sqrt(37)=6
// Note: The isqrt function is implemented below
int32_t isqrt(int32_t s);

int32_t SqrtDistance(Point_t t1, Point_t t2){
// Replace the following line with your solution
    int32_t x = 0;
    x = ((t1.x)-(t2.x))*((t1.x)-(t2.x)) + ((t1.y)-(t2.y))*((t1.y)-(t2.y));
    return isqrt(x);
}

// Returns the integer square root of the input s,
//   calculated using Newton's method
int32_t isqrt(int32_t s){
  uint32_t t;     	// t*t will become s
  if(s<0) return 0;
  t = s/10+1;        	// initial guess
  for(int32_t n = 16; n; --n){// guaranteed to finish
    t = ((t*t+s)/t)/2;  
  }
  return t; // round up
}

// (30) Question 2: Find bounding box
struct Box{
  Point_t minPoint;
  Point_t maxPoint;
};
typedef struct Box Box_t;
// Search all points of the box and 
//   find the largest and smallest x and y
//   that contain nonzero values
// Example box (actual test cases are randomly generated)
//x=0  1  2  3  4  5  6  7
// {0, 0, 0, 0, 0, 0, 0, 0}, y=0
// {0, 0, 1, 0, 0, 0, 0, 0},
// {0, 0, 0, 1, 1, 0, 0, 0},
// {0, 0, 1, 1, 1, 0, 0, 0},
// {0, 0, 1, 1, 1, 1, 0, 0}} y=4
// smallest x is 2, smallest y is 1, largest x is 5, largest y is 4
// The map point at x,y is map[x][y]
// The corners of the bounding box do not have to be non-zero
void Find_Bounding_Box(uint8_t map[8][5], Box_t *b){
// remove the following four lines and replace with your solution
   int32_t ifBreak = 0;
   int32_t ifBreak1 = 0;
   int32_t ifBreak2 = 0;
   int32_t ifBreak3 = 0;

   for(int32_t x =0; x<8; x++){
       for(int32_t y = 0; y<5; y++){
           if(map[x][y] != 0){
               (*b).minPoint.x= x;//smallest x
               ifBreak = 1;
               break;
           }
       }
       if(ifBreak){
           break;
       }
   }
   for(int32_t y = 0; y<5; y++){
       for(int32_t x = 0; x<8; x++){
           if(map[x][y] != 0){
               (*b).minPoint.y= y;//smallest y
               ifBreak1 = 1;
               break;
           }
       }
       if(ifBreak1){
           break;
       }
   }
  for(int32_t i = 7; i >= 0; i--){
      for(int32_t j = 4; j >= 0; j--){
          if(map[i][j] != 0){
              (*b).maxPoint.x = i;  // largest x
              ifBreak2 = 1;
              break;
          }
      }
      if(ifBreak2){
          break;
      }

}
  for(int32_t j = 4; j >= 0; j--){
      for(int32_t i = 7; i >= 0; i--){
          if(map[i][j] != 0){
              (*b).maxPoint.y = j;  // largest y
              ifBreak3 = 1;
              break;
          }
      }
     if(ifBreak3){
         break;
     }
}

}

