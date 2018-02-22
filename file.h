
#include <deque>
#include <pthread.h>
#include <iostream>

#include "file_abstraite.h"

class File : public FileAbstraite {
    static const int TAILLE_STOCKAGE=10;

  public:
    File();
    void Insere(const ItemFile& item);
    bool Retire(ItemFile& item);
    void Termine();
  private:
    std::deque<ItemFile> stockage;
    pthread_mutex_t mutex_stockage;
    pthread_cond_t condition_producteurs, condition_consommateurs;
    bool execution_terminee;
};
