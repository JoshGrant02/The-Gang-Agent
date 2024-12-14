from models import Card, Suit, HandFeatures, Potential, Rank, MAX_CARDS
import numpy as np

class Hand:
    def __init__(self, cards: list[Card] = None, player_num: int = None):
        self.player_num = player_num
        if cards == None:
            self.cards = []
        else:
            self.cards = cards
        self.features = HandFeatures()

    def recalculate_hand_features(self):
        sorted_cards = sorted(self.cards, key=lambda x: x.value)
        cards_left = MAX_CARDS - len(sorted_cards)
        card_counts = [0 for _ in range(13)]
        for card in sorted_cards:
            card_counts[card.value - 2] = card_counts[card.value - 2] + 1
        pair_counts = 0
        trip_counts = 0
        quad_counts = 0
        for count in card_counts:
            if count == 4:
                quad_counts += 1
            elif count == 3:
                trip_counts += 1
            elif count == 2:
                pair_counts += 1
        pair_eligibles = pair_counts + trip_counts + quad_counts
        trip_eligibles = trip_counts + quad_counts

        pair_potentiontial = Hand.calculate_pair_potential(pair_eligibles, cards_left)
        two_pair_potential = Hand.calculate_two_pair_potential(pair_eligibles, cards_left)
        trip_potential = Hand.calculate_trips_potential(pair_counts, trip_eligibles, cards_left)
        straight_potential, straight_hand = Hand.calculate_straight_potential_and_hand(sorted_cards, cards_left)
        flush_potential, flush_hand = Hand.calculate_flush_potential_and_hand(sorted_cards, cards_left)
        full_house_potential = Hand.calculate_full_house_potential(pair_counts, pair_eligibles, trip_eligibles, cards_left)
        quad_potential = Hand.calculate_quads_potential(quad_counts, trip_counts, pair_counts, cards_left)
        straight_flush_potential, straight_flush_hand = Hand.calculate_straight_flush_potential_and_hand(sorted_cards, cards_left)
        
        self.features.two_pair_potential = two_pair_potential
        self.features.three_of_a_kind_potential = trip_potential
        self.features.straight_potential = straight_potential
        self.features.flush_potential = flush_potential
        self.features.full_house_potential = full_house_potential
        self.features.four_of_a_kind_potential = quad_potential
        self.features.straight_flush_potential = straight_flush_potential

        rank = Rank.HIGH_CARD
        hand = [card.value for card in sorted_cards[::-1]]
        hand.extend([0, 0, 0])
        if straight_flush_potential == Potential.HAS_RANK:
            rank = Rank.STRAIGHT_FLUSH
            hand = straight_flush_hand
        elif quad_potential == Potential.HAS_RANK:
            rank = Rank.FOUR_OF_A_KIND
            hand = Hand.calculate_quads_hand(card_counts)
        elif full_house_potential == Potential.HAS_RANK:
            rank = Rank.FULL_HOUSE
            hand = Hand.calculate_full_house_hand(card_counts)
        elif flush_potential == Potential.HAS_RANK:
            rank = Rank.FLUSH
            hand = flush_hand
        elif straight_potential == Potential.HAS_RANK:
            rank = Rank.STRAIGHT
            hand = straight_hand
        elif trip_potential == Potential.HAS_RANK:
            rank = Rank.THREE_OF_A_KIND
            hand = Hand.calculate_trips_hand(card_counts)
        elif two_pair_potential == Potential.HAS_RANK:
            rank = Rank.TWO_PAIR
            hand = Hand.calculate_two_pair_hand(card_counts)
        elif pair_potentiontial == Potential.HAS_RANK:
            rank = Rank.ONE_PAIR
            hand = Hand.calculate_pair_hand(card_counts, cards_left)

        self.features.rank = rank
        self.features.first_card = hand[0]
        self.features.second_card = hand[1]
        self.features.third_card = hand[2]
        self.features.fourth_card = hand[3]
        self.features.fifth_card = hand[4]

    @staticmethod
    def calculate_pair_potential(pair_eligibles: int, cards_left: int) -> Potential:
        potentional = Potential.IMPOSSIBLE
        if pair_eligibles >= 1:
            potentional = Potential.HAS_RANK
        elif cards_left >= 1:
            potentional = Potential.POSSIBLE

        return potentional

    @staticmethod
    def calculate_pair_hand(card_counts: list[int], cards_left: int) -> list[int]:
        pair_card = 14 - np.argmax(card_counts[::-1])
        hand_cards = [pair_card]*2
        hand_length = 1
        reverse_index = 1
        while hand_length < 5 and reverse_index < 13:
            if card_counts[-reverse_index] > 0 and 15 - reverse_index != pair_card:
                hand_cards.append(15 - reverse_index)
                hand_length += 1
            reverse_index += 1
        while len(hand_cards) < 5:
            hand_cards.append(0)
        return hand_cards

    @staticmethod
    def calculate_two_pair_potential(pair_eligibles: int, cards_left: int) -> Potential:
        potential = Potential.IMPOSSIBLE
        if pair_eligibles >= 2:
            potential = Potential.HAS_RANK
        elif pair_eligibles == 1 and cards_left >= 1 and cards_left < 5:
            potential = Potential.ONE_CARD_AWAY
        elif cards_left >= 2:
            potential = Potential.POSSIBLE

        return potential
    
    @staticmethod
    def calculate_two_pair_hand(card_counts: list[int]) -> list[int]:
        breakpoint()
        card_counts_copy = card_counts.copy()
        two_pair_major = 14 - np.argmax(card_counts_copy[::-1])
        card_counts_copy[two_pair_major - 2] = 0
        two_pair_minor = 14 - np.argmax(card_counts_copy[::-1])
        kicker = 0
        for card_index in range(len(card_counts))[::-1]:
            if card_counts[card_index] > 0 and card_index + 2 != two_pair_major and card_index + 2 != two_pair_minor:
                kicker = card_index + 2
                break
        return [two_pair_major]*2 + [two_pair_minor]*2 + [kicker]

    @staticmethod
    def calculate_trips_potential(pair_counts: int, trip_eligibles: int, cards_left: int) -> tuple[Potential, list | None]:
        hand_cards = None
        potential = Potential.IMPOSSIBLE
        if trip_eligibles >= 1:
            potential = Potential.HAS_RANK
        elif pair_counts >= 1 and cards_left >= 1:
            potential = Potential.ONE_CARD_AWAY
        elif cards_left >= 2:
            potential = Potential.POSSIBLE

        return (potential, hand_cards)

    @staticmethod
    def calculate_trips_hand(card_counts: list[int]) -> list[int]:
        trip_card = 14 - np.argmax(card_counts[::-1])
        hand_cards = [trip_card]*3
        for card_index in range(len(card_counts))[::-1]:
            if card_counts[card_index] > 0 and card_index != trip_card:
                hand_cards.append(card_index + 2)
                break
        for card_index in range(len(card_counts))[:hand_cards[3]-2:-1]:
            if card_counts[card_index] > 0 and card_index != trip_card:
                hand_cards.append(card_index + 2)
                break

    @staticmethod
    def calculate_straight_potential_and_hand(sorted_cards: list[Card], cards_left: int) -> tuple[Potential, list[int] | None]:
        values = [card.value for card in sorted_cards]
        hand_cards = None
        straights = [
            [1, 2, 3, 4, 5],
            [2, 3, 4, 5, 6],
            [3, 4, 5, 6, 7],
            [4, 5, 6, 7, 8],
            [5, 6, 7, 8, 9],
            [6, 7, 8, 9, 10],
            [7, 8, 9, 10, 11],
            [8, 9, 10, 11, 12],
            [9, 10, 11, 12, 13],
            [10, 11, 12, 13, 14],
        ]
        cards_needed_for_straight = []
        for straight in straights:
            straight_cards_satisfied = 0
            for straight_card in straight:
                if straight_card in values:
                    straight_cards_satisfied += 1
            if straight_cards_satisfied == 5:
                hand_cards = straight
            cards_needed_for_straight.append(5 - straight_cards_satisfied)

        least_cards_needed = min(cards_needed_for_straight)
        if least_cards_needed == 0:
            potential = Potential.HAS_RANK
        elif least_cards_needed == 1 and cards_left >= 1:
            potential = Potential.ONE_CARD_AWAY
        elif least_cards_needed <= cards_left:
            potential = Potential.POSSIBLE
        else:
            potential = Potential.IMPOSSIBLE

        return (potential, hand_cards)

    @staticmethod
    def calculate_flush_potential_and_hand(sorted_cards: list[Card], cards_left: int) -> tuple[Potential, list[int] | None]:
        suits = [card.suit for card in sorted_cards]
        heart_counts = 0
        diamond_counts = 0
        spade_counts = 0
        club_counts = 0
        hand_cards = None

        for suit in suits:
            if suit == Suit.HEART.value:
                heart_counts += 1
            elif suit == Suit.DIAMOND.value:
                diamond_counts += 1
            elif suit == Suit.SPADE.value:
                spade_counts += 1
            elif suit == Suit.CLUB.value:
                club_counts += 1

        cards_needed_for_flush = 5 - max(heart_counts, diamond_counts, spade_counts, club_counts)
        if cards_needed_for_flush <= 0:
            potential = Potential.HAS_RANK
            flush_suit = np.argmax([heart_counts, diamond_counts, spade_counts, club_counts])
            num_suited = 0
            reverse_index = 1
            hand_cards = []
            while num_suited < 5:
                if sorted_cards[-reverse_index].suit == flush_suit:
                    hand_cards.append(sorted_cards[-reverse_index].value)
                    num_suited += 1
                reverse_index += 1
        elif cards_needed_for_flush == 1 and cards_left >= 1:
            potential = Potential.ONE_CARD_AWAY
        elif cards_needed_for_flush <= cards_left:
            potential = Potential.POSSIBLE
        else:
            potential = Potential.IMPOSSIBLE

        return (potential, hand_cards)

    @staticmethod
    def calculate_full_house_potential(pair_counts: int, pair_eligibles: int, trip_eligibles: int, cards_left: int) -> tuple[Potential, list[int] | None]:
        hand_cards = None
        potential = Potential.IMPOSSIBLE
        if trip_eligibles >= 2 or (trip_eligibles == 1 and pair_counts >= 1):
            potential = Potential.HAS_RANK
        elif trip_eligibles >= 1 and cards_left >= 1: # Has trips, needs pair
            potential = Potential.ONE_CARD_AWAY
        elif pair_eligibles >= 2 and cards_left >= 1: # Has two pair, needs trip upgrade
            potential = Potential.ONE_CARD_AWAY
        elif pair_eligibles >= 1 and cards_left >= 2: # Has pair, needs second pair and trip upgrade
            potential = Potential.POSSIBLE
        elif cards_left >= 3: # Assume at least 2 cards are out
            potential = Potential.POSSIBLE

        return (potential, hand_cards)

    @staticmethod
    def calculate_full_house_hand(card_counts: list[int]) -> list[int]:
        card_counts_copy = card_counts.copy()
        full_house_major = 14 - np.argmax(card_counts_copy[::-1])
        card_counts_copy[full_house_major - 2] = 0
        full_house_minor = 14 - np.argmax(card_counts_copy[::-1])
        return [full_house_major]*3 + [full_house_minor]*2
        
    @staticmethod
    def calculate_quads_potential(quad_counts: int, trip_counts: int, pair_counts: int, cards_left: int) -> tuple[Potential, list[int] | None]:
        hand_cards = None
        potential = Potential.IMPOSSIBLE
        if quad_counts >= 1:
            potential = Potential.HAS_RANK
        elif trip_counts >= 1 and cards_left >= 1:
            potential = Potential.ONE_CARD_AWAY
        elif pair_counts >= 1 and cards_left >= 2:
            potential = Potential.POSSIBLE
        elif cards_left >= 3:
            potential = Potential.POSSIBLE

        return (potential, hand_cards)

    @staticmethod
    def calculate_quads_hand(card_counts: list[int]) -> list[int]:
        quad_card = np.argmax(card_counts) + 2 # Add 2 to argmax because index 0 is 2
        hand_cards = [quad_card]*4
        if len(card_counts) == 4:
            hand_cards.append(0)
        else:
            for card_index in range(len(card_counts))[::-1]:
                if card_counts[card_index] > 0 and card_index != quad_card:
                    hand_cards.append(card_index + 2)
                    break
        return hand_cards

    @staticmethod
    def calculate_straight_flush_potential_and_hand(sorted_cards: list[Card], cards_left: int) -> tuple[Potential, list[int] | None]:
        suits = [Suit.HEART, Suit.DIAMOND, Suit.SPADE, Suit.CLUB]
        values = []
        hand_cards = None
        for suit in suits:
            values.append([card.value for card in sorted_cards if card.suit == suit.value])
        straights = [
            [1, 2, 3, 4, 5],
            [2, 3, 4, 5, 6],
            [3, 4, 5, 6, 7],
            [4, 5, 6, 7, 8],
            [5, 6, 7, 8, 9],
            [6, 7, 8, 9, 10],
            [7, 8, 9, 10, 11],
            [8, 9, 10, 11, 12],
            [9, 10, 11, 12, 13],
            [10, 11, 12, 13, 14],
        ]
        cards_needed_for_straight_flush = []
        for straight in straights: # Loop through all possible straights
            for suit_index in range(len(suits)): # Loop through all suits
                straight_flush_cards_satisfied = 0
                for straight_card in straight: # Loop through all cards in the straight
                    if straight_card in values[suit_index]: # Add card if in suit
                        straight_flush_cards_satisfied += 1
                if straight_flush_cards_satisfied == 5:
                    hand_cards = straight
                cards_needed_for_straight_flush.append(5 - straight_flush_cards_satisfied)

        least_cards_needed = min(cards_needed_for_straight_flush)
        if least_cards_needed == 0:
            potential = Potential.HAS_RANK
        elif least_cards_needed == 1 and cards_left >= 1:
            potential = Potential.ONE_CARD_AWAY
        elif least_cards_needed <= cards_left:
            potential = Potential.POSSIBLE
        else:
            potential = Potential.IMPOSSIBLE
        
        return (potential, hand_cards)

    def add_card(self, card: Card):
        self.cards.append(card)
        if len(self.cards) == 2 or len(self.cards) >= 5:
            self.recalculate_hand_features()

    def __str__(self):
        return ", ".join([card.__str__() for card in self.cards])


if __name__ == "__main__":
    hand = Hand()
    hand.add_card(Card(value=14,suit=Suit.HEART))
    hand.add_card(Card(value=8,suit=Suit.HEART))
    hand.add_card(Card(value=13,suit=Suit.HEART))
    hand.add_card(Card(value=2,suit=Suit.SPADE))
    hand.add_card(Card(value=2,suit=Suit.DIAMOND))
    hand.add_card(Card(value=3,suit=Suit.CLUB))
    hand.add_card(Card(value=3,suit=Suit.HEART))
