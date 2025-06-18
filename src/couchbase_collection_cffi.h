#pragma once 
#ifndef COUCHBASE_COLLECTION_CFF_H
#define COUCHBASE_COLLECTION_CFF_H
#include "couchbase_collection.h"
#if defined(__cplusplus)
extern "C" {
#endif
    typedef void (*CollectionChangeCallback)(void);
    typedef void (*CollectionOpenedCallback)();
    CouchBaseCollection* CouchBaseCollection_new();
    void CouchBaseCollection_free(CouchBaseCollection* c);
    void CouchBaseCollection_setCollection(CouchBaseCollection* c, const char* name);
    void CouchBaseCollection_setScope(CouchBaseCollection* c, const char* scope);
    void CouchBaseCollection_setCouchbaseDB(CouchBaseCollection* c, CouchBaseLite* couchbase);
    const char* CouchBaseCollection_getCollection(CouchBaseCollection* c);
    const char* CouchBaseCollection_getScope(CouchBaseCollection* c);
    CouchBaseLite* CouchBaseCollection_getCouchbaseDB(CouchBaseCollection* c);
    int CouchBaseCollection_openOrCreate(CouchBaseCollection* c);
    int CouchBaseCollection_close(CouchBaseCollection* c);
    char* CouchBaseCollection_saveDocument(CouchBaseCollection* c, const char* jsonDocument, const char* docId);
    char* CouchBaseCollection_query(CouchBaseCollection* c, const char* query, const char* filterKey, int dontFilter);
    void CouchBaseCollection_useDefaultCollection(CouchBaseCollection* c, int useDefault);
    int CouchBaseCollection_isDefaultCollection(CouchBaseCollection* c);
    CBLCollection* CouchBaseCollection_getCollectionInstance(CouchBaseCollection* c);
    void CouchBaseCollection_collectionOpened(CouchBaseCollection* c, CollectionOpenedCallback cb);
    void CouchBaseCollection_collectionChanged(CouchBaseCollection* c, CollectionChangeCallback cb);
#if defined(__cplusplus)
}
#endif 
#endif 