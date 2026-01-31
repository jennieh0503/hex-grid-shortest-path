#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

//costanti
#define MAXS 100  //buffer
#define MAXROUTES 5
#define MAXCOST 100

//variabili globali
int dimr, dimc;
int dx[] = {-1, -1, 0, 0, 1, 1};
int evendy[] = {0, 1, -1, 1, 0, 1};
int odddy[] = {-1, 0, -1, 1, -1, 0};
int* cost;  //array dei costi di ciascun esagono, indicizzato con il proprio offset x*dimc+y
uint8_t* numairroute;  //array del numero di rotte aeree esistenti
int** airroute;  //array di array con le destinazioni delle rotte aeree di un esagono
int* Q;  //coda (minheap) per travel_cost, utilizzato anche per visita in ampiezza in change_cost
int* dist;  //array delle distanze utilizzato per travel_cost, venendo chiamato frequentemente non lo ri-alloco ad ogni chiamata (anche per change_cost)
int* dist_gen;  //generazione della distanza, per capire se sia rispetto alla sorgente giusta
int* next;  //al posto di lista dinamica che richiede malloc uso un array statico per tracciare il successivo di ogni elemento in lista
int* prev;
int bucket[MAXCOST+1];
int bucket_gen[MAXCOST+1];
int size;  //num elementi in coda
int gen;

//intestazione sottoprogrammi
void init();
void delete();
void change_cost();
void toggle_air_route();
int existsroute(int offset1, int offset2);
void travel_cost();
void enqueue(int offset);
void dequeue(int offset);

//main
int main(){
	char str[MAXS+1];

	dimr = -1;
	dimc = -1;
	cost = NULL;
	gen = 0;

	while(scanf("%s", str) != EOF){
		if(!strcmp(str, "init")) {

			if(cost){  //precedentemente inizializzato
				delete();
			}
			if(scanf("%d %d", &dimc, &dimr) == 2){
				init();
				printf("OK\n");
			}

		} else if(!strcmp(str, "change_cost")) {

				change_cost();

		} else if(!strcmp(str, "toggle_air_route")) {

			toggle_air_route();

		} else if(!strcmp(str, "travel_cost")) {

			travel_cost();

		}
	}

	delete();

	return 0;
}



//delete
void delete(){
	int i;

	free(cost);
	free(numairroute);
	for(i=0; i<dimr*dimc; i++){
		if(airroute[i]!=NULL)
			free(airroute[i]);
	}
	free(airroute);
	free(dist);
	free(Q);
	free(dist_gen);
	free(next);
	free(prev);
}



//init
void init(){
	int n, i;

	n = dimr*dimc;

	//alloco memoria
	cost = malloc(n*sizeof(int));
	numairroute = malloc(n*sizeof(uint8_t));
	airroute = malloc(n*sizeof(int*));
	
	//non da inizializzare perché inizializzati ad ogni chiamata
	Q = malloc(n*sizeof(int));
	dist = malloc(n*sizeof(int));
	dist_gen = malloc(n*sizeof(int));
	next = malloc(n*sizeof(int));
	prev = malloc(n*sizeof(int));

	for(i=0; i<n; i++){
		cost[i] = 1;
		numairroute[i] = 0;
		airroute[i] = NULL;
		dist_gen[i] = 0;
	}

	for(i=0; i<MAXCOST+1; i++){
		bucket[i] = -1;
		bucket_gen[i] = 0;
	}

}



//change_cost
void change_cost(){
	int x, y, v, raggio;
	int offset, i, head, diff, nb;
	int xn, yn;

	if(scanf("%d %d %d %d", &y, &x, &v, &raggio) == 4){  //fornito prima colonna e poi riga
		x = dimr -1 -x;  //gli esagoni si contano dal basso
		if(x<0 || x>=dimr || y<0 || y>=dimc || raggio == 0 || v<-10 || v>10)
			printf("KO\n");
		else {  //visita in ampiezza a partire da x, y
			offset = x*dimc+y;
			for(i=0; i<dimr*dimc; i++){
				dist[i] = -1;  //inf
			}
			dist[offset] = 0;
			size = 0;  //inizializzo coda vuota
			Q[size] = offset;  //inseriso l'elemento in coda
			size++;
			head = 0;  //puntatore alla testa della coda
			while(head!=size){  //finché la coda non è vuota (la testa coincide con la fine)
				offset = Q[head];  //estraggo la testa
				x = offset/dimc;
				y = offset%dimc;
				if(dist[offset]<raggio){
					diff = v*(raggio - dist[offset])/raggio;  //raggio-dist è sicuramente maggiore di 0, si effettua una divisione intera
					if(v<0 && v*(raggio-dist[offset])%raggio!=0)
						diff--;  //se v è negativo per definizione floor arrotonda all'intero minore più vicino, quindi dopo la divisione intera bisogna sottrarre 1
					if(diff!=0){  //se fosse 0 non cambierebbe il costo e non sarebbe più necessario iterare sui vicini
						cost[offset] += diff;
						if(cost[offset]<0)
							cost[offset] = 0;
						else if(cost[offset]>MAXCOST)
							cost[offset] = MAXCOST;
					}

					for(i=0; i<6; i++){  //aggiorno gli adiacenti
						xn = x+dx[i];
						if(x%2==dimr%2){
							yn = y+evendy[i];
						} else {
							yn = y+odddy[i];
						}
						nb = xn*dimc+yn;
						if(xn>=0 && xn<dimr && yn>=0 && yn<dimc && dist[nb]==-1){  //è un esagono valido non ancora in coda
							dist[nb] = dist[offset]+1;
							Q[size] = nb;
							size++;
						}
					}

				}
				head++;
			}
			printf("OK\n");
		}
	}
}



//toggle_air_route
void toggle_air_route(){
	int x1, y1, x2, y2;
	int i;

	if(scanf("%d %d %d %d", &y1, &x1, &y2, &x2) == 4){  //fornite prima le colonne e poi le righe
		x1 = dimr -1 -x1;
		x2 = dimr -1 -x2;  //le righe si contanto dal basso

		if(x1<0 || x1>=dimr || x2<0 || x2>=dimr || y1<0 || y1>=dimc || y2<0 || y2>=dimc)  //se gli esagoni non sono validi
			printf("KO\n");
		else {
			x1 = x1*dimc+y1;  //uso offset, riutilizzo la variabile per usare meno memoria
			x2 = x2*dimc+y2;
			i = existsroute(x1, x2);
			if(i != -1){  //se esiste già la rotta aerea da x1,y1 a x2,y2 la si rimuove
				airroute[x1][i] = airroute[x1][numairroute[x1]-1];  //scambio con l'ultima
				airroute[x1][numairroute[x1]-1] = -1;  //libero l'ultima
				numairroute[x1]--;  //aggiorno il numero di rotte presenti
				printf("OK\n");
			} else if(numairroute[x1]<5) {  //se non è già presente la si aggiunge se le rotte già esistenti non sono già 5
				if(airroute[x1]==NULL){  //non ancora allocato
					airroute[x1] = malloc(MAXROUTES*sizeof(int));
				}
				airroute[x1][numairroute[x1]] = x2;
				numairroute[x1]++;
				printf("OK\n");
			} else {
				printf("KO\n");
			}
		}
	}
}


//funzione ausiliaria existsroute che verifica se esiste già una rotta da un esagono all'altro, in caso affermativo restituisce l'indice
int existsroute(int offset1, int offset2){
	int i;

	if(airroute[offset1]==NULL)  //non ha rotte aeree
		return -1;

	for(i=0; i<numairroute[offset1]; i++){
		if(airroute[offset1][i] == offset2)
			return i;
	}

	return -1;  //se non è tra le rotte presetni restituisco -1
}



//travel_cost, utilizzo algoritmo di Dijkstra ma con bucket queue perché i costi si sono max 100
void travel_cost(){
	int xp, yp, xd, yd;
	int offset1, offset2, nb;
	int i, newdist, currdist;

	if(scanf("%d %d %d %d", &yp, &xp, &yd, &xd) == 4){  //fornite prima righe poi colonne
		xp = dimr -1 -xp;
		xd = dimr -1 -xd;  //esagoni contati dal basso

		if(xp<0 || xp>=dimr || yp<0 || yp>=dimc || xd<0 || xd>=dimr || yd<0 || yd>=dimc)  //esagoni non validi
			printf("-1\n");
		else {
			offset1 = xp*dimc+yp;
			offset2 = xd*dimc+yd;  //utilizzo gli offset

			gen++;

			dist[offset1] = 0;
			currdist = 0;
			dist_gen[offset1] = gen;
			size = 0;
			enqueue(offset1);

			while(size>0){
				while(bucket_gen[currdist%(MAXCOST+1)]!=gen || bucket[currdist%(MAXCOST+1)]==-1){  //finché le celle sono vuote o vecchie
					currdist++;  //scorro in avanti fino a trovare la prima distanza di questa gen
				}

				offset1 = bucket[currdist%(MAXCOST+1)];  //estraggo il min
			
				dequeue(offset1);

				if(offset1==offset2){
					printf("%d\n", dist[offset2]);  //se estraggo la destinazione stampo la distanza calcolata ed esco
					return;
				} else if(cost[offset1]!=0) {  //se fosse 0 non sarebbe attraversabile quindi non cambierebbero le distanze degli adiacenti/rotte aeree
					newdist = dist[offset1] + cost[offset1];

					xp = offset1/dimc;
					yp = offset1%dimc;

					if(xp%2==dimr%2){
						for(i=0; i<6; i++){
							xd = xp+dx[i];
							yd = yp+evendy[i];

							if(xd>=0 && xd<dimr && yd>=0 && yd<dimc){  //se è un esagono valido
								nb = xd*dimc+yd;
								if(dist_gen[nb] != gen){  //la distanza calcolata non è di questa chiamata, quindi significa che non l'ho ancora mai aggiunto in coda
									dist[nb] = newdist;
									dist_gen[nb] = gen;
									enqueue(nb);
								} else if (dist[nb]>newdist) {  //se è già stato aggiunto in coda ma devo modificare la distanza perché ne ho trovata una minore
									if(prev[nb]==-1){  //è il primo in lista
										bucket[dist[nb]%(MAXCOST+1)] = next[nb];  //modifico la testa togliendolo dalla lista
									} else {  //non è il primo in lista ma ha un precedente
										next[prev[nb]] = next[nb];  //lo elimino dalla lista modificando il precedente
										prev[nb] = -1;
									}
									if(next[nb]!=-1){  //se ha un successivo modifico il riferimento di questo
										prev[next[nb]] = nb;
										next[nb] = -1;
									}
									size--;
									dist[nb] = newdist;  //aggiorno la distanza
									enqueue(nb);  //lo ri-aggiungo in lista nel bucket corretto
								}
							}
						}
					} else {
						for(i=0; i<6; i++){
							xd = xp+dx[i];
							yd = yp+odddy[i];

							if(xd>=0 && xd<dimr && yd>=0 && yd<dimc){  //se è un esagono valido
								nb = xd*dimc+yd;
								if(dist_gen[nb] != gen){  //la distanza calcolata non è di questa chiamata, quindi significa che non l'ho ancora mai aggiunto in coda
									dist[nb] = newdist;
									dist_gen[nb] = gen;
									enqueue(nb);
								} else if (dist[nb]>newdist) {  //se è già stato aggiunto in coda ma devo modificare la distanza perché ne ho trovata una minore
									if(prev[nb]==-1){  //è il primo in lista
										bucket[dist[nb]%(MAXCOST+1)] = next[nb];  //modifico la testa togliendolo dalla lista
									} else {  //non è il primo in lista ma ha un precedente
										next[prev[nb]] = next[nb];  //lo elimino dalla lista modificando il precedente
										prev[nb] = -1;
									}
									if(next[nb]!=-1){  //se ha un successivo modifico il riferimento di questo
										prev[next[nb]] = nb;
										next[nb] = -1;
									}
									size--;
									dist[nb] = newdist;  //aggiorno la distanza
									enqueue(nb);  //lo ri-aggiungo in lista nel bucket corretto
								}
							}
						}
					}

					
					if(airroute[offset1]!=NULL){
						for(i=0; i<numairroute[offset1]; i++){  //aggiorno le rotte aeree, newdist è uguale perché il costo aereo è sempre uguale a quello di terra
							yd = airroute[offset1][i];
							if(dist_gen[yd] != gen){  //la validità dell'esagono è assicurata da toggle_air_route
								dist[yd] = newdist;
								dist_gen[yd] = gen;
								enqueue(yd);
							} else if(dist[yd]>newdist) {
								if(prev[yd]==-1){  //è il primo in lista
									bucket[dist[yd]%(MAXCOST+1)] = next[yd];  //modifico la testa togliendolo dalla lista
								} else {  //non è il primo in lista ma ha un precedente
									next[prev[yd]] = next[yd];  //lo elimino dalla lista modificando il precedente
									prev[yd] = -1;
								}
								if(next[yd]!=-1){  //se ha un successivo modifico il riferimento di questo
									prev[next[yd]] = yd;
									next[yd] = -1;
								}
								size--;
								dist[yd] = newdist;  //aggiorno la distanza
								enqueue(yd);  //lo ri-aggiungo in lista nel bucket corretto
							}
						}
					}
				}
			}

			printf("-1\n");  //se esco senza trovare la distanza allora è irraggiungibile
			
		}
	}
}

void enqueue(int offset){
	int i;

	i = dist[offset]%(MAXCOST+1);

	if(bucket_gen[i]!=gen){  //se contiene elementi dalla chiamata precedente elimino
		bucket[i] = -1;  //reset
		bucket_gen[i] = gen;
	}

	next[offset] = bucket[i];
	prev[offset] = -1;
	if(bucket[i]!=-1)
		prev[bucket[i]] = offset;
	bucket[i] = offset;

	size++;
}

void dequeue(int offset){
	int i;

	i = dist[offset]%(MAXCOST+1);

	bucket[i] = next[offset];
	if(bucket[i]!=-1)
		prev[bucket[i]] = -1;
	next[offset] = -1;
	prev[offset] = -1;
	size--;
}