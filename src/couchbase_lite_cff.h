#pragma once
#ifndef COUCHBASE_LITE_CFF_H
#define COUCHBASE_LITE_CFF_H

#include "couchbase_lite.h"
#if defined(__cplusplus)

extern "C" {
#endif
CouchBaseLite* CouchBaseLite_new();
void CouchBaseLite_free(CouchBaseLite* db);
void CouchBaseLite_setLocalDB(CouchBaseLite* db, const char* path);
int CouchBaseLite_open(CouchBaseLite* db, const char* path);
CBLDatabase* CouchBaseLite_getCouchBase(CouchBaseLite* db);
bool CouchBaseLite_isConnected(CouchBaseLite* db);
bool CouchBaseLite_disconnect(CouchBaseLite* db);

#if defined(__cplusplus)
}
#endif 
#endif 