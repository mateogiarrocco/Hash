#include <stdbool.h>
#include <stdlib.h>
#include "lista.h"

//...................Declaracion de estructuras....................

typedef struct nodo {
    void* dato;
    struct nodo* prox;
} nodo_t;

struct lista {
    struct nodo* primero;
    struct nodo* ultimo;
    size_t largo;
};

struct lista_iter {
	lista_t* lista;
	struct nodo* act;
	struct nodo* ant;
};

//......................Primitivas de la lista.................

nodo_t* nodo_crear(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t)); 
	if (!nodo) return NULL; 
	nodo->dato = valor;
	nodo->prox = NULL; 
	return nodo; 
}
lista_t* lista_crear(){
	lista_t* lista = malloc(sizeof(lista_t));
	if(!lista)	return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;
	return lista;
}
size_t lista_largo(const lista_t *lista){
	return lista->largo;
}
bool lista_esta_vacia(const lista_t *lista){
	return lista_largo(lista) == 0;
}
bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if(!nodo)	return false;	
	if(lista_esta_vacia(lista))	lista->ultimo = nodo;
	nodo->prox = lista->primero;
	lista->primero = nodo;
	lista->largo ++;
	return true;
}
bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if(!nodo) return false;
	if(lista_esta_vacia(lista))	lista->primero = nodo;
	else lista->ultimo->prox = nodo;
	lista->ultimo = nodo;	
	lista->largo ++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if(lista_esta_vacia(lista)) return NULL;
	nodo_t* aux = lista->primero;
	void* valor = lista->primero->dato;
	if(lista_largo(lista) == 1){
		lista->primero = NULL;
		lista->ultimo = NULL;
	}
	else lista->primero = lista->primero->prox;
	free(aux);
	lista->largo --;
	return valor;
}
void *lista_ver_primero(const lista_t *lista){
	if(lista_esta_vacia(lista))	return NULL;
	return lista->primero->dato;              
}
void *lista_ver_ultimo(const lista_t* lista){
	if(lista_esta_vacia(lista))	return NULL;
	return lista->ultimo->dato;
}
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	void* dato_aux;

	while(!lista_esta_vacia(lista)){
		dato_aux = lista_borrar_primero(lista);
		if(destruir_dato)	destruir_dato(dato_aux);
	}
	free(lista);
}

//..................Primitivas de los iteradores................

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* lista_iter = malloc(sizeof(lista_iter_t));
	if(!lista_iter)	return NULL;
	lista_iter->lista = lista;
	lista_iter->ant = NULL;
	lista_iter->act = lista->primero;
	return lista_iter;
}
bool lista_iter_al_final(const lista_iter_t *iter){
	return(!iter->act);
}
bool lista_iter_avanzar(lista_iter_t *iter){
	if(lista_iter_al_final(iter))	return false;
	iter->ant = iter->act;
	iter->act = iter->act->prox;
	return true;
}
void *lista_iter_ver_actual(const lista_iter_t *iter){
	if(lista_iter_al_final(iter))	return NULL;
	return iter->act->dato;
}
void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}
bool lista_iter_insertar(lista_iter_t *iter, void *valor){
	nodo_t* nodo = nodo_crear(valor);
	if(!nodo)	return false;

	nodo->prox = iter->act;
	if((lista_esta_vacia(iter->lista)) || (!iter->ant))	iter->lista->primero = nodo;
	else iter->ant->prox = nodo;
	if(!iter->act) iter->lista->ultimo = nodo;
	iter->act = nodo;
	iter->lista->largo ++;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
	if(!iter->act)	return NULL;
	nodo_t* aux = iter->act;
	void* valor = iter->act->dato;
	bool borrado = false;
	if(iter->act == iter->lista->primero){
		if(lista_largo(iter->lista) == 1) iter->act = NULL;
		else iter->act = iter->act->prox;
		return lista_borrar_primero(iter->lista);
		borrado = true;
	}
	else if((iter->act == iter->lista->ultimo) && (!borrado)){
		iter->act = NULL;
		iter->lista->ultimo = iter->ant;
		iter->ant->prox = NULL;
	}
	else{
	 	iter->act = iter->act->prox;
	 	iter->ant->prox = iter->act;
	}
	if(!borrado){
		iter->lista->largo --;
		free(aux);
	}
	return valor;
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	if(!lista->primero) return;
	nodo_t* actual = lista->primero;
	while(actual && visitar(actual->dato,extra)){
		actual = actual->prox;
	}
}