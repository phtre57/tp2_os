
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <random>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "file.h"

static const int N_PRODUCTEURS=3;
static const int N_CONSOMMATEURS=3;
static const int N_ITEMS=50;

File ma_file;
int prochain_numero_serie = 0;
pthread_mutex_t mutex_numero_serie;

void *Producteur(void *param_id_thread) {
    long id_thread = (long) param_id_thread;

    auto graine_aleatoire =  static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count());

    std::default_random_engine generateur(id_thread + graine_aleatoire);
    std::uniform_int_distribution<int> distribution(0,1000);

    ItemFile item;

    std::cout << "Producteur " << id_thread << " debute." << std::endl;

    while(prochain_numero_serie < N_ITEMS) {
        pthread_mutex_lock(&mutex_numero_serie);
        if (prochain_numero_serie < N_ITEMS) {
            item.numero_serie = prochain_numero_serie++;

            // Ajouter l'item a la file.
            item.valeur = distribution(generateur);
            ma_file.Insere(item);
        }
        pthread_mutex_unlock(&mutex_numero_serie);

        // Cette passe-passe avec le stringstream permet de limiter l'entrchassement de caracteres
        // dans une situation multi-thread. Il n'y a pas de garantie qu'elle l'elimine completement.
        std::stringstream ss_producteur;
        ss_producteur << "Prod" << id_thread << ": +" << item.numero_serie << '\n';
        std::cout << ss_producteur.str() << std::flush;

        // Faire dormir le producteur pendant un certain temps.
        float phase = std::fabs(std::sin(0.03 * (float)item.numero_serie));
        int temps_sommeil = 5.0 * phase * (float)item.valeur;
        std::this_thread::sleep_for(std::chrono::milliseconds(temps_sommeil));
    }

    std::cout << "Producteur " << id_thread << " se termine." << std::endl;
}


void *Consommateur(void *param_id_thread) {
    long id_thread = (long) param_id_thread;
    ItemFile item;
    bool element_retire_avec_succes = true;

    std::cout << "Consommateur " << id_thread << " debute." << std::endl;

    while(element_retire_avec_succes) {
        element_retire_avec_succes = ma_file.Retire(item);

        if(element_retire_avec_succes) {
            // Cette passe-passe avec le stringstream permet de limiter l'entrchassement de caracteres
            // dans une situation multi-thread. Il n'y a pas de garantie qu'elle l'elimine completement.
            std::stringstream ss_consommateur;
            ss_consommateur << "Cons" << id_thread << ":      -" << item.numero_serie << '\n';
            std::cout << ss_consommateur.str() << std::flush;

            // Faire dormir le producteur pendant un certain temps.
            float phase = std::fabs(std::cos(0.03 * (float)item.numero_serie));
            int temps_sommeil = 5.0 * phase * (float)item.valeur;

            std::this_thread::sleep_for(std::chrono::milliseconds(temps_sommeil));
        }
    }

    std::cout << "Consommateur " << id_thread << ": se termine." << std::endl;
}


int main() {
    std::array<pthread_t, N_PRODUCTEURS> producteurs;
    std::array<pthread_t, N_CONSOMMATEURS> consommateurs;

    // Creer les threads producteurs.
    for(long i = 0; i < N_PRODUCTEURS; ++i) {
        int retour = pthread_create(&producteurs[i], NULL, Producteur, (void *)i);

        if(retour != 0) {
            throw std::runtime_error("Erreur lors de la creation d'um thread.");
        }
    }

    // Creer les threads consommateurs.
    for(long i = 0; i < N_CONSOMMATEURS; ++i) {
        int retour = pthread_create(&consommateurs[i], NULL, Consommateur, (void *)i);

        if(retour != 0) {
            throw std::runtime_error("Erreur lors de la creation d'um thread.");
        }
    }

    for(auto producteur : producteurs) {
        pthread_join(producteur, NULL);
    }

    std::cout << "Appel à Termine()" << std::endl;
    ma_file.Termine();

    for(auto consommateur : consommateurs) {
        pthread_join(consommateur, NULL);
    }

    return 0;
}
