#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX_PLAYERS 6  
#define MIN_PLAYERS 2
#define NUM_DICES 5

struct Player { 
    int id;
    pthread_t id_thread;
    char name[20];
    char dice[5];
};

struct Play {
    int id_last;
    int id_current;
    int dice;
    int number;
    int paco_bet; 
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
    play.dice = 0;
    play.number= 0;
    play.paco_bet  = 0; //false
    for(int i=0; i<6; i++){
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
    printf("What's your name?\n");
    scanf("%s",players[0].name);
    pthread_t dealer;


    pthread_create(&dealer,NULL,dealer_func,players);
    pthread_join(dealer,NULL);
    return 0;
}



void *dealer_func(void *args){
    int game_on = 1;
    int action;
    struct Player *players = (struct Player *) args;
    while(game_on == 1){
        printf("Continue game [1] Exit[2]\n");
        scanf("%d",&game_on);
        printf("Dice player: %d number player: %d\n",play.dice,play.number);
        for(int i=0; i<6;i++){
            if(players[i].id == 0)
            {
                //Jugador
                
                printf("jugador: %d\n",i);
                //actualitzem valors ultim jugador i actual
                play.id_last = play.id_current;
                play.id_current  = players[i].id;
                pthread_create(&players[i].id_thread, NULL, user_func,players);
                pthread_join(players[i].id_thread,NULL);
            }
            else if (players[i].id != -1)
            {
                //Maquina

                printf("jugador: %d\n",i);
                //actualitzem valors ultim jugador i actual
                play.id_last = play.id_current;
                play.id_current  = players[i].id;
                pthread_create(&players[i].id_thread, NULL, machine_func,players);
                pthread_join(players[i].id_thread,NULL);
            }
            else
            {
                printf("-1\n");
            }
        }      
    }
    pthread_exit(0);
}

void *user_func(void *args){
    struct Player *players = (struct Player *) args;
    int dice,number = 0;
    int action;
    printf("Bid [0] , Dudo [1], Exit[2]\n");
    scanf("%d", &action);
    if(action == 0){ //Bid
        while(dice > 6 || dice < 1 || number < 1 || (number <= play.number && dice <= play.dice))
        {
            printf("Bid: \n");
            scanf("%d %d", &dice, &number);

        }
        play.dice = dice;
        play.number = number;
    }
    else if (action == 1) //Dudo
    {
        int count = 0;
        printf("DUDO: %d %d\n",play.dice, play.number);
        
        //Contem el nombre de daus amb el numero play.number i el nombre de jokers 1
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
        printf("Count = %d",count);

        //Comprovem qui ha guanyat i restem un dau posantlo a -1
        if(play.number >= count){
            for(int i = 0; i < NUM_DICES; i++){
                if(players[0].dice[i] != -1){
                    players[0].dice[i] == -1;
                } 
            }
        }
        else
        {
            for(int i = 0; i < NUM_DICES; i++){
                if(players[play.id_last].dice[i] != -1){
                    players[play.id_last].dice[i] == -1;
                }
            }
        }
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
    int action = 0; //TODO: canviar
    //random bid or dudo todo
    if(action == 0) // Bid
    {
        //random dice and number
        while(dice > 6 || dice < 1 || number < 1 || (number <= play.number && dice <= play.dice)){
            srand(time(0));
            dice  = (rand() % 6)+1;
            number = (rand()% 12) +1;
        }
        play.number = number;
        play.dice = dice;
               
    }
    else // Dudo
    {
        int count = 0;
        printf("DUDO: %d %d\n",play.dice, play.number);
        
        //Contem el nombre de daus amb el numero play.number i el nombre de jokers 1
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
        printf("Count = %d",count);

        //Comprovem qui ha guanyat i restem un dau posantlo a -1
        if(play.number >= count){
            for(int i = 0; i < NUM_DICES; i++){
                if(players[0].dice[i] != -1){
                    players[0].dice[i] == -1;
                } 
            }
        }
        else
        {
            for(int i = 0; i < NUM_DICES; i++){
                if(players[play.id_last].dice[i] != -1){
                    players[play.id_last].dice[i] == -1;
                }
            }
        }
    }
    
    pthread_exit(0);

}