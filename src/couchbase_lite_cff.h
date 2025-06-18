#pragma once
#ifndef COUCHBASE_LITE_CFF_H
#define COUCHBASE_LITE_CFF_H
#include <iostream>
#include "couchbase_lite.h"
#if defined(__cplusplus)

extern "C" {
#endif 
typedef void (*isConnectedCallback)(void);
CouchBaseLite* CouchBaseLite_new();
void CouchBaseLite_free(CouchBaseLite* db);
void CouchBaseLite_setLocalDB(CouchBaseLite* db, const char* path);
int CouchBaseLite_open(CouchBaseLite* db);
CBLDatabase* CouchBaseLite_getCouchBase(CouchBaseLite* db);
bool CouchBaseLite_isConnected(CouchBaseLite* db);
bool CouchBaseLite_disconnect(CouchBaseLite* db);
void CouchBaseLite_onConnected(CouchBaseLite* db, isConnectedCallback cb) {
    if (db) {
        std::cout << "Setting onConnected callback." << std::endl;
        db->onConnected(cb);
    }
}
#if defined(__cplusplus)
}
#endif 
#endif 