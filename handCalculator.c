#include <stdint.h>
#include "deck.h"

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

int main(int argc, char** argv)
{

}

//A method to search a hand for a straight flush and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a straight flush is present, setting values not in the final hand to 0 
int findStraightFlush(int hand[HANDSIZE], const int values[HANDSIZE], const int suits[HANDSIZE], uint16_t* rank)
{

}

//A method to search a hand for quads and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a quads are present, setting values not in the final hand to 0 
int findQuads(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    int quadsIndex = 0;
    int quadsValue;
    int quadsFound = 0;
 
    int kickerIndex = 0;
    int kicker = values[0];

    //Loop through hand, looking for quads
    for (; quadsIndex < HANDSIZE - 4; quadsIndex++)
    {
        if (values[quadsIndex] == values[quadsIndex + 1] == values[quadsIndex + 2] == values[quadsIndex + 3])
        {
            quadsValue = values[quadsIndex];
            quadsFound = 1;
            //If quads is at the beginning, the kicker is the 5th card
            if (quadsIndex == 0)
            {
                kickerIndex = 4;
                kicker = values[4];
            }
            break;
        }
    }

    //Return if we didnt find quads
    if (!quadsFound) return 0;

    //Encode quads into rank
    *rank ^= QUADS;
    *rank ^= quadsValue<<POSONE;
    *rank ^= kicker<<POSTWO;

    //Clear the hand value if it is not in the final hand
    int index;
    while (index < HANDSIZE)
    {
        //Skip the quads
        if (index == quadsIndex)
        {
            index += 4;
        }
        //Skip the kicker
        else if (index == kickerIndex)
        {
            index++;
        }
        //Clear other cards
        else
        {
            hand[index] = 0;
            index++;
        }
    }

    return 1;
}

//A method to search a hand for a full house and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a full house is present, setting values not in the final hand to 0 
int findFullHouse(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    int tripsIndex;
    int tripsValue;
    int tripsFound = 0;

    int pairIndex;
    int pairValue;
    int pairFound = 0;

    int index = 0;
    //Loop through the hand, looking for full house
    while (index < HANDSIZE)
    {
        //Look for trips if we haven't found it
        if (!tripsFound && (HANDSIZE - index) > 3)
        {
            if (values[index] == values[index + 1] == values[index + 2])
            {
                tripsIndex = index;
                tripsValue = values[index];
                tripsFound = 1;
                index += 3;
            }
        }
        //Look for pair if we haven't found it
        if (!pairFound && (HANDSIZE - index) > 2)
        {
            if (values[index] == values[index + 1])
            {
                pairIndex = index;
                pairValue = values[index];
                pairFound = 1;
                index += 2;
                continue;
            }
        }
        index++;
    }

    //Didn't find a full house
    if (!(tripsFound && pairFound)) return 0;

    //Encode full house into rank
    *rank ^= FULLHOUSE;
    *rank ^= tripsValue << POSONE;
    *rank ^= pairValue << POSTWO;

    //Clear the hand value if it is not in the final hand
    index = 0;
    while (index < HANDSIZE)
    {
        //Skip the trips
        if (index == tripsIndex)
        {
            index += 3;
        }
        //Skip the pair
        else if (index == pairIndex)
        {
            index += 2;
        }
        //Clear other cards
        else
        {
            hand[index] = 0;
            index++;
        }
    }

    return 1;
}

//A method to search a hand for a flush and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a flush is present, setting values not in the final hand to 0 
int findFlush(int hand[HANDSIZE], const int values[HANDSIZE], int suits[HANDSIZE], uint16_t* rank)
{
    int hearts = 0;
    int diamonds = 0;
    int spades = 0;
    int clubs = 0;
    int flushsuit = -1;
    //Total up suits
    for (int i = 0; i < HANDSIZE; i++)
    {
        switch (suits[i])
        {
            case HEART:
                hearts++;
                break;
            case DIAMOND:
                diamonds++;
                break;
            case SPADE:
                spades++;
                break;
            case CLUB:
                clubs++;
                break;
        }
    }
    
    //Check for flush of any suit
    if (hearts >= 5)
    {
        flushsuit = HEART;
    }
    else if (diamonds >= 5)
    {
        flushsuit = DIAMOND;
    }
    else if (spades >= 5)
    {
        flushsuit = SPADE;
    }
    else if (clubs >= 5)
    {
        flushsuit = CLUB;
    }

    //Didn't find a flush
    if (flushsuit == -1) return 0;

    //Loop through cards, encoding the first 5 of the flushsuit into the rank
    int numEncoded = 0;
    for (int i = 0; i < HANDSIZE; i++)
    {
        if (suits[i] == flushsuit && numEncoded < 5)
        {
            numEncoded++;
            //Each number has a one hot encoded bit, which is turned on if the hand has it
            switch (values[i])
            {
                case ACE:
                    *rank ^= HAS_A;
                    break;
                case KING:
                    *rank ^= HAS_K;
                    break;
                case QUEEN:
                    *rank ^= HAS_Q;
                    break;
                case JACK:
                    *rank ^= HAS_J;
                    break;
                case 10:
                    *rank ^= HAS_10;
                    break;
                case 9:
                    *rank ^= HAS_9;
                    break;
                case 8:
                    *rank ^= HAS_8;
                    break;
                case 7:
                    *rank ^= HAS_7;
                    break;
                case 6:
                    *rank ^= HAS_6;
                    break;
                case 5:
                    *rank ^= HAS_5;
                    break;
                case 4:
                    *rank ^= HAS_4;
                    break;
                case 3:
                    *rank ^= HAS_3;
                    break;
            }
        }
        //Clear the hand value if it is not in the final hand
        else
        {
            hand[i] = 0;
        }
    }
    return 1;
}

//A method to search a hand for a straight and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a straight is present, setting values not in the final hand to 0 
int findStraight(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{

}

//A method to search a hand for trips and encode its strength into the rank if present
//WARNING: This method will modify the hand array if trips are present, setting values not in the final hand to 0 
int findTrips(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    int tripsIndex = 0;
    int tripsValue;
    int tripsFound = 0;

    int topKickerIndex = 0;
    int topKicker = values[0];
    int bottomKickerIndex = 1;
    int bottomKicker = values[1];

    //Loop through hand, looking for trips
    for (; tripsIndex < HANDSIZE - 3; tripsIndex++)
    {
        if (values[tripsIndex] == values[tripsIndex + 1] == values[tripsIndex + 2])
        {
            tripsValue = values[tripsIndex];
            tripsFound = 1;
            //If trips is at the beginning, the kickers are the 4th and 5th cards
            if (tripsIndex == 0)
            {
                topKickerIndex = 3;
                topKicker = values[3];
                bottomKickerIndex = 4;
                bottomKicker = values[4];
            }
            //If trips is at the 2nd card, the kickers are the 1st and 5th cards
            else if (tripsIndex == 1)
            {
                bottomKickerIndex = 4;
                bottomKicker = values[4];
            }
            break;
        }
    }

    //Return if we didnt find trips
    if (!tripsFound) return 0;

    //Encode trips into rank
    *rank ^= TRIPS;
    *rank ^= tripsValue<<POSONE;
    *rank ^= topKicker<<POSTWO;
    *rank ^= bottomKicker<<POSTHREE;

    //Clear the hand value if it is not in the final hand
    int index;
    while (index < HANDSIZE)
    {
        //Skip the quads
        if (index == tripsIndex)
        {
            index += 3;
        }
        //Skip the kickers
        else if (index == topKickerIndex || index == bottomKicker)
        {
            index++;
        }
        //Clear other cards
        else
        {
            hand[index] = 0;
            index++;
        }
    }

    return 1;
}

//A method to search a hand for a two pair and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a two pair is present, setting values not in the final hand to 0 
int findTwoPair(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    int topPairIndex;
    int topPairValue;
    int topPairFound = 0;

    int bottomPairIndex;
    int bottomPairValue;
    int bottomPairFound = 0;

    int kickerIndex = 0;
    int kickerValue = values[0];

    int index = 0;
    //Loop through the hand, looking for the first pair
    while (index < HANDSIZE - 2)
    {
        if (values[index] == values[index + 1])
        {
            topPairIndex = index;
            topPairValue = values[index];
            topPairFound = 1;
            index += 2;
            //If first pair is at 1st card, kicker gets bumped back to 3rd card
            if (index == 0)
            {
                kickerIndex = 2;
                kickerValue = values[2];
            }
            break;
        }
        index++;
    }
    //If we found the first pair and there is still hand to loop through, look for second pair
    while (index < (HANDSIZE - 2))
    {
        if (values[index] == values[index + 1])
        {
            bottomPairIndex = index;
            bottomPairValue = values[index];
            bottomPairFound = 1;
            index += 2;
            //If second pair is at 3rd card, first pair must've been at 1st card, so kicker is 4th card
            if (index == 2)
            {
                kickerIndex = 4;
                kickerValue = values[4];
            }
            break;
        }
        index++;
    }

    if (!(topPairFound && bottomPairFound)) return 0;

    //Encode full house into rank
    *rank ^= TWOPAIR;
    *rank ^= topPairValue << POSONE;
    *rank ^= bottomPairValue << POSTWO;
    *rank ^= kickerValue << POSTHREE;

    //Clear the hand value if it is not in the final hand
    index = 0;
    while (index < HANDSIZE)
    {
        //Skip the pairs
        if (index == topPairIndex || index == bottomPairIndex)
        {
            index += 2;
        }
        //Skip the kicker
        else if (index == kickerIndex)
        {
            index ++;
        }
        //Clear other cards
        else
        {
            hand[index] = 0;
            index++;
        }
    }

    return 1;
}

//A method to search a hand for a pair and encode its strength into the rank if present
//WARNING: This method will modify the hand array if a pair is present, setting values not in the final hand to 0 
int findPair(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    int pairIndex = 0;
    int pairValue;
    int pairFound = 0;

    int topKickerIndex = 0;
    int topKicker = values[0];
    int middleKickerIndex = 1;
    int middleKicker = values[1];
    int bottomKickerIndex = 2;
    int bottomKicker = values[3];

    //Loop through hand, looking for trips
    for (; pairIndex < HANDSIZE - 2; pairIndex++)
    {
        if (values[pairIndex] == values[pairIndex + 1])
        {
            pairValue = values[pairIndex];
            pairIndex = 1;
            //If pair is at the beginning, the kickers are the 3rd, 4th and 5th cards
            if (pairIndex == 0)
            {
                topKickerIndex = 2;
                topKicker = values[2];
                middleKickerIndex = 3;
                middleKicker = values[3];
                bottomKickerIndex = 4;
                bottomKicker = values[4];
            }
            //If pair is at the 2nd card, the kickers are the 1st, 4th, and 5th cards
            else if (pairIndex == 1)
            {
                middleKickerIndex = 3;
                middleKicker = values[3];
                bottomKickerIndex = 4;
                bottomKicker = values[4];
            }
            //If pair is at the 3rd card, the kickers are the 1st, 2nd, and 5th cards
            else if (pairIndex == 2)
            {
                bottomKickerIndex = 4;
                bottomKicker = values[4];
            }
            break;
        }
    }

    //Return if we didnt find trips
    if (!pairFound) return 0;

    //Encode pair and kickers into rank
    //Adjust kickers so they will always be a number 0-9
    //Subtract 2 to adjust range from 2-14 to 0-12
    //Top kicker can only be 2-12, so subtract 2 to make it 0-10
    //Middle kicker can only be 1-11, so subtract 1 to make it 0-10
    //Bottom kicker can only be 0-10, so subtract 0 to make it 0-10
    //The kickers can never be the value of the pair,
    //so subtract 1 if kicker value is above the pair to make the range 0-9
    //Now they can be decimally encoded into the ones, tens, and hundreds place
    //The pair value will take the 1000s & 10000s place (up to 14000)
    //The pair encoding stays in the top two bits (>16000 decimal so no overlap)
    int tkRelative = topKicker    - 2 - 2 - (topKicker > pairValue ? 1 : 0);
    int mkRelative = middleKicker - 2 - 1 - (middleKicker > pairValue ? 1 : 0);
    int bkRelative = bottomKicker - 2 - 0 - (bottomKicker > pairValue ? 1 : 0);
    *rank ^= PAIR;
    *rank += 1000*pairValue;
    *rank += 100*tkRelative;
    *rank += 10*mkRelative;
    *rank += bkRelative;

    //Clear the hand value if it is not in the final hand
    int index;
    while (index < HANDSIZE)
    {
        //Skip the quads
        if (index == pairIndex)
        {
            index += 3;
        }
        //Skip the kickers
        else if (index == topKickerIndex || index == middleKicker || index == bottomKicker)
        {
            index++;
        }
        //Clear other cards
        else
        {
            hand[index] = 0;
            index++;
        }
    }

    return 1;
}

//A method to encode the high cards into the rank
//WARNING: This method will modify the hand array, setting values not in the final hand to 0
void findHighCard(int hand[HANDSIZE], const int values[HANDSIZE], uint16_t* rank)
{
    //Loop through top cards, encoding them into the rank
    for (int i = 0; i < 5; i++)
    {
        //Each number has a one hot encoded bit, which is turned on if the hand has it
        switch (values[i])
        {
            case ACE:
                *rank ^= HAS_A;
                break;
            case KING:
                *rank ^= HAS_K;
                break;
            case QUEEN:
                *rank ^= HAS_Q;
                break;
            case JACK:
                *rank ^= HAS_J;
                break;
            case 10:
                *rank ^= HAS_10;
                break;
            case 9:
                *rank ^= HAS_9;
                break;
            case 8:
                *rank ^= HAS_8;
                break;
            case 7:
                *rank ^= HAS_7;
                break;
            case 6:
                *rank ^= HAS_6;
                break;
            case 5:
                *rank ^= HAS_5;
                break;
            case 4:
                *rank ^= HAS_4;
                break;
            case 3:
                *rank ^= HAS_3;
                break;
        }
    }
    //Clear the cards from the hand that are not high cards
    for (int i = 5; i < HANDSIZE; i++)
    {
        hand[HANDSIZE] = 0;
    }
}

//A method to reorder the hand from higherst card value to lowest card value
//WARNING: this method will reorder the contents of hand, values, and suits
void sortHand(int hand[HANDSIZE], int values[HANDSIZE], int suits[HANDSIZE])
{
    for (int index = 0; index < HANDSIZE; index++)
    {
        int lowestIndex = index;
        int highestValue = values[index];
        for (int i = index; i < HANDSIZE; i++)
        {
            if (values[i] > highestValue)
            {
                lowestIndex = i;
                highestValue = values[i];
            }
            //Reorder values array
            int temp = values[index];
            values[index] = values[lowestIndex];
            values[lowestIndex] = temp;
            //Synchronously reorder suits array
            temp = suits[index];
            suits[index] = suits[lowestIndex];
            suits[lowestIndex] = temp;
            //Synchronously reorder hand array
            temp = hand[index];
            hand[index] = hand[lowestIndex];
            hand[lowestIndex] = temp;
        }
    }
}

//A function to calculate the relative rank of a poker hand.
//Comparing the integers returned by this function, the higher integer is the better hand.
//If the integers are the same, both of the hands are of the same strenght.
//This will take into account all kickers and nuances related to hand rank.
//WARNING: this method will alter the hand array in the followig ways:
//    First, it will sort all the cards by numerical value (not 52-1, but Ace-2)
//    Second, it will clear (set to zero) any card that is not used to determine rank (i.e. 5 nonzero cards will remain)
int calculateRank(int hand[HANDSIZE])
{
    uint16_t rank = 0;
    int values[HANDSIZE];
    int suits[HANDSIZE];
    //Calculate values and suits for each card
    for (int i = 0; i < HANDSIZE; i++)
    {
        values[i] = hand[i] % 13 + 2;
        suits[i] = hand[i] / 13;
    }
    sortHand(hand, values, suits);
    //Check for straight
    if (findStraightFlush(hand, values, suits, &rank)) return rank;
    if (findQuads(hand, values, &rank)) return rank;
    if (findFullHouse(hand, values, &rank)) return rank;
    if (findFlush(hand, values, suits, &rank)) return rank;
    if (findStraight(hand, values, &rank)) return rank;
    if (findTrips(hand, values, &rank)) return rank;
    if (findTwoPair(hand, values, &rank)) return rank;
    if (findPair(hand, values, &rank)) return rank;
    findHighCard(hand, values, &rank);
    return rank;
}