#include "couchbase_replicator_cffi.h"
CouchbaseReplicator* CouchbaseReplicator_new() {
    return new CouchbaseReplicator();
}
void CouchbaseReplicator_free(CouchbaseReplicator* r) {
    if (r) {
        r->stop();
        delete r;
    }
}
void CouchbaseReplicator_setCouchBaseDB(CouchbaseReplicator* r
, CouchBaseLite* couchbase) {
    if (r && couchbase) {
        r->setCouchBaseDB(couchbase);
    }
}
void CouchbaseReplicator_setTargetUrl(CouchbaseReplicator* r, const
char* url) {
    if (r && url) {
        r->setTargetUrl(url);
    }
}
void CouchbaseReplicator_setUsername(CouchbaseReplicator* r, const char* user) {
    if (r && user) {
        r->setUsername(user);
    }
}
void CouchbaseReplicator_setPassword(CouchbaseReplicator* r, const char* password) {
    if (r && password) {
        r->setPassword(password);   
    }
}
void CouchbaseReplicator_setHeartbeat(CouchbaseReplicator* r, int heartbeat) {
    if (r) {
        r->setHeartbeat(heartbeat);
    }
}
void CouchbaseReplicator_setMaxAttempts(CouchbaseReplicator* r, int maxAttempts) {
    if (r) {
        r->setMaxAttempts(maxAttempts);
    }
}
void CouchbaseReplicator_setMaxAttemptWaitTime(CouchbaseReplicator* r, int maxAttemptWaitTime) {
    if (r) {
        r->setMaxAttemptWaitTime(maxAttemptWaitTime);
    }
}
void CouchbaseReplicator_setAutoPurge(CouchbaseReplicator* r, int autoPurge) {
    if (r) {
        r->setAutoPurge(autoPurge != 0);
    }       
}
void CouchbaseReplicator_setReplicationType(CouchbaseReplicator* r, int type) {
    if (r) {    
        r->setReplicationType(static_cast<CouchbaseReplicator::ReplicationType>(type));
    }
}
void CouchbaseReplicator_addCollection(CouchbaseReplicator* r, CouchBaseCollection* collection) {
    if (r && collection) {
        r->addCollection(collection);   
    }
}   
CouchBaseLite* CouchbaseReplicator_getCouchBaseDB(CouchbaseReplicator* r) {
    if (r) {
        return r->getCouchBaseDB();
    }
    return nullptr;
}
const char* CouchbaseReplicator_getTargetUrl(CouchbaseReplicator* r) {
    if (r) {
        return r->getTargetUrl().c_str();
    }
    return nullptr;
}
const char* CouchbaseReplicator_getUsername(CouchbaseReplicator* r) {
    if (r) {
        return r->getUsername().c_str();
    }
    return nullptr;
}
const char* CouchbaseReplicator_getPassword(CouchbaseReplicator* r) {
    if (r) {
        return r->getPassword().c_str();
    }
    return nullptr;
}
int CouchbaseReplicator_getHeartbeat(CouchbaseReplicator* r) {
    if (r) {
        return r->getHeartbeat();
    }
    return 0;
}
int CouchbaseReplicator_getMaxAttempts(CouchbaseReplicator* r) {
    if (r) {
        return r->getMaxAttempts();
    }
    return 0;
}
int CouchbaseReplicator_getMaxAttemptWaitTime(CouchbaseReplicator* r) {
    if (r) {
        return r->getMaxAttemptWaitTime();
    }
    return 0;
}
int CouchbaseReplicator_getAutoPurge(CouchbaseReplicator* r) {
    if (r) {
        return r->getAutoPurge() ? 1 : 0;
    }
    return 0;
}   
int CouchbaseReplicator_getReplicationType(CouchbaseReplicator* r) {
    if (r) {
        return static_cast<int>(r->getReplicationType());
    }
    return -1; // Invalid type
}

void CouchbaseReplicator_start(CouchbaseReplicator* r) {
    if (r) {
        r->start();
    }
}
void CouchbaseReplicator_stop(CouchbaseReplicator* r) {
    if (r) {
        r->stop();
    }
}
void CouchbaseReplicator_syncChanged(CouchbaseReplicator* r, SyncStatusChangedCallback callback) {
    if (r && callback) {
        r->syncChanged(callback);
    }
}
void CouchbaseReplicator_syncStopped(CouchbaseReplicator* r, SyncStoppedCallback callback) {
    if (r && callback) {
        r->syncStopped(callback);
    }
}
void CouchbaseReplicator_syncActive(CouchbaseReplicator* r, SyncActiveCallback callback) {
    if (r && callback) {
        r->syncActive(callback);
    }
}
void CouchbaseReplicator_syncOffline(CouchbaseReplicator* r, SyncOfflineCallback callback) {
    if (r && callback) {
        r->syncOffline(callback);
    }
}
void CouchbaseReplicator_syncIdle(CouchbaseReplicator* r, SyncIdleCallback callback) {
    if (r && callback) {
        r->syncIdle(callback);
    }
}
void CouchbaseReplicator_syncBusy(CouchbaseReplicator* r, SyncBusyCallback callback) {
    if (r && callback) {
        r->syncBusy(callback);
    }
}   
