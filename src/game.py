from pydantic import BaseModel
from hand import Hand
from deck import Deck
from models import Turn

class TheGang():
    NUM_PLAYERS = 4

    def __init__(self):
        self.hands = [Hand(player_num=player_num) for player_num in range(self.NUM_PLAYERS)]
        self.deck = Deck()
        self.deck.shuffle()
        self.turn = Turn.FLOP.value

    def reset(self):
        self.hands = [Hand(player_num=player_num) for player_num in range(self.NUM_PLAYERS)]
        self.deck.shuffle()
        self.turn = Turn.FLOP.value

        for _ in range(2): # Deal 2 cards to each player
            for hand in self.hands:
                hand.add_card(self.deck.draw())

        return [hand.features for hand in self.hands]

    def reward(self, actions: list[int]) -> list[int]:
        hand_order = self.hands.copy()
        hand_order.sort(key=lambda x: (
            x.features.rank.value,
            x.features.first_card,
            x.features.second_card,
            x.features.third_card,
            x.features.fourth_card,
            x.features.fifth_card
        ), reverse=True)
        rewards = [0, 0, 0, 0]
        for i, hand in enumerate(hand_order):
            order = 4 - i
            player_num = hand.player_num
            action = actions[player_num]
            if abs(action - order) == 0: # Player guessed correctly
                rewards[player_num] = 50
            elif abs(action - order) <= 1: # Player guessed within 1
                rewards[player_num] = 25
            elif abs(action - order) <= 2: # Player guessed within 2
                rewards[player_num] = 10
            else: # Player guessed incorrectly
                rewards[player_num] = 0
        return rewards

    def generate_state_array(self, actions: list[int]):
        state_array = []
        for player, hand in enumerate(self.hands):
            opposite_player_actions = actions.copy()
            opposite_player_actions.pop(player)
            state_array.append([
                hand.features.rank.value,
                hand.features.first_card,
                hand.features.second_card,
                hand.features.third_card,
                hand.features.fourth_card,
                hand.features.fifth_card,
                hand.features.two_pair_potential.value,
                hand.features.three_of_a_kind_potential.value,
                hand.features.straight_potential.value,
                hand.features.flush_potential.value,
                hand.features.full_house_potential.value,
                hand.features.four_of_a_kind_potential.value,
                hand.features.straight_flush_potential.value,
                opposite_player_actions[0],
                opposite_player_actions[1],
                opposite_player_actions[2],
                self.turn
            ])
        return state_array

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
        elif self.turn != Turn.FINAL_GUESS.value: # Deal 1 card out
            card = self.deck.draw()
            for hand in self.hands:
                hand.add_card(card)

        self.turn += 1 # Increment the turn
        done = self.turn == Turn.DONE.value

        states = self.generate_state_array(actions)

        return {"state": states, "reward": rewards, "done": done}
