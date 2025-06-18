#pragma once 
#ifndef COUCHBASE_LITE_H
#define COUCHBASE_LITE_H
#include <cbl/CouchbaseLite.h>
typedef void (*isConnectedCallback)(void);
class CouchBaseLite{


private:
    bool connected = false; 
    CBLDatabase* db = nullptr;
    std::string localDB;
    isConnectedCallback onConnectedCallback = nullptr;

public :
    CouchBaseLite() = default; 
    ~CouchBaseLite();
    void setLocalDB(const std::string& path);
    const std::string& getLocalDB() const;
    void connect();
    bool isConnected() const;
    bool disconnect();
    CBLDatabase* getCouchBase();
    void onConnected(isConnectedCallback cb) {
        onConnectedCallback = cb;
    }

};
#endif // COUCHBASE_LITE_H