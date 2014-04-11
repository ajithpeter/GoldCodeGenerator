/*

Gold Code Generator - Learning Experiment for understanding GPS C/A Code Generation

Ajith Peter <ajith.peter@gmail.com>
Standard Process
11th April 2014

Ref:
    [1] Darius Plaušinaitis, Receiver Overview; Code Generation and Carrier Generation, Danish GPS Center, Presentation
    [2] James Bao-Yen Tsui, Fundamentals of Global Positioning System Receivers : A Software Approach, Wiley Series in Microwave and Optical Engineering

*/


#include <stdio.h>

#define BIT(x, n) ((x & (1 << n)) ? 1 : 0)

#define G2(x) (((g2ShiftRegister & 0x03FF) & x) ? 1 : 0)

#define G2_2 G2(0x0100)
#define G2_3 G2(0x0080)
#define G2_6 G2(0x0010)
#define G2_8 G2(0x0004)
#define G2_9 G2(0x0002)
#define G2_10 G2(0x0001)

short g1ShiftRegister;
short g2ShiftRegister;

/* G2 Delay Taps for GPS PRN */
int g2Tap1[] = {2, 3, 4, 5, 1, 2, 1, 2, 3, 2, 3, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 1, 4, 5, 6, 7, 8, 1, 2, 3, 4};
int g2Tap2[] = {6, 7, 8, 9, 9, 10, 8, 9, 10, 3, 4, 6, 7, 8, 9, 10, 4, 5, 6, 7, 8, 9, 3, 6, 7, 8, 9, 10, 6, 7, 8, 9};

void resetSR()
{
    g1ShiftRegister = 0xffff;
    g2ShiftRegister = 0xffff;
}

/* Uses a 11 position shift register - first 10 positions are the actual bits and the last position is the bit shifted out*/
short generateG1()
{
    g1ShiftRegister |= (((g1ShiftRegister & 0x0100) ? 0x0001 : 0x0000) ^ ((g1ShiftRegister & 0x0002) ? 0x0001 : 0x0000)) ? 0x0400 : 0x0000;
    g1ShiftRegister = (g1ShiftRegister >> 1) & 0x03FF;
    return g1ShiftRegister & 0x0001;
}

/* Uses 10 position shift register */
short generateG2(short PRN)
{
    short retval = BIT(g2ShiftRegister, 10 - g2Tap1[PRN]) ^ BIT(g2ShiftRegister, 10 - g2Tap2[PRN]);
    g2ShiftRegister = ((g2ShiftRegister >> 1) & 0x01FF) | ((G2_2 ^ G2_3 ^ G2_6 ^ G2_8 ^ G2_9 ^ G2_10) ? 0x0200 : 0x0000);
    return retval;
}

short generateCA(short PRN)
{
    return (generateG1() ^ generateG2(PRN)) & 1;
}

void testGold()
{
    int i;
    short rv = 0;
    short j;

    printf("PRN\t  Taps\t\tFirst 10 Chips C/A (Octal)\n");
    for (j = 0; j < 32; j++)
    {
        rv = 0;
        resetSR();
        for (i = 0; i < 10; i++)
        {
            rv = rv << 1;        
            rv |= generateCA(j);       
        }
        printf("%d\t %2d + %d \t%o\n", j + 1, g2Tap1[j], g2Tap2[j], rv);        
    }

}

int main(int argc, char *argv[])
{
  testGold();
  printf("Press enter to continue ...");
  getchar();	
  return 0;
}
