//Deck.c
//Josh Grant
//11/28/2023

/*
 * This file contains code to simulate a deck of cards that can be shuffled, dealt, burned, and counted.
 * The deck must be initialized before use to allocate dynamic memory to store the cards.
 * The deck must be destroyed the end of use to free the allocated memory.
 * Each card in the deck is an integer from 1 to 52, where each integer translates to 1 card in a 52 card deck.
 * The value can be determined by performing ((number - 1) % 13 + 2), where 11->J, 12->Q, 13->K, 14->A
 * and all other numbers are their respective values.
 * The suit can be determined by doing performing (number / 13 round down), where 0->Hearts, 1->Diamonds, 2->Spades, and 3->Clubs
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "deck.h"

static int* deck;
static int deckInitialized = 0;
static int cardIndex;

//Initialized an ordered deck
void initializeDeck()
{
    //Only initialize if we haven't alread
    if (!deckInitialized)
    {
        time_t seed;
        time(&seed);
        srand(seed);
        deck = malloc(sizeof(int)*DECKSIZE);
        //Set the initial values for the deck
        for (int i = 0; i < DECKSIZE; i++)
        {
            deck[i] = i+1;
        }
        cardIndex = 0;
        deckInitialized = 1;
    }
}

//Free's memory
void destroyDeck()
{
    //Only destroy if we have initialized
    if (deckInitialized)
    {
        free(deck);
        deckInitialized = 0;
    }
}

//Collects all the cards and shuffle them
void shuffle()
{
    //Create an array of random values
    int order[DECKSIZE];
    for (int i = 0; i < DECKSIZE; i++)
    {
        order[i] = rand();
    }

    //Sort the array of random values, moving the deck the same manner to shuffle it
    for (int currentIndex = 0; currentIndex < DECKSIZE; currentIndex++)
    {
        int lowestIndex = currentIndex;
        int lowestValue = order[currentIndex];
        for (int i = currentIndex; i < DECKSIZE; i++)
        {
            if (order[i] < lowestValue)
            {
                lowestIndex = i;
                lowestValue = order[i];
            }
            //Reorder order array
            int temp = order[currentIndex];
            order[currentIndex] = order[lowestIndex];
            order[lowestIndex] = temp;
            //Synchronously reorder the deck
            temp = deck[currentIndex];
            deck[currentIndex] = deck[lowestIndex];
            deck[lowestIndex] = temp;
        }
    }

    cardIndex = 0;
}

//Deals the card from the top of the deck
//RETURN: The value of the card. 0 if deck is empty 
int dealCard()
{
    if (cardIndex >= DECKSIZE) return 0;
    return deck[cardIndex++];
}

//Burns a card
//RETURN: 1 if card is burned. 0 if deck is empty
int burnCard()
{
    if (cardIndex >= DECKSIZE) return 0;
    ++cardIndex;
    return 1;
}

//RETURN: the number of cards left in the deck
int cardsLeft()
{
    return DECKSIZE - cardIndex;
}

//RETURN: the number of cards that were either dealt or burned
int cardsDealt()
{
    return cardIndex;
}