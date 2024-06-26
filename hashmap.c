#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"
#include <stdbool.h>
#include <assert.h>


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

bool aviable(Pair **buckets, long pos) {
  if (buckets[pos] == NULL || buckets[pos]->key == NULL) return true;
  return false;
}

void insertPair(Pair **buckets, char * key, void * value, long pos) {
  if (buckets[pos] == NULL) buckets[pos] = createPair(key, value);
  buckets[pos]->key = key;
  buckets[pos]->value = value;
}

void insertMap(HashMap * map, char * key, void * value) {
    if(map==NULL || key==NULL) return;
    if(map->size > 0.7 * map->capacity) enlarge(map);
  
    long hashkey = hash(key, map->capacity);
    long i = hashkey;
  
    do {
      if (aviable(map->buckets, i)) {
        insertPair(map->buckets, key, value, i);
        map->size++;
        map->current = i;
        return;
      } i = (i + 1) % map->capacity;
    } while (i != hashkey);
}

void enlarge(HashMap * map) {
  enlarge_called = 1; //no borrar (testing purposes)
  
  long old_capacity = map->capacity;
  map->capacity *= 2;
  
  Pair ** old_buckets = map->buckets;
  Pair ** new_buckets = (Pair **) calloc (map->capacity, sizeof(Pair *));
  assert(new_buckets != NULL);
  map->buckets = new_buckets;
  
  for (long i = 0 ; i < old_capacity ; i++) {
    if (old_buckets[i] != NULL) insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
  }
  free(old_buckets);
}

HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    assert(map != NULL);
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    assert(map->buckets != NULL);
    map->size = 0;
    map->capacity = capacity;
    map->current = -1;
    return map;
}

void eraseMap(HashMap * map,  char * key) {    
  Pair *deleted = searchMap(map, key);
  if (deleted != NULL) {
    deleted->key = NULL;
    map->size -= 1;
  }
}

Pair * searchMap(HashMap * map,  char * key) {   
  long hashkey = hash(key, map->capacity);
  while (map->buckets[hashkey] != NULL) {
    if (is_equal(map->buckets[hashkey]->key, key)) {
      map->current = hashkey;
      return map->buckets[hashkey];
    } hashkey = (hashkey + 1) % map->capacity;
  } return NULL;
}

Pair * firstMap(HashMap * map) {
  for (long i = 0; i < map->capacity; i++) {
    if (map->buckets[i] != NULL && map->buckets[i]->key) {
      map->current = i;
      return map->buckets[i];
    }
  } return NULL;
}

Pair * nextMap(HashMap * map) {
  for (long i = map->current + 1; i < map->capacity; i++) {
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
      map->current = i;
      return map->buckets[i];
    }
  } return NULL;
}
