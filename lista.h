#ifndef LISTA_H
#define LISTA_H
#include <stdbool.h>
#include <stdio.h>

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

// Crea una lista
// Post: devuelve una lista vacia
lista_t* lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos enlistados. Si tiene, devuelve false
// Pre: la lista fue creada
bool lista_esta_vacia(const lista_t *lista);

// Inserta un elemento al principio de la lista. Devuelve falso en caso de error
// Pre: la lista fue creada
// Post: se agregó un elemento a la lista, el elemento insertado es el primero de la lista
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un elemento al final de la lista. Devuelve falso en caso de error
// Pre: la lista fue creada
// Post: se agregó un elemento a la lista, el elemento insertado es el ultimo de la lista
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se saca el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// tiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si tiene elementos,
// devuelve el valor del primero, si esta vacía devuelve NULL.
// Pre: la lista fue creada
// Post: se devolvió el primer elemento de la lista, en caso que no esté vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si tiene elementos,
// devuelve el valor del ultimo, si esta vacía devuelve NULL.
// Pre: la lista fue creada
// Post: se devolvió el ultimo elemento de la lista, en caso que no esté vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista
// Pre: la lista fue creada
// Post: devuelve la cantidad de elementos que hay en la lista
size_t lista_largo(const lista_t *lista);

// Destruye la lista
// Si se recibe la función destruir_dato por parámetro,
// llama a destruir_dato para cada uno de los elementos de la lista
// Pre: la lista fue creada, destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL si no se la utiliza.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));


// Crea un iterador para la lista
// Post: crea un iterador que la lista que recibe, y devuelve un iterador externo 
// situado en la primera posicion de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza el iterador en una posicion
// Post: Sitúa al iterador en la siguiente posicion. Devuelve false si hay un error,
// si no, devuelve true.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el dato del nodo sobre el cual esta parado. 
// Si esta vacia, devuelve NULL.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Verifica si el iterador esta al final de la lista, es decir si no apunta a ningun nodo 
// en ese caso devuelve true, sino, devuelve false.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador externo
// Pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

// Post: inserta un elemento a la lista. 
// Si no pudo insertar, devuelve falso, si pudo, true.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Post: devuelve el dato borrado.
// Borra un elemento. Devuelve false si no pudo borrarlo, true si pudo.
void *lista_iter_borrar(lista_iter_t *iter);

// Itera internamente sobre la lista usando la funcion visitar enviada por parametro. 
// Si el nodo existe y si la funcion resulta verdadera, se puede avanzar en la iteracion.
// De no cumplirse, se dejara de iterar.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

#endif //LISTA_H