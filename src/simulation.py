from game import TheGang

if __name__ == "__main__":
    environment = TheGang()
    environment.reset()
    environment.step([0, 0, 0, 0])
    environment.step([0, 0, 0, 0])
    environment.step([0, 0, 0, 0])
    environment.step([0, 0, 0, 0])