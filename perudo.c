#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define MAXN 40 // Número màxim de files a la distribució binomial
#define MAX_PLAYERS 6
#define MIN_PLAYERS 2
#define NUM_DICES 5

struct Player // Estructura general dels jugadors
{
    int id;
    pthread_t id_thread;
    char dice[5];
};

struct Play // Estructura de la partida
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

unsigned long int taula_binomial[MAXN + 1][MAXN + 1]; // Taula binomial
unsigned long int calc_binomial();                    // Funció per omplir la taula binomial

int main() // Funció principal de setup
{
    srand(time(0));
    init_game();
    calc_binomial();
    pthread_t dealer;

    pthread_create(&dealer, NULL, dealer_func, NULL);
    pthread_join(dealer, NULL);
    return 0;
}

void init_game() // Inicialitza una partida nova
{

    srand(time(0));
    int num_players = 0;
    int level = -1;
    printf("\n-------------------------\n");
    printf("WELCOME TO PERUDO!\nThis game has been developed by Jaume Serra\n");
    while (num_players > MAX_PLAYERS || num_players < MIN_PLAYERS)
    {
        printf("\nHow many players? [2-6]\n");
        scanf("%d", &num_players);
    }

    while (level != 0 && level != 1)
    {
        printf("Dumm Players [0] or Smart Players [1] ?\n");
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

void *dealer_func() // Printa el taulell i organitza la partida i els threads
{
    int game_on = 0;
    int action;
    char taulell[6] = "EXYZQW"; // Lletres pel taulell
    while (game_on != 4)
    {
        for (int i = 0; i < MAX_PLAYERS; i++) // Comporvem que no s'hagi acabat la partida
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
                init_game();
            }

            if (players[i].id == 0)
            {
                // Jugador
                // Printem el taulell
                printf("\n-------------------------\n");

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

                // actualitzem valors ultim jugador i actual
                printf("-------------------------\n");
                printf("Turn Player: %d \n", players[i].id);
                if (play.dice != 0 && play.number != 0)
                {
                    printf("Last bet  Dice:%d  Number:%d\n", play.dice, play.number);
                }
                else
                {
                    printf("New Round\n");
                }
                printf("-------------------------\n");

                game_on = 0;
                while (game_on != 1 && game_on != 2 && game_on != 3 && game_on != 4)
                {
                    printf("Continue game [1], Start New Game [2], Add/Del Players [3], Exit[4]\n");
                    scanf("%d", &game_on);
                }
                if (game_on == 2)
                {
                    init_game();
                    printf("New Round\n");
                    printf("-------------------------\n");
                }

                if (game_on == 3)
                {
                    int action_ad;
                    int new_players;
                    printf("Add [0] or Delete [1]\n");
                    scanf("%d", &action_ad);

                    if (action_ad == 0) // Afegir jugadors
                    {

                        printf("Hom many players? \n");
                        scanf("%d", &new_players);

                        if (play.current_players + new_players > MAX_PLAYERS) // Maxim nombre de jugadors
                        {
                            printf("ERROR! Max Number of Players\n");
                            while (game_on != 1 && game_on != 2 && game_on != 4)
                            {
                                printf("Continue game [1], Start New Game [2], Exit[4]\n");
                                scanf("%d", &game_on);
                            }
                        }
                        else // Afegim jugadors
                        {
                            play.dice = 0;
                            play.number = 0;
                            play.paco_bet = 0;
                            for (int i = 0; i < MAX_PLAYERS; i++)
                            {
                                if (new_players == 0)
                                {
                                    break;
                                }

                                if (players[i].id == -1)
                                {
                                    // assignem id i daus
                                    players[i].id = i;
                                    printf("Jugador amb ID: %d Afegit\n", players[i].id);

                                    for (int j = 0; j < NUM_DICES; j++)
                                    {
                                        players[i].dice[j] = (rand() % 6) + 1;
                                    }
                                    play.current_players += 1;
                                    new_players -= 1;
                                }
                            }
                        }
                    }
                    else // Eliminar jugadors
                    {
                        printf("Hom many players? \n");
                        scanf("%d", &new_players);
                        if (play.current_players - new_players < MIN_PLAYERS)
                        {
                            printf("ERROR! Min Number of Players\n");
                            while (game_on != 1 && game_on != 2 && game_on != 4)
                            {
                                printf("Continue game [1], Start New Game [2], Exit[4]\n");
                                scanf("%d", &game_on);
                            }
                        }
                        else // Eliminem jugadors
                        {
                            play.dice = 0;
                            play.number = 0;
                            play.paco_bet = 0;
                            for (int i = 0; i < MAX_PLAYERS; i++)
                            {
                                if (new_players == 0)
                                {
                                    printf("New Round\n");
                                    break;
                                }
                                if (players[i].id != -1 && players[i].id != 0)
                                {
                                    printf("Jugador amb ID: %d Eliminat\n", players[i].id);
                                    players[i].id = -1;
                                    new_players -= 1;
                                    play.current_players -= 1;
                                }
                            }
                        }
                    }
                }

                if (game_on == 4) // Sortim de la partida
                    break;

                play.id_last = play.id_current;
                play.id_current = players[i].id;

                pthread_create(&players[i].id_thread, NULL, user_func, NULL);
                pthread_join(players[i].id_thread, NULL);

                printf("-------------------------\n");
                sleep(1); // Millor jugabilitat
            }
            else if (players[i].id != -1)
            {
                // Maquina
                // actualitzem valors ultim jugador i actual
                printf("-------------------------\n");

                play.id_last = play.id_current;
                play.id_current = players[i].id;

                printf("Turn Player: %d \n", play.id_current);
                if (play.dice != 0 && play.number != 0)
                {
                    printf("Last bet  Dice:%d  Number:%d\n", play.dice, play.number);
                }
                else
                {
                    printf("New Round\n");
                }
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
                if (play.dice != 0 && play.number != 0)
                {
                    printf("Player %d bet  Dice:%d  Number:%d\n", play.id_current, play.dice, play.number);
                }

                printf("-------------------------\n");
                sleep(1);
            }
        }
    }
    pthread_exit(0);
}
void shuffle_dices() // Tornem a assignar daus a tots els jugadors actuals
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

void dudo() // Funció dudo que calcula el nombre de daus actuals i elimina un dau al jugador que perd
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
                printf("Jugador %d Perd dau \n", players[play.id_current].id);
                play.current_dices -= 1;
                if (i == (NUM_DICES - 2))
                {
                    printf("Palifico\n");
                    play.palifico += 1;
                }
                if (i == (NUM_DICES - 1))
                {
                    printf("Jugador %d Eliminat \n", players[play.id_current].id);
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
                printf("Jugador %d Perd dau \n", players[play.id_last].id);
                play.current_dices -= 1;
                if (i == (NUM_DICES - 2))
                {
                    printf("Palifico\n");
                    play.palifico += 1;
                }
                if (i == (NUM_DICES - 1))
                {
                    printf("Jugador %d Eliminat \n", players[play.id_last].id);

                    players[play.id_last].id = -1;
                    play.current_players -= 1;
                    play.palifico -= 1;
                }
                break;
            }
        }
    }
}
void *user_func() // Thread de l'usuari function
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
                printf("Paco bet\n");
                play.paco_bet = 1;
                break;
            }

            else if (dice != 1 && number >= (play.number * 2) + 1 && play.paco_bet == 1) // Returning normal bet
            {
                printf("Normal bet\n");
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

void *machine_func() // Thread dumm machine function
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
                printf("Paco bet\n");
                play.paco_bet = 1;
                break;
            }

            if (dice != 1 && number >= (play.number * 2) + 1 && play.paco_bet == 1) // Returning normal bet
            {
                printf("Normal bet\n");
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

void *pro_machine_func() // Thread machine smart function. Utilitza distribucio binomial per calcular la probabilitat i després actua en conseqüència
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

            for (int j = 0; j < NUM_DICES; j++)
            {
                if (players[play.id_current].id != -1 && players[play.id_current].dice[j] != -1 && (players[play.id_current].dice[j] == play.dice || players[play.id_current].dice[j] == 1))
                {
                    count_number += 1;
                }
                count_dices += 1;
            }
            prob_dice = calc_prob(play.current_dices, (play.number * 2) + 1);

            if (prob_dice > 0.50)
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

unsigned long int calc_binomial() // Funció per omplir la taula binomial
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

double calc_prob(int dices, int number) // Calcula la probabilitat de daus i number utilitzant la distribucio binomial
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