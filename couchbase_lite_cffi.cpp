// couchbase_lite_cffi.cpp

#include "couchbase_lite_cffi.h" 

#include <cbl/CouchbaseLite.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <random>
#include <cstring>
static std::string generate_unique_id() {
    std::ostringstream oss;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    for (int i = 0; i < 16; ++i) {
        oss << std::hex << dis(gen);
    }
    return oss.str();
}

struct CouchBaseLite {
    CBLDatabase* db = nullptr;

    bool open(const char* path) {
        CBLError err;
        db = CBLDatabase_Open(FLStr(path), nullptr, &err);
        return db != nullptr;
    }

    void close() {
        if (db) {
            CBLDatabase_Close(db, nullptr);
            CBLDatabase_Release(db);
            db = nullptr;
        }
    }
};

struct CouchBaseCollection {
    CBLCollection* collection = nullptr;
    std::string scope= "default";
    std::string collectionName= "default";

    CBLListenerToken* listenerToken = nullptr;
    CollectionChangeCallback onChange = nullptr;
    bool create_collection(CouchBaseLite* db, const char* name, const char* scope) {
        CBLError err;
        collection = CBLDatabase_CreateCollection(db->db, FLStr(name), FLStr(scope), &err);
        return collection != nullptr;
    }

    bool open(CouchBaseLite* db, const char* name, const char* scope) {
        CBLError err;
        this->collectionName = name ? name : "default";
        this->scope = scope ? scope : "default";
        collection = CBLDatabase_Collection(db->db, FLStr(name), FLStr(scope), &err);
        return collection != nullptr;
    }

    char* couchbase_save_json(CouchBaseCollection* col, const char* docId, const char* json) {
        CBLError err;
        std::string docID = (docId && strlen(docId) > 0) ? docId : generate_unique_id();
        CBLDocument* doc = CBLDocument_CreateWithID(FLStr(docID.c_str()));
        if (!CBLDocument_SetJSON(doc, FLStr(json), &err)) {
            CBLDocument_Release(doc);
            return nullptr;
        }
        if (!CBLCollection_SaveDocument(col->collection, doc, &err)) {
            CBLDocument_Release(doc);
            return nullptr;
        }
        CBLDocument_Release(doc);
        char* result = (char*)malloc(docID.size() + 1);
        strcpy(result, docID.c_str());
        return result;
    }

    char* get_by_id(const char* docId) {
        CBLError err;
        const CBLDocument* doc = CBLCollection_GetDocument( collection, FLStr(docId), &err);
        if (!doc) return nullptr;
        FLDict props = CBLDocument_Properties(doc);
        FLStringResult json = FLValue_ToJSON((FLValue)props);
        char* result = (char*)malloc(json.size + 1);
        memcpy(result, json.buf, json.size);
        result[json.size] = '\0';
        FLSliceResult_Release(json);
        CBLDocument_Release(doc);
        return result;
    }

    char* query_n1ql(const char* whereClause) {
        CBLError err;
        std::string queryStr = std::string("SELECT * FROM ")  +this->scope+ std::string(".") +this->collectionName + " WHERE " + whereClause;
        CBLQuery* query = CBLDatabase_CreateQuery(CBLCollection_Database(collection), kCBLN1QLLanguage, FLStr(queryStr.c_str()), nullptr, &err);
        if (!query) return nullptr;

        CBLResultSet* rs = CBLQuery_Execute(query, &err);
        if (!rs) {
            CBLQuery_Release(query);
            return nullptr;
        }

        std::ostringstream out;
        out << "[";
        bool first = true;
        while (CBLResultSet_Next(rs)) {
            if (!first) out << ",";
            FLValue val = CBLResultSet_ValueAtIndex(rs, 0);
            FLStringResult json = FLValue_ToJSON(val);
            out.write((const char*)json.buf, json.size);
            FLSliceResult_Release(json);
            first = false;
        }
        out << "]";
        CBLResultSet_Release(rs);
        CBLQuery_Release(query);

        std::string resultStr = out.str();
        char* result = (char*)malloc(resultStr.size() + 1);
        strcpy(result, resultStr.c_str());
        return result;
    }

    int delete_by_id(const char* docId) {
        CBLError err;
        const CBLDocument* doc = CBLCollection_GetDocument(collection, FLStr(docId), &err);
        if (!doc) return 0;
        int success = CBLCollection_DeleteDocument(collection, doc, &err);
        CBLDocument_Release(doc);
        return success;
    }
    
    void setChangeListener() {
    if (!collection || !onChange) return;

    listenerToken = CBLCollection_AddChangeListener(collection, [](void* context, const CBLCollectionChange* change) {
        auto* col = static_cast<CouchBaseCollection*>(context);
        printf("[DEBUG] Collection change received\n");

        FLValue docIDsVal = (FLValue)change->docIDs;
        if (!docIDsVal || FLValue_GetType(docIDsVal) != kFLArray) {
            printf("[INFO] docIDs not available (type = %d). Skipping...\n", FLValue_GetType(docIDsVal));
            return;
        }

        FLArray docIDs = FLValue_AsArray(docIDsVal);
        size_t count = FLArray_Count(docIDs);
        printf("[INFO] docIDs count = %zu\n", count);

        for (size_t i = 0; i < count; ++i) {
            FLValue val = FLArray_Get(docIDs, i);
            if (!val || FLValue_GetType(val) != kFLString) {
                printf("[WARN] docID[%zu] is not a valid string\n", i);
                continue;
            }
            FLString id = FLValue_AsString(val);
            char* copied = strdup((const char*)id.buf);  // Let Python free
            col->onChange(copied);
        }
    }, this);
}

};

struct CouchbaseReplicator {
    CouchBaseLite* couchbase = nullptr;
    std::string targetUrl;
    std::string username;
    std::string password;
    int replicationType = 2;
    bool autoPurge = true;
    bool proxyEnable = false;
    std::string proxyHost;
    int proxyPort = 0;
    std::vector<CBLCollection*> collections;
    CBLReplicator* replicator = nullptr;
    void (*onStatusChanged)(int) = nullptr;

    void addCollection(CouchBaseCollection* col) {
        collections.push_back(col->collection);
    }

    void start() {
        CBLError err;
        CBLEndpoint* endpoint = CBLEndpoint_CreateWithURL(FLStr(targetUrl.c_str()), &err);
        if (!endpoint) return;

        std::vector<CBLReplicationCollection> colConfigs;
        for (auto* col : collections) {
            CBLReplicationCollection rc = {col, nullptr};
            colConfigs.push_back(rc);
        }

        CBLReplicatorConfiguration config = {};
        config.database = couchbase->db;
        config.endpoint = endpoint;
        config.replicatorType = (CBLReplicatorType)replicationType;
        config.collections = colConfigs.data();
        config.collectionCount = colConfigs.size();
        config.continuous = true;
        config.disableAutoPurge = !autoPurge;

        if (!username.empty() && !password.empty()) {
            config.authenticator = CBLAuth_CreatePassword(FLStr(username.c_str()), FLStr(password.c_str()));
        }

        if (proxyEnable) {
            static CBLProxySettings proxy = {};
            proxy.type = kCBLProxyHTTP;
            proxy.hostname = FLStr(proxyHost.c_str());
            proxy.port = proxyPort;
            config.proxy = &proxy;
        }

        replicator = CBLReplicator_Create(&config, &err);
        if (config.authenticator) {
            CBLAuth_Free(config.authenticator);
        }
        CBLEndpoint_Free(endpoint);
        if (!replicator) return;

        CBLReplicator_AddChangeListener(replicator, [](void* ctx, CBLReplicator*, const CBLReplicatorStatus* status) {
            auto* repl = (CouchbaseReplicator*)ctx;
            if (repl->onStatusChanged) repl->onStatusChanged(status->activity);
        }, this);

        CBLReplicator_Start(replicator, false);
    }

    void stop() {
        if (replicator) {
            CBLReplicator_Stop(replicator);
            CBLReplicator_Release(replicator);
            replicator = nullptr;
        }
    }
};

//extern "C" {
CouchBaseLite* CouchBaseLite_new() { return new CouchBaseLite(); }
void CouchBaseLite_free(CouchBaseLite* db) { if (db) { db->close(); delete db;
 } }
int CouchBaseLite_open(CouchBaseLite* db, const char* path) { return db->open(path) ? 1 : 0; }

CouchBaseCollection* CouchBaseCollection_new() { return new CouchBaseCollection(); }
bool CouchBaseCollection_create(CouchBaseCollection* c, CouchBaseLite* db, const char* name , const char* scope) {
    return c->create_collection(db, name, scope) ? 1 : 0;
}

void CouchBaseCollection_free(CouchBaseCollection* c){
if (c->listenerToken) {
    CBLListener_Remove(c->listenerToken);
    c->listenerToken = nullptr;
}


}
int CouchBaseCollection_open(CouchBaseCollection* c, CouchBaseLite* db, const char* name, const char* scope) {
    return c->open(db, name, scope) ? 1 : 0;
}
char* CouchBaseCollection_save_json(CouchBaseCollection* col, const char* docId, const char* json) {
    return col->couchbase_save_json(col, docId, json);
}
char* CouchBaseCollection_get_by_id(CouchBaseCollection* col, const char* docId) {
    return col->get_by_id(docId);
}
char* CouchBaseCollection_query_n1ql(CouchBaseCollection* col, const char* whereClause) {
    return col->query_n1ql(whereClause);
}
int CouchBaseCollection_delete_by_id(CouchBaseCollection* col, const char* docId) {
    return col->delete_by_id(docId);
}
void couchbase_free_string(char* str) {
    printf("C++ freeing string: %p -> %s\n", str, str);
    free(str);
}
void CouchBaseCollection_setChangeCallback(CouchBaseCollection* col, CollectionChangeCallback cb) {
    col->onChange = cb;
    col->setChangeListener();
}


CouchbaseReplicator* CouchbaseReplicator_new() { return new CouchbaseReplicator(); }
void CouchbaseReplicator_free(CouchbaseReplicator* r) { if (r) { r->stop(); delete r; } }
void CouchbaseReplicator_setCouchbase(CouchbaseReplicator* r, CouchBaseLite* db) { r->couchbase = db; }
void CouchbaseReplicator_setTargetUrl(CouchbaseReplicator* r, const char* url) { r->targetUrl = url; }
void CouchbaseReplicator_setUsername(CouchbaseReplicator* r, const char* user) { r->username = user; }
void CouchbaseReplicator_setPassword(CouchbaseReplicator* r, const char* pass) { r->password = pass; }
void CouchbaseReplicator_setReplicationType(CouchbaseReplicator* r, int type) { r->replicationType = type; }
void CouchbaseReplicator_setAutoPurge(CouchbaseReplicator* r, int purge) { r->autoPurge = purge != 0; }
void CouchbaseReplicator_setProxy(CouchbaseReplicator* r, int enable, const char* host, int port) {
    r->proxyEnable = enable != 0;
    r->proxyHost = host;
    r->proxyPort = port;
}
void CouchbaseReplicator_addCollection(CouchbaseReplicator* r, CouchBaseCollection* col) { r->addCollection(col); }
void CouchbaseReplicator_setStatusCallback(CouchbaseReplicator* r, void (*cb)(int)) { r->onStatusChanged = cb; }
void CouchbaseReplicator_start(CouchbaseReplicator* r) { r->start(); }
void CouchbaseReplicator_stop(CouchbaseReplicator* r) { r->stop(); }
//} // extern "C"
