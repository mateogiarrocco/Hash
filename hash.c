#include <stdbool.h>
#include "lista.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>

const size_t TAM = 47;
const size_t ELEM_MAX = 2;
const size_t FACTOR_AGRANDAR = 2;
const size_t ELEM_MIN = 4;
const size_t FACTOR_ACHICAR = 2;

size_t hashing(const char* str, size_t length){ //djb, 47 elem: 4"17 seg, 13 elem: 4"26 seg, 97 elem: 4"24 seg
	size_t hash = 5381;
	size_t i = 0;

	for(i = 0; i < length; str ++, i ++){
		hash = ((hash << 5) + hash) + (size_t)(*str);
	}
	return hash;
}

struct hash {
	lista_t** listas;
	size_t capacidad;
	size_t cantidad;
	void (*hash_destruir_dato_t) (void *);
} ;

typedef struct hash_campo {
	char* clave;
	void* valor;
} hash_campo_t;

bool claves_iguales(char* clave1,  const char* clave2) {
	return !strcmp(clave1, clave2);
}

char* crear_clave(const char* clave){
	size_t largo = strlen(clave);
	char* clave_aux = malloc((largo + 1) * sizeof(char));
	if(!clave_aux) return NULL;
	strcpy(clave_aux, clave);
	return clave_aux;
}

lista_iter_t* hash_buscar_elemento(const hash_t* hash, const char* clave, size_t pos) {
	if(hash->cantidad == 0) return NULL;
	lista_iter_t* iter = lista_iter_crear(hash->listas[pos]);
	if(!iter) return NULL;
	hash_campo_t* aux = (hash_campo_t*) lista_iter_ver_actual(iter);
	if (!aux) {
		free(iter);
		return NULL;
	}
	while(!claves_iguales(aux->clave, clave)) {
		lista_iter_avanzar(iter);
		if(lista_iter_al_final(iter)) {
			lista_iter_destruir(iter);
			return NULL;
		}
		aux = (hash_campo_t*) lista_iter_ver_actual(iter);
	}
	return iter;
}

hash_t* hash_crear(hash_destruir_dato_t hash_destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if(!hash) return NULL;
	hash->listas = calloc(TAM, sizeof(lista_t*));
	if(!hash->listas) {
		free(hash);
		return NULL;
	}
	hash->cantidad = 0;
	hash->capacidad = TAM;
	hash->hash_destruir_dato_t = hash_destruir_dato;
	return hash;
}
	
size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}
		 
hash_campo_t* campo_crear(char* clave, void* valor) {
	hash_campo_t* hash_campo = malloc(sizeof(hash_campo_t));
	if(!hash_campo) return NULL;
	hash_campo->clave = clave;
	hash_campo->valor = valor;
	return hash_campo;
}

bool hash_redimensionar(hash_t* hash, size_t capacidad_nueva){
	lista_t** listas_viejas = hash->listas;
	size_t capacidad_vieja = hash->capacidad;
	hash->capacidad = capacidad_nueva;
	hash->cantidad = 0;
	hash->listas = calloc(hash->capacidad, sizeof(lista_t*));
	if(!hash->listas) return false;
	for(int i = 0; i < capacidad_vieja; i++) {
		if(listas_viejas[i]) {
			lista_iter_t* iter_lista = lista_iter_crear(listas_viejas[i]);
			if(!iter_lista) return false;
			while(!lista_iter_al_final(iter_lista)) {
				hash_campo_t* aux = (hash_campo_t*) lista_iter_ver_actual(iter_lista);
				hash_guardar(hash, aux->clave, aux->valor);
				free(aux->clave);
				lista_iter_avanzar(iter_lista);
			}
			lista_iter_destruir(iter_lista);
			lista_destruir(listas_viejas[i], free);
		}
	}
	free(listas_viejas);
	return true;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {
	size_t pos_lista = hashing(clave, strlen(clave)) % hash->capacidad;
	if(!hash->listas[pos_lista]) hash->listas[pos_lista] = lista_crear();
	lista_iter_t* iter = hash_buscar_elemento(hash, clave, pos_lista);
	if(!iter) {
		char* clave_aux = crear_clave(clave);
		if(!clave_aux) return false;
		hash_campo_t* campo = campo_crear(clave_aux, dato);
		if(!campo){
			free((void*) clave_aux);
			return false;
		}
		lista_insertar_ultimo(hash->listas[pos_lista], campo);
		hash->cantidad ++;
		if(hash->cantidad >= hash->capacidad * ELEM_MAX) {
			if(!hash_redimensionar(hash, hash->capacidad * FACTOR_AGRANDAR)){
				return false;
			}
		}
		return true;
	}
	hash_campo_t* aux = (hash_campo_t*) lista_iter_ver_actual(iter);
	void* destruir = aux->valor;
	aux->valor = dato;
	if(hash->hash_destruir_dato_t) hash->hash_destruir_dato_t(destruir);
	lista_iter_destruir(iter);
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {
	if(!hash_pertenece(hash, clave)) return NULL;
	size_t pos_lista = hashing(clave, strlen(clave)) % hash->capacidad;    
	lista_iter_t* iter = hash_buscar_elemento(hash, clave, pos_lista);
	hash_campo_t* aux = (hash_campo_t*) lista_iter_borrar(iter);
	lista_iter_destruir(iter);
	hash->cantidad--;
	if(hash->cantidad * ELEM_MIN <= hash->capacidad){
		if(!hash_redimensionar(hash, hash->capacidad / FACTOR_ACHICAR)){
			return false;
		}
	}
	free(aux->clave);
	void* dato_aux = aux->valor;
	free(aux);
	return dato_aux;
}
	
void *hash_obtener(const hash_t *hash, const char *clave) {
	if(!hash_pertenece(hash, clave)) return NULL;
	size_t pos_lista = hashing(clave, strlen(clave)) % hash->capacidad;
	lista_iter_t* iter = hash_buscar_elemento(hash, clave, pos_lista);
	hash_campo_t* aux = (hash_campo_t*) lista_iter_ver_actual(iter);
	lista_iter_destruir(iter);
	return aux->valor;
}	
	
bool hash_pertenece(const hash_t *hash, const char *clave) {
	if(hash_cantidad(hash) == 0) return false;
	size_t pos_lista = hashing(clave, strlen(clave)) % hash->capacidad;
	if(!hash->listas[pos_lista]) return false;
	lista_iter_t* iter = hash_buscar_elemento(hash, clave, pos_lista);
	if(iter) {
		lista_iter_destruir(iter);
		return true;
	}
	return false;
}

void hash_destruir(hash_t *hash){
	for(int i = 0;i < hash->capacidad; i++){ 
		if(hash->listas[i]) {
			while(!lista_esta_vacia(hash->listas[i])) {
				hash_campo_t* campo = lista_borrar_primero(hash->listas[i]);
				free(campo->clave);
				if(hash->hash_destruir_dato_t) hash->hash_destruir_dato_t(campo->valor);
				hash->cantidad --;
				free(campo);
			}
			lista_destruir(hash->listas[i], NULL);
		}
	}
	free(hash->listas);
	free(hash);
}

//...........Primitivas del iterador.............

struct hash_iter{
	const hash_t* hash;
	size_t pos_lista;
	lista_iter_t* iter_lista;
} ;

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(!hash_iter)	return NULL;
	hash_iter->hash = hash;
	hash_iter->pos_lista = 0;
	if(hash_cantidad(hash_iter->hash) == 0) {
		hash_iter->iter_lista = NULL;
		return hash_iter; 
	}
	while(!hash_iter->hash->listas[hash_iter->pos_lista]) hash_iter->pos_lista++;
	hash_iter->iter_lista = lista_iter_crear(hash_iter->hash->listas[hash_iter->pos_lista]);
	if(!hash_iter->iter_lista) {
		free(hash_iter);
		return NULL;
	}
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)) return false;
	lista_iter_avanzar(iter->iter_lista);
	if(lista_iter_al_final(iter->iter_lista)){
		lista_iter_destruir(iter->iter_lista);
		iter->pos_lista ++;
		while(iter->pos_lista < iter->hash->capacidad && !iter->hash->listas[iter->pos_lista]) {
			iter->pos_lista ++;
		}
		if (iter->pos_lista >= iter->hash->capacidad) {
			iter->iter_lista = NULL;
			return true;
		}
		lista_iter_t* iter_lista = lista_iter_crear(iter->hash->listas[iter->pos_lista]);
		if(!iter_lista) return false;
		iter->iter_lista = iter_lista;
		return true;
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)) return NULL;
	hash_campo_t* aux = (hash_campo_t*) lista_iter_ver_actual(iter->iter_lista);
	return aux->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	return !iter->iter_lista;
}

void hash_iter_destruir(hash_iter_t* iter){
	if(iter->iter_lista) lista_iter_destruir(iter->iter_lista);
	free(iter);
}
