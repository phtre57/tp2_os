
#include "file.h"

File::File() {
	mutex_stockage = PTHREAD_MUTEX_INITIALIZER;
	condition_producteurs = PTHREAD_COND_INITIALIZER;
	condition_consommateurs = PTHREAD_COND_INITIALIZER;
	execution_terminee = false; 
}


void File::Insere(const ItemFile& item) {
	pthread_mutex_lock(&mutex_stockage);
	while (stockage.size() == 10){
		//std::cout << "in while insere \n";
		pthread_cond_wait(&condition_producteurs, &mutex_stockage);
	}
	stockage.push_back(item);
	//std::cout << "Item insert serial_no: " << item.numero_serie << "\n";
	pthread_cond_signal(&condition_consommateurs);
	pthread_mutex_unlock(&mutex_stockage);

}


bool File::Retire(ItemFile& item) {
	//std::cout << "Bool execution_terminee: " << !execution_terminee << "\n";
	pthread_mutex_lock(&mutex_stockage);
	if (!execution_terminee){
		while (stockage.size() == 0){
			//std::cout << "in while retire \n";
			//std::cout << "Bool execution_terminee: " << !execution_terminee << "\n";
			if (execution_terminee){
				//std::cout << "in while retire false \n";
				//std::cout << "deque size: " << stockage.size() << "\n";
				pthread_mutex_unlock(&mutex_stockage);
				return false;
			}
			else{
				//std::cout << "in while retire true \n";
				pthread_cond_wait(&condition_consommateurs, &mutex_stockage);
			}
		}
		item = stockage.front();
		//std::cout << "Item remove serial_no: " << item.numero_serie << "\n";
		stockage.pop_front();
		pthread_cond_signal(&condition_producteurs);
		pthread_mutex_unlock(&mutex_stockage);
		return true;
	}

	if (stockage.size() > 0){
		item = stockage.front();
		stockage.pop_front();
		//std::cout << "here " << stockage.size() << "\n";
		pthread_mutex_unlock(&mutex_stockage);
		return true;
	}

	pthread_mutex_unlock(&mutex_stockage);
	return false;
    
}


void File::Termine() {
	execution_terminee = true;
	pthread_cond_broadcast(&condition_consommateurs);
}
