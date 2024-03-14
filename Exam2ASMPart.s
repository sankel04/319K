// Exam2ASMPart.s
// ECE319K/ECE319H Exam2, Spring 2023
// ***Sanjita Kelkar***
// This is the assembly Part of Exam 2 (See Exam2ASMPart.c for C part)
// The assembly part is not related to the C part
// The assembly part has one function
     .text
     .align 2
     .global CountandRemoveX
//(30) Question 3: array manipulation
// Write an assembly subroutine that counts and removes all occurrences
//   of number X from an array.
//   The value -32768 marks the end of the array.
// Inputs: R0 is a pointer to array of signed 16-bit integers
//         R1 is X, the 16-bit integer to count and remove
// Output: R0 is the count of the occurrences that were removed
//   You will modify the array that was passed to you
// Note: R1 will never be -32768
// Note: you get half points for just counting the number of occurances
// Follow AAPCS
// Example test cases
// Case 0: R0->{-32768}, R1=5                   (empty)
//             {-32768} and return R0=0         (empty)
// Case 1: R0->{ 1,-2,5,10,-32768}, R1=5        (size is 4)
//             { 1,-2,10,-32768} and return R0=1(reduces to size 3)
// Case 2: R0->{ 1,1,1,-3,1,7,1,8,-32768}, R1=1 (size is 8)
//             { -3,7,8,-32768} and return R0=5 (reduces to size 3)
// Case 3: R0->{ 7,-3,0,7,-3,-32768}, R1=-1     (size is 5)
//             { 7,-3,0,7,-3,-32768} and return R0=0 (remains 5)
CountandRemoveX:
//remove the following line and replace with your solution
   // R0 is the pointer to array
   // R1 is 16 bit value
PUSH {R4-R7, LR}
LDR R5, =0//counter
LOOP:
   LDR R3, =0
   LDRSH R2, [R0,R3] // has value of first element
   LDR R7, =-2
   ADDS R0, #2
   LDR R4, =-32768
   CMP R4, R2
   BEQ DONE
   CMP R2, R1 //is it equal
   BNE LOOP

   ADDS R5, #1
   MOVS R3, #0
   LDRSH R6, [R0,R3]
   STRH R6, [R0,R7]
   CMP R6, R4
   BEQ DONE
   ADDS R3, #2
   B LOOP


DONE: LDR R0, [R5]
POP {R4-R7, PC}
   BX LR


  .end
