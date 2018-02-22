#ifndef FILE_ABSTRAITE_H
#define FILE_ABSTRAITE_H

#include <pthread.h>

struct ItemFile {
    int numero_serie;
    int valeur;
};

class FileAbstraite {
  public:
    virtual ~FileAbstraite() {}
    virtual void Insere(const ItemFile& item)=0;
    virtual bool Retire(ItemFile& item)=0;
    virtual void Termine()=0;
};
#endif
