import numpy as np
import random

from models import Card, SUITS, VALUES

# A class to manage and deal a deck of cards
class Deck:
    DECK_SIZE = 52

    # Initialize and shuffle the deck
    def __init__(self):
        # Set up initial deck
        self.cards = [Card(value=value, suit=suit) for value in VALUES for suit in SUITS]
        self.card_index = 0
        self.shuffle()

    # Shuffle the deck
    def shuffle(self) -> None:
        self.card_index = 0
        random.shuffle(self.cards)

    # Draw a card from the deck
    def draw(self) -> Card:
        if self.card_index >= self.DECK_SIZE:
            raise Exception("No cards left to draw")
        card = self.cards[self.card_index]
        self.card_index += 1
        return card

    
    def burn(self) -> None:
        if self.card_index >= self.DECK_SIZE:
            raise Exception("No cards left to burn")
        self.card_index += 1

    # Get the number of cards left in the deck
    def cards_left(self) -> int:
        return self.DECK_SIZE - self.card_index

    # Get the number of cards that have been dealt
    def cards_dealt(self) -> int:
        return self.card_index

if __name__ == "__main__":
    deck = Deck()
    print("Cards Left:", deck.cards_left())
    print("Cards Dealt:", deck.cards_dealt())
    print("Drawing:", deck.draw())
    print("Drawing:", deck.draw())
    print("Drawing:", deck.draw())
    deck.burn()
    print("Burning")
    print("Cards Left:", deck.cards_left())
    print("Cards Dealt:", deck.cards_dealt())

    print("Shuffling...")
    deck.shuffle()
    print("Cards Left:", deck.cards_left())
    print("Cards Dealt:", deck.cards_dealt())
    print("Drawing:", deck.draw())
    deck.burn()
    print("Burning")
    print("Drawing:", deck.draw())
    deck.burn()
    print("Burning")
    print("Cards Left:", deck.cards_left())
    print("Cards Dealt:", deck.cards_dealt())

