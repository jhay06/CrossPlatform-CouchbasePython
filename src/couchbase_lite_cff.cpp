#include "couchbase_lite_cff.h"

CouchBaseLite * CouchBaseLite_new() {
    return new CouchBaseLite();
}
void CouchBaseLite_free(CouchBaseLite *db) {
    if (db) {
        if(db->isConnected()) {
            db->disconnect();
        } 
        delete db;
    }
}

void CouchBaseLite_setLocalDB(CouchBaseLite *db, const char *path) {
    if (db) {
        db->setLocalDB(path);
    }
}

int CouchBaseLite_open(CouchBaseLite *db, const char *path) {
    if (db) {
        db->connect();
        return db->isConnected() ? 1 : 0;
    }
    return 0;
}

CBLDatabase * CouchBaseLite_getCouchBase(CouchBaseLite *db) {
    if (db) {
        return db->getCouchBase();
    }
    return nullptr;
}

bool CouchBaseLite_isConnected(CouchBaseLite *db) {
    if (db) {
        return db->isConnected();
    }
    return false;
}

bool CouchBaseLite_disconnect(CouchBaseLite *db) {
    if (db) {
        return db->disconnect();
    }
    return false;
}

