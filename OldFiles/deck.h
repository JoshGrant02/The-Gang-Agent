//Deck.h
//Josh Grant
//11/28/2023

/*
 * This file contains prototypes and macros for simulating a deck of cards that can be shuffled, dealt, burned, and counted.
 */

#ifndef DECK_H
#define DECK_H

#define DECKSIZE 52
#define HEART 0
#define DIAMOND 1
#define SPADE 2
#define CLUB 3

#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void initializeDeck();
void destroyDeck();
void shuffle();
int dealCard();
int burnCard();
int cardsLeft();

#endif