from game import TheGang

if __name__ == "__main__":
    environment = TheGang()
    environment.reset()
    environment.step([1, 2, 3, 4])
    environment.step([1, 2, 3, 4])
    environment.step([1, 2, 3, 4])
    environment.step([1, 2, 3, 4])