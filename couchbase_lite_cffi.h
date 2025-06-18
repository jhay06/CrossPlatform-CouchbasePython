// couchbase_lite_cffi.h

#pragma once

#ifdef __cplusplus

extern "C" {
#endif

typedef void (*CollectionChangeCallback)(const char* docID);
struct CouchBaseLite;
struct CouchBaseCollection;
struct CouchbaseReplicator;

CouchBaseLite* CouchBaseLite_new();
void CouchBaseLite_free(CouchBaseLite* db);
int CouchBaseLite_open(CouchBaseLite* db, const char* path);

CouchBaseCollection* CouchBaseCollection_new();
void CouchBaseCollection_free(CouchBaseCollection* c);
bool CouchBaseCollection_create(CouchBaseCollection* c, CouchBaseLite* db, const char* name, const char* scope);
int CouchBaseCollection_open(CouchBaseCollection* c, CouchBaseLite* db, const char* name, const char* scope);
char* CouchBaseCollection_save_json(CouchBaseCollection* col, const char* docId, const char* json);
char* CouchBaseCollection_get_by_id(CouchBaseCollection* col, const char* docId);
char* CouchBaseCollection_query_n1ql(CouchBaseCollection* col, const char* whereClause);
int CouchBaseCollection_delete_by_id(CouchBaseCollection* col, const char* docId);
void couchbase_free_string(char* str);
void CouchBaseCollection_setChangeCallback(CouchBaseCollection* col, CollectionChangeCallback cb) ;
CouchbaseReplicator* CouchbaseReplicator_new();
void CouchbaseReplicator_free(CouchbaseReplicator* r);
void CouchbaseReplicator_setCouchbase(CouchbaseReplicator* r, CouchBaseLite* db);
void CouchbaseReplicator_setTargetUrl(CouchbaseReplicator* r, const char* url);
void CouchbaseReplicator_setUsername(CouchbaseReplicator* r, const char* user);
void CouchbaseReplicator_setPassword(CouchbaseReplicator* r, const char* pass);
void CouchbaseReplicator_setReplicationType(CouchbaseReplicator* r, int type);
void CouchbaseReplicator_setAutoPurge(CouchbaseReplicator* r, int purge);
void CouchbaseReplicator_setProxy(CouchbaseReplicator* r, int enable, const char* host, int port);
void CouchbaseReplicator_addCollection(CouchbaseReplicator* r, CouchBaseCollection* col);
void CouchbaseReplicator_setStatusCallback(CouchbaseReplicator* r, void (*cb)(int));
void CouchbaseReplicator_start(CouchbaseReplicator* r);
void CouchbaseReplicator_stop(CouchbaseReplicator* r);

#ifdef __cplusplus
}
#endif
