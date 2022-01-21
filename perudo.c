#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define MAXN 40
#define MAX_PLAYERS 6
#define MIN_PLAYERS 2
#define NUM_DICES 5

struct Player
{
    int id;
    pthread_t id_thread;
    char dice[5];
};

struct Play
{
    int id_last;
    int id_current;
    int dice;
    int number;
    int paco_bet;
    int current_players;
    int current_dices;
    int level;
    int palifico;
};

struct Play play;
struct Player players[MAX_PLAYERS];

void *pro_machine_func();
void *dealer_func();
void *user_func();
void *machine_func();
void init_game();
double calc_prob(int dices, int number);

unsigned long int taula_binomial[MAXN + 1][MAXN + 1];       // Taula binomial
unsigned long int calc_binomial(); // Funcio per crear la taula binomial

/*
TODO:
    - Calcular una vegada només la binomial
        ? Despres del tria jugador o sempre


    - Comprovar placifico Bet

    - Afegir param p i q a la funcio calc_prob

    ----------------------------------------
    - Afegir/eliminar jugadors


*/

int main()
{
    srand(time(0));
    init_game();
    calc_binomial();
    pthread_t dealer;

    pthread_create(&dealer, NULL, dealer_func, NULL);
    pthread_join(dealer, NULL);
    return 0;
}

void init_game()
{

    srand(time(0));
    int num_players = 0;
    int level = -1;

    while (num_players > MAX_PLAYERS || num_players < MIN_PLAYERS)
    {
        printf("How many players?\n");
        scanf("%d", &num_players);
    }

    while (level != 0 && level != 1)
    {
        printf("Dumm Players [0] or Smart Players [1]\n");
        scanf("%d", &level);
    }

    play.level = level;
    play.id_current = 0;
    play.id_last = 0;
    play.dice = 0;
    play.number = 0;
    play.paco_bet = 0; // Init a false
    play.palifico = 0; // Init a false
    play.current_players = num_players;
    play.current_dices = num_players * NUM_DICES;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (i < num_players)
        {
            players[i].id = i;
            for (int j = 0; j < NUM_DICES; j++)
            {
                players[i].dice[j] = (rand() % 6) + 1;
            }
        }
        else
        {
            players[i].id = -1;
        }
    }
}

void *dealer_func()
{
    int game_on = 1;
    int action;
    char taulell[6] = "EXYZQW";
    while (game_on == 1)
    {
        for (int i = 0; i < MAX_PLAYERS; i++)
        {

            if (play.current_players == 1)
            {
                for (int k = 0; k < MAX_PLAYERS; k++)
                {
                    if (players[k].id != -1)
                    {
                        printf("S'ha acabat el joc.\n");
                        printf("El jugador %d ha guanyat la partida\n", players[k].id);
                    }
                }
                int result;
                printf("Tornar a jugar [1] o sortir [2] : ");
                scanf("%d", &result);
                if (result == 2)
                {
                    game_on = 2;
                    break;
                }
                init_game(players);
            }

            if (players[i].id == 0)
            {
                // Printem el taulell
                for (int k = 0; k < MAX_PLAYERS; k++)
                {
                    if (players[k].id != -1)
                    {
                        for (int l = 0; l < NUM_DICES; l++)
                        {
                            if (players[k].id == 0)
                            {
                                if (players[k].dice[l] != -1)
                                {
                                    printf("%d", players[k].dice[l]);
                                }
                            }
                            else
                            {
                                if (players[k].dice[l] != -1)
                                {
                                    printf("%c", taulell[k]);
                                }
                            }
                        }
                        printf("|");
                    }
                }
                printf("\n");
                // Jugador
                // actualitzem valors ultim jugador i actual
                printf("-------------------------\n");
                printf("Continue game [1] Exit[2]\n");

                scanf("%d", &game_on);
                if (game_on == 2)
                    break;

                play.id_last = play.id_current;
                play.id_current = players[i].id;

                pthread_create(&players[i].id_thread, NULL, user_func, NULL);
                pthread_join(players[i].id_thread, NULL);

                printf("-------------------------\n");
                // sleep(1);
            }
            else if (players[i].id != -1)
            {
                // Maquina
                // actualitzem valors ultim jugador i actual
                printf("-------------------------\n");

                play.id_last = play.id_current;
                play.id_current = players[i].id;

                printf("Actual player: %d \n", play.id_current);
                printf("Last bet: D:%d N:%d\n", play.dice, play.number);
                if (play.level == 0)
                {
                    pthread_create(&players[i].id_thread, NULL, machine_func, NULL);
                    pthread_join(players[i].id_thread, NULL);
                }
                else
                {
                    pthread_create(&players[i].id_thread, NULL, pro_machine_func, NULL);
                    pthread_join(players[i].id_thread, NULL);
                }

                printf("Player bet: D:%d N:%d\n", play.dice, play.number);

                printf("-------------------------\n");
                printf("\n");
                sleep(1);
            }
        }
    }
    pthread_exit(0);
}
void shuffle_dices()
{

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].id != -1)
        {
            for (int j = 0; j < NUM_DICES; j++)
            {
                if (players[i].dice[j] != -1)
                    players[i].dice[j] = (rand() % 6) + 1;
            }
        }
    }
}

void dudo()
{

    int count = 0;
    play.paco_bet = 0;
    // Contem el nombre de daus amb el numero play.number i el nombre de jokers (1)

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (players[i].id != -1)
        {
            for (int j = 0; j < NUM_DICES; j++)
            {
                if (players[i].dice[j] == play.dice || players[i].dice[j] == 1)
                {
                    count++;
                }
            }
        }
    }

    printf("Count Dudo = %d\n", count);

    // Comprovem qui ha guanyat i restem un dau posantlo a -1
    if (count >= play.number)
    {
        for (int i = 0; i < NUM_DICES; i++)
        {
            if (players[play.id_current].dice[i] != -1)
            {
                players[play.id_current].dice[i] = -1;
                printf("\nJugador %d Perd dau \n", players[play.id_current].id);
                play.current_dices -= 1;
                if (i == (NUM_DICES - 2))
                {
                    printf("\nPalifico\n");
                    play.palifico += 1;
                }
                if (i == (NUM_DICES - 1))
                {
                    printf("\nJugador %d Eliminat \n", players[play.id_current].id);
                    players[play.id_current].id = -1;
                    play.current_players -= 1;
                    play.palifico -= 1;
                }
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < NUM_DICES; i++)
        {
            if (players[play.id_last].dice[i] != -1)
            {
                players[play.id_last].dice[i] = -1;
                printf("\nJugador %d Perd dau \n", players[play.id_last].id);
                play.current_dices -= 1;
                if (i == (NUM_DICES - 2))
                {
                    printf("\nPalifico\n");
                    play.palifico += 1;
                }
                if (i == (NUM_DICES - 1))
                {
                    printf("\nJugador %d Eliminat \n", players[play.id_last].id);

                    players[play.id_last].id = -1;
                    play.current_players -= 1;
                    play.palifico -= 1;
                }
                break;
            }
        }
    }
}
void *user_func()
{
    int dice, number = 0;
    int action;
    int paco_bet = 0;

    if (play.dice == 0 || play.number == 0)
    { // comprovar que no comenci el torn
        action = 0;
        paco_bet = 1;
    }
    else
    {
        while (action != 0 && action != 1)
        {
            printf("Bid [0] , Dudo [1]\n");
            scanf("%d", &action);
        }
    }
    if (action == 0) // Bid
    {
        while (dice > 6 || dice < 1 || number < 1 || number < play.number || (dice <= play.dice && number <= play.number) || dice < play.dice)
        {
            printf("Bid: \n");
            scanf("%d %d", &dice, &number);
            if (play.palifico > 0) // Hi ha un o mes jugadors amb un dau
            {
                play.paco_bet = 0;
            }

            else if (dice == 1 && number >= round(play.number / 2) && play.paco_bet == 0 && paco_bet == 0) // Bet on pacos
            {
                printf("Paco bet");
                play.paco_bet = 1;
                break;
            }

            else if (dice != 1 && number >= (play.number * 2) + 1 && play.paco_bet == 1) // Returning normal bet
            {
                printf("Normal bet");
                play.paco_bet = 0;
                break;
            }
            else if (dice == 1 && paco_bet == 1) // Inici de ronda i bet pacos prohibit
            {
                dice = -1;
            }
        }
        play.dice = dice;
        play.number = number;
    }
    else if (action == 1) // Dudo
    {
        dudo(players);
        play.dice = 0;
        play.number = 0;
        shuffle_dices(players);
    }

    pthread_exit(0);
}

void *machine_func()
{
    int dice, number = 0;
    int action = rand() % 2;
    int bet_pacos = 0; // Comprovar que no s'aposti a pacos primera ronda

    if (play.dice == 0 || play.number == 0)
    { // comprovar que no comenci el torn
        action = 0;
        bet_pacos = 1;
    }

    if (action == 0) // Bid
    {
        // random dice and number
        while (dice > 6 || dice < 1 || number < 1 || number < play.number || (dice <= play.dice && number <= play.number) || dice < play.dice)
        {
            srand(time(0));
            dice = (rand() % NUM_DICES) + 1;
            number = (rand() % play.current_dices) + 1;

            if (play.number == play.current_dices) // Comprovem que no arribi a nombres molt elevats
            {
                dudo(players);
                play.dice = 0;
                play.number = 0;
                shuffle_dices(players);
            }
            if (play.palifico > 0) // Hi ha un o mes jugadors amb un dau
            {
                play.paco_bet = 0;
            }

            if (dice == 1 && number >= round(play.number / 2) && play.paco_bet == 0 && bet_pacos == 0) // Bet on pacos
            {
                printf("Paco bet");
                play.paco_bet = 1;
                break;
            }

            if (dice != 1 && number >= (play.number * 2) + 1 && play.paco_bet == 1) // Returning normal bet
            {
                printf("Normal bet");
                play.paco_bet = 0;
                break;
            }
            if (dice == 1 && bet_pacos == 1) // Inici de ronda i bet pacos prohibit
            {
                dice = -1;
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

void *pro_machine_func()
{
    double prob_dice, prob_number = 0;
    int count_dices, count_number = 0;

    if (play.dice == 0 || play.number == 0) // comprovar que no comenci el torn
    {                  
        play.dice = 2; // fem bid de dau 2 number 1 -> mínim
        play.number = 1;
    }

    else // Calculem probabilitat i decidim accio
    {
        /*

        1- Recorre els daus del jugador
        2- Contar quants daus com play.dice o 1 té el jugador
        3- Calcular prob restant daus jugador i el nombre real

        */

        if (play.paco_bet == 0) // Normal bet probabilitat
        {
            for (int i = 0; i < NUM_DICES; i++)
            {
                if (players[play.id_current].id != -1 && players[play.id_current].dice[i] != -1 && (players[play.id_current].dice[i] == play.dice || players[play.id_current].dice[i] == 1))
                {
                    count_number += 1;
                }
                count_dices += 1;
            }

            if (play.dice < 6)
            {
                prob_dice = calc_prob(play.current_dices, play.number);
            }
            prob_number = calc_prob(play.current_dices - count_dices, play.number - count_number + 1);
        }

        else // Retorn de paco bet a normal bet probabilitat
        {
            play.dice = 2;

            for(int j = 0; j < NUM_DICES ; j++)
            {
                if (players[play.id_current].id != -1 && players[play.id_current].dice[j] != -1 && (players[play.id_current].dice[j] == play.dice || players[play.id_current].dice[j] == 1))
                {
                    count_number += 1;
                }
                count_dices += 1;
            }
            prob_dice = calc_prob(play.current_dices,(play.number*2)+1);

            if(prob_dice > 0.50)
            {
                play.paco_bet = 0;
            }
        }

        if (prob_dice > 0.50 || prob_number > 0.50) // Bid
        {
            printf("Normal bet\n");
            if (prob_dice > prob_number)
            {
                play.dice += 1;
            }
            else
            {
                play.number += 1;
                if (play.number > play.current_dices)
                {
                    dudo(players);
                    play.dice = 0;
                    play.number = 0;
                    shuffle_dices(players);
                }
            }
        }
        else // Dudo
        {
            printf("Dudo bet\n");

            dudo(players);
            play.dice = 0;
            play.number = 0;
            shuffle_dices(players);
        }
    }
    prob_dice = 0;
    prob_number = 0;
    pthread_exit(0);
}

unsigned long int calc_binomial()
{
    int n, k;

    for (k = 0; k <= MAXN; k++)
        taula_binomial[0][k] = 0;
    for (n = 1; n <= MAXN; n++)
        taula_binomial[n][0] = 1;

    taula_binomial[1][1] = 1;
    for (n = 2; n <= MAXN; n++)
    {
        for (k = 1; k < n; k++)
            taula_binomial[n][k] = taula_binomial[n - 1][k - 1] + taula_binomial[n - 1][k];
        taula_binomial[n][n] = 1;
    }
}

double calc_prob(int dices, int number)
{

    // Variables probabilitat

    double p = 0.34;
    double q = 0.66;
    double prob = 0;

    // Calcul probabilitat

    for (int j = 0; j <= number; j++)
    {
        prob += taula_binomial[dices][j] * pow(p, j) * pow(q, dices - j);
    }
    return 1 - prob;
}