#pragma once 
#ifndef COUCHBASE_REPLICATOR_CFF_H
#define COUCHBASE_REPLICATOR_CFF_H
#include "couchbase_replicator.h"
#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*SyncStatusChangedCallback)();
typedef void (*SyncStoppedCallback)();
typedef void (*SyncActiveCallback)();
typedef void (*SyncOfflineCallback)();
typedef void (*SyncIdleCallback)();
typedef void (*SyncBusyCallback)();
CouchbaseReplicator* CouchbaseReplicator_new();
void CouchbaseReplicator_free(CouchbaseReplicator* r);
void CouchbaseReplicator_setCouchBaseDB(CouchbaseReplicator* r, CouchBaseLite* couchbase);
void CouchbaseReplicator_setTargetUrl(CouchbaseReplicator* r, const char* url);
void CouchbaseReplicator_setUsername(CouchbaseReplicator* r, const char* user);
void CouchbaseReplicator_setPassword(CouchbaseReplicator* r, const char* password);
void CouchbaseReplicator_setHeartbeat(CouchbaseReplicator* r, int heartbeat);
void CouchbaseReplicator_setMaxAttempts(CouchbaseReplicator* r, int maxAttempts);
void CouchbaseReplicator_setMaxAttemptWaitTime(CouchbaseReplicator* r, int maxAttemptWaitTime);
void CouchbaseReplicator_setAutoPurge(CouchbaseReplicator* r, int autoPurge);
void CouchbaseReplicator_setReplicationType(CouchbaseReplicator* r, int type);
void CouchbaseReplicator_addCollection(CouchbaseReplicator* r, CouchBaseCollection* collection);
CouchBaseLite* CouchbaseReplicator_getCouchBaseDB(CouchbaseReplicator* r);
const char* CouchbaseReplicator_getTargetUrl(CouchbaseReplicator* r);
const char* CouchbaseReplicator_getUsername(CouchbaseReplicator* r);
const char* CouchbaseReplicator_getPassword(CouchbaseReplicator* r);
int CouchbaseReplicator_getHeartbeat(CouchbaseReplicator* r);
int CouchbaseReplicator_getMaxAttempts(CouchbaseReplicator* r);
int CouchbaseReplicator_getMaxAttemptWaitTime(CouchbaseReplicator* r);
int CouchbaseReplicator_getAutoPurge(CouchbaseReplicator* r);
int CouchbaseReplicator_getReplicationType(CouchbaseReplicator* r);
void CouchbaseReplicator_start(CouchbaseReplicator* r);
void CouchbaseReplicator_stop(CouchbaseReplicator* r);
void CouchbaseReplicator_syncChanged(CouchbaseReplicator* r, SyncStatusChangedCallback callback);
void CouchbaseReplicator_syncStopped(CouchbaseReplicator* r, SyncStoppedCallback callback);
void CouchbaseReplicator_syncActive(CouchbaseReplicator* r, SyncActiveCallback callback);
void CouchbaseReplicator_syncOffline(CouchbaseReplicator* r, SyncOfflineCallback callback);
void CouchbaseReplicator_syncIdle(CouchbaseReplicator* r, SyncIdleCallback callback);
void CouchbaseReplicator_syncBusy(CouchbaseReplicator* r, SyncBusyCallback callback);


#if defined(__cplusplus)
}
#endif 
#endif 