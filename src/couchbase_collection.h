#ifndef COUCHBASECOLLECTION_H
#define COUCHBASECOLLECTION_H 
 
class CouchBaseLite;
#include <cbl/CouchbaseLite.h> 
#include "json.hpp"
typedef void (*CollectionChangeCallback)(void);
typedef void (*CollectionOpenedCallback)();
class CouchBaseCollection
{
    
public:
    explicit CouchBaseCollection()=default;
    ~CouchBaseCollection();
    void setCollection(std::string name);
    void setScope(std::string scope);
    void setCouchbaseDB(CouchBaseLite *couchbase);
    std::string getCollection();
    std::string getScope();
    CouchBaseLite* getCouchbaseDB();
    bool openOrCreate();
    bool close();
    std::string saveDocument( nlohmann::json jsonDocument , std::string docId = nullptr);
    std::vector<nlohmann::json> query(std::string query,std::string filterKey = "", bool dontFilter= false);


     void useDefaultCollection(bool useDefault);
     bool isDefaultCollection();

    CBLCollection *getCollectionInstance();

    void collectionOpened(CollectionOpenedCallback cb){
        this->onCollectionOpened = cb;
    }
    void collectionChanged(CollectionChangeCallback cb){
        this->onChange = cb;
    }

private:
    std::string collectionName;
    std::string scopeName;
    CouchBaseLite *couchbase;
    bool useDefault= false;
    CBLCollection *collection;

    bool collectionExist();

    void createCollection();
    bool openCollection();
    CBLListenerToken *listenerToken; 
    CollectionChangeCallback onChange = nullptr;
    CollectionOpenedCallback onCollectionOpened = nullptr;
};

#endif // COUCHBASECOLLECTION_H
