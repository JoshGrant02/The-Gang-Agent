# The Poker Table
Josh Grant  
12/08/2023  

This is a very basic implementation of a poker simulator. Due to time constraints of the lab, I was not able to get most of what I was hoping to get done, completed. I for some reason had originally thought that this lab was due Thursday of Finals week, but I learned on this Thursday (12/7) that I had one day left to complete it. My program does not have player interaction with the game, but it will deal cards out and calculate a winner. My initial idea was a very ambitious project. I have written an extensive amount of code to get this much functionality complete, and I explore the usage of sockets. While my entire app is not complete, I hope it is enough work for the scope of this project. Without further ado, Below is an explanation of the current functionality of my app.
## Functionality
To begin the app, run `./thePokerTable` program. It should greet you with a welcome message and then declare that it is waiting for players. To add a player, you must open a new wsl terminal and run the `./pokerPlayer` program. It will connect to a socket used by The Poker Table and persist that connection until The Poker Table gracefully terminates. A player cannot currently voluntarily leave the game (they can end their program, but it will cause unexpected behavior for The Poker Table. If the `./pokerPlayer` is ran when the The Poker Table is not running, it will display a courteous message and exit. Up to 10 players can join the game at once.

To start the game, type `start` in The Poker Table. A dealer will deal cards to each player at the table in standard texas-hold-em format, and then deal 5 cards to the table right away and reveal them. This is where I was not able to get the time to create betting rounds with communication from the players. The current program forgoes all betting and reveals all cards right away. Once all cards are reveled, the dealer will calculate who had the best hand, say who has the best hand, and print the 5 cards that were in the best hand. The dealer then waits for 10 seconds before automatically starting the next round of poker. 

You can type `quit` into The Poker Table at any time to queue the end of the game. Quitting will not immediately terminate the game, but it will cause the game to end at the end of the current hand (or deal one more hand if none is in progress). Once the hand finishes, all the players will be told that the game is over, and the game will end.

No other commands are currently implemented by either the table or the players.

## Multi-Threading
I use an extensive amount of multi-threading to complete this program. First off, the player is currently implemented as one thread, so there isn't any multithreading there. The table begins as a manager thread, but splits off into the following threads.
- **Manager:** Initial thread. Always running. Watches the console for commands and manages the other threads (including joining at the end)
- **Bouncer:** Thread creating upon program execution to listen for player connections using sockets.
- **Butler:** When the bouncer accepts a communication, a butler thread is spun off to manage the communication between that player and the table. There is one butler thread per player.
- **Dealer:** When the game is started, a dealer thread is created which deals cards to the Butlers (to ultimately get passed to the players) and manage the poker game, including calculating the winner.

When the signal for quit is ran, the bouncer attempts to join with all butlers before joining back with the manager, and the dealer joins with the manager. 

## Sockets
The IPC technique that we did not explore in another lab that I utilized in this lab is sockets. I set up server-client paradigm where The Poker Table acts as the server and listens for and manages streaming connections from the players (clients).

## Other IPC
I also used a mutex to lock and unlock the console for The Poker Player. As I have many threads that may be printing messages to the console at similar times, I made it so each thread will lock the console before printing so they don't get interrupted mid print, and they will unlock after printing. For almost every instance of using this mutex, is is directly and only surrounding the `printf` statement (there are a few exceptions), so I am pretty sure my program is deadlock free.

I also used "thread signals" to allow the dealer to signal the butlers when a card is ready to be handed to the players. While signal handlers are registered for the entire process, I learned that you can send a signal to a specific thread using `pthread_sigqueue`, so that is how I perform that communication

Shared memory/Heap. I don't explicitely use "shared memory" (shm), but as all my operations are in different threads, I make use of the heap to track the game state. I created a struct that stores all relevant data for the game that I initailized on the heap and created a static reference to in each file.

## Calculating Winning Hands
This is something I am especially proud of for this application, is that I formulated a way to encode the rank (strength) of each hand into a 16 bit integer, so once computed, the integers can simply be compared to determine which hand is better. All of the logic is implemented in the `handCalculator.c` file, and I have uploaded the file I used to plan and document this encoding as `Poker hand strength encoding.xlsx`.

## Bugs
While I tried to make the normal operation of this program as smooth as possible, due to time constraints, I was not able to check for/handle every edge case, and my program does have some bugs. The most common bug is if you either terminate the table or player forcefully (ctrl+C), the other party will not know that the connection has been broken, and may encounter unintentional behavior.

Additionally, Valgrind does not like my program. While I tried to be conciencious of freeing memory and joining threads upon termination, running Valgrind with my program encounters a lot of errors immediately related to mutices and conditional jumps on uninitialized values. I am not sure if this is in part due to my program being multithreaded and Valgrind not being able to handle it, but I am assuming it is not all that, and I do have some memory issues that I have not realized. I realize that having no memory leaks is a basic requirement for this lab and that I do likely fall short on it.

## Ambitions / Future Dev
While I didn't get as much functionality as I was hoping completed for this lab, I chose this project because I thought it is one that would be really cool to complete and that it is one I would genuinely be interested in pursuing further after the class is over. I cannot give any gurantees as to my timeline for completing this project, but over time, I would like to eventually encorporate the following features in order:
1. **Playable Game:** Make it an actual playable game where the player processes can communicate with the table process to place table bets, etc.
2. **Player Balances / Rudimentary Login:** I would like to implement a feature to allow a player to "login" to their account, where their current balance will be persisted in a file that is read and written to by the table. A player can "buy in" for more to increase their balance (buying in wouldn't cost any real money and would just be a functionality for functionality's sake).
3. **Network Sockets:** This will either be the hardest one to implement or the easiest one to implement, depending on how difficult it is to switch the socket type and handle network communication. I would like to be able to play this over hte internet, so I can give someone the player executable and they can connect to the table over the internet. This will also require, however, either all players being in the same room to stil see the table screen, or communcation being adjusted so each player will also see the game running on their own screen.
4. **GUI:** The application currently has a text UI in the terminal. A fully completed application will have a GUI.