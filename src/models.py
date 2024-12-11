from pydantic import BaseModel
from enum import Enum

class Suit(Enum):
    HEART = 0
    DIAMOND = 1
    SPADE = 2
    CLUB = 3

MAX_CARDS = 7
SUITS = [Suit.HEART, Suit.DIAMOND, Suit.SPADE, Suit.CLUB]
SUITS_STR = {
    Suit.HEART.value: '♥',
    Suit.DIAMOND.value: '♦',
    Suit.SPADE.value: '♠',
    Suit.CLUB.value: '♣'
}
VALUES = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14]
VALUES_STR = {
    2: '2',
    3: '3',
    4: '4',
    5: '5',
    6: '6',
    7: '7',
    8: '8',
    9: '9',
    10: 'T',
    11: 'J',
    12: 'Q',
    13: 'K',
    14: 'A'
}

class Card(BaseModel):
    # The number (or face value) of the card (A = 14)
    value: int
    # The suit of the card
    suit: int

    def __str__(self):
        return f"{VALUES_STR[self.value]}{SUITS_STR[self.suit]}"

class Rank(Enum):
    HIGH_CARD = 0
    ONE_PAIR = 1
    TWO_PAIR = 2
    THREE_OF_A_KIND = 3
    STRAIGHT = 4
    FLUSH = 5
    FULL_HOUSE = 6
    FOUR_OF_A_KIND = 7
    STRAIGHT_FLUSH = 8

class Potential(Enum):
    HAS_RANK = 3
    ONE_CARD_AWAY = 2
    POSSIBLE = 1
    IMPOSSIBLE = 0

class HandFeatures(BaseModel):
    # Current rank of the hand
    rank: Rank
    # The kickers of the hand
    two_pair_potential: Potential
    three_of_a_kind_potential: Potential
    straight_potential: Potential
    flush_potential: Potential
    full_house_potential: Potential
    four_of_a_kind_potential: Potential
    straight_flush_potential: Potential
