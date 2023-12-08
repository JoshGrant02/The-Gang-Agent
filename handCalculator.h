#ifndef H_HANDCALCULATOR
#define H_HANDCALCULATOR

#define ROYALFLUSH (0b1111 << 12)
#define STRAIGHTFLUSH (0b1110 << 12)
#define QUADS (0b1101 << 12)
#define FULLHOUSE (0b1100 << 12)
#define FLUSH (0b1011 << 12)
#define STRAIGHT (0b1010 << 12)
#define TRIPS (0b1001 << 12)
#define TWOPAIR (0b1000 << 12)
#define PAIR (0b01 << 14)
#define HAS_A (0b1 << 11)
#define HAS_K (0b1 << 10)
#define HAS_Q (0b1 << 9)
#define HAS_J (0b1 << 8)
#define HAS_10 (0b1 << 7)
#define HAS_9 (0b1 << 6)
#define HAS_8 (0b1 << 5)
#define HAS_7 (0b1 << 4)
#define HAS_6 (0b1 << 3)
#define HAS_5 (0b1 << 2)
#define HAS_4 (0b1 << 1)
#define HAS_3 (0b1 << 0)
#define POSONE 8
#define POSTWO 4
#define POSTHREE 0 
#define HANDSIZE 7
#define ASCIIZERO 48
#define HANDINDICATOR (0b1111 << 12)
#define PAIRINDICATOR (0b11 << 14)

void getCardValue(int card, char valueBuffer[3]);
int calculateRank(int hand[HANDSIZE]);

#endif