from pydantic import BaseModel
from hand import Hand
from deck import Deck
from models import Turn

class TheGangConfig(BaseModel):
    players: int

class TheGang():
    NUM_PLAYERS = 4

    def __init__(self):
        self.hands = [Hand() for _ in range(self.NUM_PLAYERS)]
        self.deck = Deck()
        self.deck.shuffle()
        self.turn = Turn.FLOP.value

    def reset(self):
        self.hands = [Hand() for _ in range(self.NUM_PLAYERS)]
        self.deck.shuffle()
        self.turn = Turn.FLOP.value

        for _ in range(2): # Deal 2 cards to each player
            for hand in self.hands:
                hand.add_card(self.deck.draw())

        return [hand.features for hand in self.hands]

    def reward(self, actions: list[int]) -> list[int]:
        return [0, 0, 0, 0]

    def step(self, actions: list[int]):
        if self.turn == Turn.DONE.value:
            raise Exception("Game is finished, please reset")
        if len(actions) != self.NUM_PLAYERS:
            raise Exception("Must supply 4 actions for 4 players")

        rewards = self.reward(actions)

        self.deck.burn() # Burn 1

        if self.turn == Turn.FLOP.value: # Deal 3 cards out
            for _ in range(3):
                card = self.deck.draw()
                for hand in self.hands:
                    hand.add_card(card)
        else: # Deal 1 card out
            card = self.deck.draw()
            for hand in self.hands:
                hand.add_card(card)

        self.turn += 1 # Increment the turn
        done = self.turn == Turn.DONE.value

        states = [hand.features for hand in self.hands]

        return {"state": states, "rewards": rewards, "done": done}
    