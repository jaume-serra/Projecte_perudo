#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>


#define MAXN 40
#define MAX_PLAYERS 6  
#define MIN_PLAYERS 2
#define NUM_DICES 5

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
    int current_dices;
};

struct Play play;
void *pro_machine_func(void *arg);
void *dealer_func(void *args);
void *user_func(void *args);
void *machine_func(void *args);
void init_game(void *args);
double calc_prob(int dices, int number);

unsigned long int t[MAXN+1][MAXN+1]; //Taula binomial
unsigned long int calc_binomial(int n, int x); //Funcio per crear la taula binomial


int main(){
    //INIT
    srand(time(0));
    int num_players = 0;
    struct Player players[MAX_PLAYERS];
    init_game(players);
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
    play.current_dices = num_players * NUM_DICES;
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
                //Printem el taulell
                for(int k=0; k < MAX_PLAYERS; k++)
                {
                    if(players[k].id  != -1){
                        for(int l=0; l < NUM_DICES; l++)
                        {
                            if(players[k].id == 0)
                            {
                                if(players[k].dice[l] != -1)
                                {
                                    printf("%d",players[k].dice[l]);
                                }
                            }
                            else
                            {
                                if(players[k].dice[l] != -1)
                                {
                                    printf("X");
                                }


                            }
                        }
                        printf("|");
                    }


                    
                }
                printf("\n");
                //Jugador
                //actualitzem valors ultim jugador i actual
                printf("-------------------------\n");
                printf("Continue game [1] Exit[2]\n");

                scanf("%d",&game_on);
                if(game_on == 2) break;
                
                play.id_last = play.id_current;
                play.id_current  = players[i].id;
                
                
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
                
                pthread_create(&players[i].id_thread, NULL, pro_machine_func,players);
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
                play.current_dices -= 1;

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
                play.current_dices -= 1;

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
    if(play.dice == 0 || play.number == 0){//comprovar que no comenci el torn
        printf("Bid[0] Exit[2]\n");
        scanf("%d", &action); 
        while(action == 1)
        {
            printf("Bid[0] Exit[2]\n");
            scanf("%d", &action); 
        }
    }
    else
    {
        printf("Bid [0] , Dudo [1], Exit[2]\n");
        scanf("%d", &action); 
        
    }
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
           
        
    if(play.dice == 0 || play.number == 0){ //comprovar que no comenci el torn
        action = 0;
    }

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


void *pro_machine_func(void *args){
    struct Player *players = (struct Player *) args;
    double prob_dice, prob_number = 0;
    int count_dices,count_number = 0;
    if(play.dice == 0 || play.number == 0){ //comprovar que no comenci el torn
        play.dice = 2; //fem bid de dau 2 number 1 -> mínim
        play.number = 1;
    }

    else //Calculem probabilitat i decidim accio
    {
        /*
        
        1- Recorre els daus del jugador
        2- Contar quants daus com play.dice o 1 té el jugador
        3- Calcular prob restant daus jugador i el nombre real
        
        */
        for(int i = 0; i < NUM_DICES; i++)
        {
            if(players[play.id_current].dice[i] != -1 && (players[play.id_current].dice[i] == play.dice || players[play.id_current].dice[i] == 1))
            {
                count_number++;
            }
            count_dices ++;
        }
        if(play.dice < 6)  prob_dice = calc_prob(play.current_dices-count_dices, play.number-count_number);
        prob_number = calc_prob(play.current_dices-count_dices, play.number-count_number+1);
        printf("Calculs prob dau: %f i prob numb: %f\n",prob_dice,prob_number);        
        
        if( prob_dice > 0.40 || prob_number > 0.40)//Bid
        {
            printf("Normal bet\n");
            if(prob_dice > prob_number)
            {
                play.dice += 1;
            }
            else
            {
                play.number += 1;
            }
        }
        else //Dudo
        {
            printf("Dudo bet\n");

            dudo(players);
            play.dice = 0;
            play.number = 0;
            shuffle_dices(players);
        }
    }
    pthread_exit(0);
}

unsigned long int calc_binomial(int m, int x) {
    int n, k;

    for (k = 0; k<=MAXN; k++) t[0][k] = 0;
    for (n = 1; n<=MAXN; n++) t[n][0] = 1;

    t[1][1] = 1;
    for (n = 2; n<=MAXN; n++) {
        for (k = 1; k < n; k++)
            t[n][k] = t[n-1][k-1] + t[n-1][k];
        t[n][n] = 1;
    }


    return t[m][x];
}

double calc_prob(int dices, int number)
{

    //Variables probabilitat

    double p = 0.33;    
    double q = 0.66;
    double prob = 0;

    //Calcul probabilitat        

    for(int j= 0 ; j <= number; j++)
    {
        prob += calc_binomial(dices, j) * pow(p,j) * pow(q,dices-j);
    }
    return 1-prob;
}