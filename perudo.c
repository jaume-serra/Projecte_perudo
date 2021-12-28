#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define MAX_PLAYERS 6  
#define MIN_PLAYERS 2
#define NUM_DICES 2

struct Player { 
    int id;
    pthread_t id_thread;
    char dice[5];
};

struct Play {
    int id_last;
    int id_current;
    int dice;
    int number;
    int paco_bet; 
    int current_players;
};

struct Play play;

void *dealer_func(void *args);
void *user_func(void *args);
void *machine_func(void *args);






int main(){
    //INIT
    srand(time(0));
    int num_players = 0;

    while(num_players > MAX_PLAYERS || num_players < MIN_PLAYERS){
        printf("How many players?\n");
        scanf("%d",&num_players);
    }
    
    struct Player players[num_players];
    play.id_current = 0;
    play.id_last = 0;
    play.dice = 1;
    play.number= 4;
    play.paco_bet  = 0; //false
    play.current_players = num_players;
    for(int i=0; i<MAX_PLAYERS; i++){
        if(i < num_players){
            players[i].id = i;
            for(int j = 0 ; j < NUM_DICES; j++){
                players[i].dice[j] = (rand() % 6)+1;
            }
        }
        else
        {
            players[i].id = -1;
        }
      
    }

    pthread_t dealer;


    pthread_create(&dealer,NULL,dealer_func,players);
    pthread_join(dealer,NULL);
    return 0;
}

void init_game(void *args){

    struct Player *players = (struct Player *) args;
    srand(time(0));
    int num_players = 0;

    while(num_players > MAX_PLAYERS || num_players < MIN_PLAYERS){
        printf("How many players?\n");
        scanf("%d",&num_players);
    }
    
    play.id_current = 0;
    play.id_last = 0;
    play.dice = 0;
    play.number= 0;
    play.paco_bet  = 0; //false
    play.current_players = num_players;
    for(int i=0; i<MAX_PLAYERS; i++){
        if(i < num_players){
            players[i].id = i;
            for(int j = 0 ; j < NUM_DICES; j++){
                players[i].dice[j] = (rand() % 6)+1;
            }
        }
        else
        {
            players[i].id = -1;
        }
      
    }
}

void *dealer_func(void *args){
    int game_on = 1;
    int action;
    struct Player *players = (struct Player *) args;
    while(game_on == 1){
        for(int i=0; i < MAX_PLAYERS; i++){
            if(play.current_players == 1){
                for(int k=0; k < MAX_PLAYERS; k++)
                {
                    if(players[k].id != -1)
                    {
                        printf("S'ha acabat el joc.\n");
                        printf("El jugador %d ha guanyat la partida\n",players[k].id);                      
                    }
                }
                int result;
                printf("Tornar a jugar [1] o sortir [2] : ");
                scanf("%d",&result);
                if(result == 2){
                    game_on = 2;
                    break;
                }
                init_game(players);
            }
            if(players[i].id == 0)
            {
                //Jugador
                //actualitzem valors ultim jugador i actual
                printf("-------------------------\n");
                printf("Continue game [1] Exit[2]\n");
                scanf("%d",&game_on);
                if(game_on == 2) break;
                
                play.id_last = play.id_current;
                play.id_current  = players[i].id;
                
                printf("Actual player: %d Last player: %d \n",play.id_current,play.id_last);
                printf("Actual bet: D:%d N:%d\n",play.dice,play.number);
                
                pthread_create(&players[i].id_thread, NULL, user_func,players);
                pthread_join(players[i].id_thread,NULL);
                
                printf("-------------------------\n");
                printf("Daus: \n");
                for (int j = 0; j < NUM_DICES; j++)
                {
                    printf("%d ",players[i].dice[j]);
                }
                
                printf("\n-------------------------\n");

            }
            else if (players[i].id != -1)
            {
                //Maquina
                //actualitzem valors ultim jugador i actual
                printf("-------------------------\n");

                play.id_last = play.id_current;
                play.id_current = players[i].id;

                printf("Actual player: %d Last player: %d \n",play.id_current,play.id_last);
                printf("Actual bet: D:%d N:%d\n",play.dice,play.number);
                
                pthread_create(&players[i].id_thread, NULL, machine_func,players);
                pthread_join(players[i].id_thread,NULL);
                
                printf("-------------------------\n");
                printf("Daus: \n");
                for (int j = 0; j < NUM_DICES; j++)
                {

                    printf("%d ",players[i].dice[j]);
                }
                
                printf("\n-------------------------\n");
            }
       
        }
    }
    pthread_exit(0);
}
void shuffle_dices(void *args){
    struct Player *players = (struct Player *) args;
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(players[i].id != -1 )
        {
            for(int j= 0; j < NUM_DICES; j++)
            {
                if(players[i].dice[j] != -1) players[i].dice[j] = (rand() % 6)+1;
            }
        }
    }
}


void dudo(void *args){

    struct Player *players = (struct Player *) args;
    int count = 0;
        
    //Contem el nombre de daus amb el numero play.number i el nombre de jokers (1)

    for(int i = 0; i < MAX_PLAYERS; i++){
        if(players[i].id != -1){
            for(int j = 0; j < NUM_DICES; j++)
            {   
                if(players[i].dice[j] == play.dice || players[i].dice[j] == 1)
                {
                    count++;
                }
            }   
        }
    }
    
    printf("Count Dudo = %d\n",count);

    //Comprovem qui ha guanyat i restem un dau posantlo a -1
    if(count > play.number){
        for(int i = 0; i < NUM_DICES; i++){
            if(players[play.id_current].dice[i] != -1){
                players[play.id_current].dice[i] = -1;
                printf("\nJugador %d Perd dau \n",players[play.id_current].id);

                if(i ==(NUM_DICES-1)){
                    printf("\nJugador %d Eliminat \n",players[play.id_current].id);
                    players[play.id_current].id = -1;
                    play.current_players -= 1;
                }
                break;
            } 
        }
    }
    else
    {
        for(int i = 0; i < NUM_DICES; i++){
            if(players[play.id_last].dice[i] != -1){
                players[play.id_last].dice[i] = -1;
                printf("\nJugador %d Perd dau \n",players[play.id_last].id);

                if(i ==(NUM_DICES-1)){
                    printf("\nJugador %d Eliminat \n",players[play.id_last].id);

                    players[play.id_last].id = -1;
                    play.current_players -= 1;

                }
                break;
            }
        }
    }



}
void *user_func(void *args){
    struct Player *players = (struct Player *) args;
    int dice,number = 0;
    int action;
    printf("Bid [0] , Dudo [1], Exit[2]\n");
    scanf("%d", &action);
    if(action == 0) //Bid
    { 
        while(dice > 6 || dice < 1 || number < 1 || number < play.number || (dice <= play.dice && number <= play.number) || dice < play.dice )
        {
            printf("Bid: \n");
            scanf("%d %d", &dice, &number);
            if(dice == 1 && number >= round(play.number/2) && play.paco_bet == 0) //Bet on pacos
            {
                printf("Paco bet");
                play.paco_bet = 1;
                break;
            }

            if(dice != 1 && number >= (play.number*2)+1 && play.paco_bet == 1) //Returning normal bet
            {
                printf("Normal bet");
                play.paco_bet = 0;
                break;
            }
            
        }
        play.dice = dice;
        play.number = number;
    }
    else if (action == 1) //Dudo
    {
        
        dudo(players);
        play.dice = 0;
        play.number = 0;
        shuffle_dices(players);

        
    }
    else if (action == 2) //Exit
    {
        pthread_exit(0);

    }
        
    pthread_exit(0);
  
}

void *machine_func(void *args){
    struct Player *players = (struct Player *) args;
    int dice,number = 0;
    int action = rand() % 2;
    printf("action: %d\n",action);
    if(action == 0) // Bid
    {
        //random dice and number
        while(dice > 6 || dice < 1 || number < 1 || number < play.number || (dice <= play.dice && number <= play.number) || dice < play.dice ){
            srand(time(0));
            dice  = (rand() % 6)+1;
            number = (rand()% 30) +1;
            
            if(dice == 1 && number >= round(play.number/2) && play.paco_bet == 0) //Bet on pacos
            {
                printf("Paco bet");
                play.paco_bet = 1;
                break;
            }

            if(dice != 1 && number >= (play.number*2)+1 && play.paco_bet == 1) //Returning normal bet
            {
                printf("Normal bet");
                play.paco_bet = 0;
                break;
            }
            
        }
        play.number = number;
        play.dice = dice;
    }
    else // Dudo
    {
        dudo(players);
        play.dice = 0;
        play.number = 0;
        shuffle_dices(players);

    }
    
    
    pthread_exit(0);

}