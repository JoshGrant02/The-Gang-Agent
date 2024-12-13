from models import Card, Suit, HandFeatures, Potential, Rank, MAX_CARDS

class Hand:
    def __init__(self, cards: list[Card] = None):
        if cards == None:
            self.cards = []
        else:
            self.cards = cards
        self.features = HandFeatures()

    def recalculate_hand_features(self):
        sorted_cards = sorted(self.cards, key=lambda x: x.value)
        cards_left = MAX_CARDS - len(sorted_cards)
        card_counts = [0 for _ in range(15)]
        for card in sorted_cards:
            card_counts[card.value] = card_counts[card.value] + 1
        pair_potention, two_pair_potential, trip_potential, full_house_potential, quad_potential = Hand.calculate_pair_like_potentials(card_counts, cards_left)
        straight_potential = Hand.calculate_straight_potential(sorted_cards, cards_left)
        flush_potential = Hand.calculate_flush_potential(sorted_cards, cards_left)
        straight_flush_potential = Hand.calculate_straight_flush_potential(sorted_cards, cards_left)
        
        self.features.two_pair_potential = two_pair_potential
        self.features.three_of_a_kind_potential = trip_potential
        self.features.straight_potential = straight_potential
        self.features.flush_potential = flush_potential
        self.features.full_house_potential = full_house_potential
        self.features.four_of_a_kind_potential = quad_potential
        self.features.straight_flush_potential = straight_flush_potential

        rank = Rank.HIGH_CARD
        if straight_flush_potential == Potential.HAS_RANK:
            rank = Rank.STRAIGHT_FLUSH
        elif quad_potential == Potential.HAS_RANK:
            rank = Rank.FOUR_OF_A_KIND
        elif full_house_potential == Potential.HAS_RANK:
            rank = Rank.FULL_HOUSE
        elif flush_potential == Potential.HAS_RANK:
            rank = Rank.FLUSH
        elif straight_potential == Potential.HAS_RANK:
            rank = Rank.STRAIGHT
        elif trip_potential == Potential.HAS_RANK:
            rank = Rank.THREE_OF_A_KIND
        elif two_pair_potential == Potential.HAS_RANK:
            rank = Rank.TWO_PAIR
        elif pair_potention == Potential.HAS_RANK:
            rank = Rank.ONE_PAIR

        self.features.rank = rank

    @staticmethod
    def calculate_pair_like_potentials(card_counts: list[int], cards_left: int) -> tuple[Potential, Potential, Potential, Potential]:
        num_cards = len(card_counts)
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

        pair_potentional = Potential.IMPOSSIBLE
        if pair_eligibles >= 1:
            pair_potentional = Potential.HAS_RANK
        elif cards_left >= 1:
            pair_potentional = Potential.POSSIBLE

        two_pair_potential = Potential.IMPOSSIBLE
        if pair_eligibles >= 2:
            two_pair_potential = Potential.HAS_RANK
        elif pair_eligibles == 1 and cards_left >= 1 and num_cards > 2:
            two_pair_potential = Potential.ONE_CARD_AWAY
        elif cards_left >= 2:
            two_pair_potential = Potential.POSSIBLE

        trip_potential = Potential.IMPOSSIBLE
        if trip_eligibles >= 1:
            trip_potential = Potential.HAS_RANK
        elif pair_counts >= 1 and cards_left >= 1:
            trip_potential = Potential.ONE_CARD_AWAY
        elif cards_left >= 2:
            trip_potential = Potential.POSSIBLE

        full_house_potential = Potential.IMPOSSIBLE
        if trip_eligibles >= 2 or (trip_eligibles == 1 and pair_counts >= 1):
            full_house_potential = Potential.HAS_RANK
        elif trip_eligibles >= 1 and cards_left >= 1: # Has trips, needs pair
            full_house_potential = Potential.ONE_CARD_AWAY
        elif pair_eligibles >= 2 and cards_left >= 1: # Has two pair, needs trip upgrade
            full_house_potential = Potential.ONE_CARD_AWAY
        elif pair_eligibles >= 1 and cards_left >= 2: # Has pair, needs second pair and trip upgrade
            full_house_potential = Potential.POSSIBLE
        elif cards_left >= 3: # Assume at least 2 cards are out
            full_house_potential = Potential.POSSIBLE

        quad_potential = Potential.IMPOSSIBLE
        if quad_counts >= 1:
            quad_potential = Potential.HAS_RANK
        elif trip_counts >= 1 and cards_left >= 1:
            quad_potential = Potential.ONE_CARD_AWAY
        elif pair_counts >= 1 and cards_left >= 2:
            quad_potential = Potential.POSSIBLE
        elif cards_left >= 3:
            quad_potential = Potential.POSSIBLE

        return pair_potentional, two_pair_potential, trip_potential, full_house_potential, quad_potential
    

    @staticmethod
    def calculate_two_pair_potential(card_counts: list[int], cards_left: int) -> Potential:
        '''
        values = [card.value for card in sorted_cards]
        pair_counts = 0
        i = 0
        while i < len(values) - 1:
            if values[i] == values[i + 1]:
                pair_counts += 1
                i += 1 # Skip paired card to not double count trips
            i += 1
        '''
        pair_counts = 0
        for count in card_counts:
            if count >= 2:
                pair_counts += 1
        if pair_counts >= 2:
            return Potential.HAS_RANK
        elif pair_counts == 1 and cards_left >= 1:
            return Potential.ONE_CARD_AWAY
        elif pair_counts == 0 and cards_left >= 2:
            return Potential.POSSIBLE
        else:
            return Potential.IMPOSSIBLE

    @staticmethod
    def calculate_three_of_a_kind_potential(sorted_cards: list[Card], cards_left: int) -> Potential:
        values = [card.value for card in sorted_cards]
        has_pair = False
        num_cards = len(values)
        i = 0
        while i < num_cards - 1:
            if i < num_cards - 2 and values[i] == values[i + 1] and values[i] == values[i + 2]:
                return Potential.HAS_RANK
            elif values[i] == values[i + 1] or values[i] == values[i + 2]:
                has_pair = True
                i += 1
            i += 1
        if has_pair and cards_left >= 1:
            return Potential.ONE_CARD_AWAY
        elif cards_left >= 2:
            return Potential.POSSIBLE
        else:
            return Potential.IMPOSSIBLE

    @staticmethod
    def calculate_straight_potential(sorted_cards: list[Card], cards_left: int) -> Potential:
        values = [card.value for card in sorted_cards]
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
            cards_needed_for_straight.append(5 - straight_cards_satisfied)

        least_cards_needed = min(cards_needed_for_straight)
        if least_cards_needed == 0:
            return Potential.HAS_RANK
        elif least_cards_needed == 1 and cards_left >= 1:
            return Potential.ONE_CARD_AWAY
        elif least_cards_needed <= cards_left:
            return Potential.POSSIBLE
        else:
            return Potential.IMPOSSIBLE

    @staticmethod
    def calculate_flush_potential(sorted_cards: list[Card], cards_left: int) -> Potential:
        suits = [card.suit for card in sorted_cards]
        heart_counts = 0
        diamond_counts = 0
        spade_counts = 0
        club_counts = 0

        breakpoint()
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
            return Potential.HAS_RANK
        elif cards_needed_for_flush == 1 and cards_left >= 1:
            return Potential.ONE_CARD_AWAY
        elif cards_needed_for_flush <= cards_left:
            return Potential.POSSIBLE
        else:
            return Potential.IMPOSSIBLE

    @staticmethod
    def calculate_straight_flush_potential(sorted_cards: list[Card], cards_left: int) -> Potential:
        suits = [Suit.HEART, Suit.DIAMOND, Suit.SPADE, Suit.CLUB]
        values = []
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
                cards_needed_for_straight_flush.append(5 - straight_flush_cards_satisfied)

        least_cards_needed = min(cards_needed_for_straight_flush)
        if least_cards_needed == 0:
            return Potential.HAS_RANK
        elif least_cards_needed == 1 and cards_left >= 1:
            return Potential.ONE_CARD_AWAY
        elif least_cards_needed <= cards_left:
            return Potential.POSSIBLE
        else:
            return Potential.IMPOSSIBLE

    def add_card(self, card: Card):
        self.cards.append(card)
        if len(self.cards) > 1:
            self.recalculate_hand_features()

    def __str__(self):
        return ", ".join([card.__str__() for card in self.cards])


if __name__ == "__main__":
    hand = Hand()
    hand.add_card(Card(value=6,suit=Suit.SPADE))
    hand.add_card(Card(value=5,suit=Suit.HEART))
    hand.add_card(Card(value=10,suit=Suit.HEART))
    hand.add_card(Card(value=7,suit=Suit.SPADE))
    hand.add_card(Card(value=3,suit=Suit.HEART))
    hand.add_card(Card(value=2,suit=Suit.HEART))
