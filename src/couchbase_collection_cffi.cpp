#include "couchbase_collection_cffi.h"
#include "iostream"
CouchBaseCollection* CouchBaseCollection_new() {
    return new CouchBaseCollection();
}
void CouchBaseCollection_free(CouchBaseCollection* c) {
    if (c) {
      
        delete c;
    }
}
void CouchBaseCollection_setCollection(CouchBaseCollection* c, const char* name) {
    if (c && name) {
        c->setCollection(name);
    }
}
void CouchBaseCollection_setScope(CouchBaseCollection* c, const char* scope) {
    if (c && scope) {
        c->setScope(scope);
    }
}
void CouchBaseCollection_setCouchbaseDB(CouchBaseCollection* c, CouchBaseLite* couchbase) {
    if (c && couchbase) {
        c->setCouchbaseDB(couchbase);       
    }
}
const char* CouchBaseCollection_getCollection(CouchBaseCollection* c) {
    if (c) {    
        return c->getCollection().c_str();
    }
    return nullptr;
}
const char* CouchBaseCollection_getScope(CouchBaseCollection* c) {
    if (c) {
        return c->getScope().c_str();
    }
    return nullptr;
}
CouchBaseLite* CouchBaseCollection_getCouchbaseDB(CouchBaseCollection* c) {
    if (c) {
        return c->getCouchbaseDB();
    }
    return nullptr;
}   
int CouchBaseCollection_openOrCreate(CouchBaseCollection* c) {
    if (c) {
        return c->openOrCreate() ? 1 : 0;
    }
    return 0;
}
int CouchBaseCollection_close(CouchBaseCollection* c) {
    if (c) {
        return c->close() ? 1 : 0;
    }
    return 0;
}   

char* CouchBaseCollection_saveDocument(CouchBaseCollection* c, const char* jsonDocument, const char* docId) {
    if (c && jsonDocument) {
        nlohmann::json jsonDoc = nlohmann::json::parse(jsonDocument);
        std::string docIdStr = docId ? docId : "";
        return strdup(c->saveDocument(jsonDoc, docIdStr).c_str());
    }
    return nullptr;
}
char* CouchBaseCollection_query(CouchBaseCollection* c, const char* query, const char* filterKey, int dontFilter) {
    if (c && query) {
        std::string filterKeyStr = filterKey ? filterKey : "";
        auto q=  c->query(query, filterKeyStr, dontFilter != 0);
        nlohmann::json resultJson = nlohmann::json::array();
        for (const auto& item : q) {
            resultJson.push_back(item);
        }
        std::string resultStr = resultJson.dump();
        char* result = (char*)malloc(resultStr.size() + 1);
        if (result) {
            strcpy(result, resultStr.c_str());
            return result;
        } else {
            std::cerr << "Memory allocation failed for query result string." << std::endl;  
        }
         
    
    }
    return {};
}

void CouchBaseCollection_useDefaultCollection(CouchBaseCollection* c, int useDefault) {
    if (c) {
        c->useDefaultCollection(useDefault != 0);
    }
}
int CouchBaseCollection_isDefaultCollection(CouchBaseCollection* c) {
    if (c) {
        return c->isDefaultCollection() ? 1 : 0;
    }
    return 0;
}

CBLCollection* CouchBaseCollection_getCollectionInstance(CouchBaseCollection* c) {
    if (c) {
        return c->getCollectionInstance();
    }
    return nullptr;
}

void CouchBaseCollection_collectionOpened(CouchBaseCollection* c, CollectionOpenedCallback cb) {
    if (c) {
        c->collectionOpened(cb);
    }
}

void CouchBaseCollection_collectionChanged(CouchBaseCollection* c, CollectionChangeCallback cb) {
    if (c) {
        c->collectionChanged(cb);
    }
}
extern "C" {
    void couchbase_free_string(char* str);
}
 

