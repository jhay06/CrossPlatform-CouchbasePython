#include "couchbase_lite.h" 
#include <fleece/Fleece.h>
#include <fleece/FLSlice.h> 
#include <iostream>
 

CouchBaseLite::~CouchBaseLite()
{
    disconnect();
    //free collection
}


void CouchBaseLite::setLocalDB(const std::string &path)
{
    this->localDB = path;
}


const std::string &CouchBaseLite::getLocalDB() const
{
    return this->localDB;
}


static void getting_started_change_listener(void* context, CBLReplicator* repl, const CBLReplicatorStatus* status) {
    if(status->error.code != 0) {
        printf("Error %d / %d\n", status->error.domain, status->error.code);
    }
}

void CouchBaseLite::connect(){

    CBLError error;

    auto dbFile  =(FLSlice) FLSliceResult_CreateWith(this->localDB.c_str() , this->localDB.length());
    this->db = CBLDatabase_Open(dbFile, nullptr, &error);

    if(!this->db){
        std::cerr << "Error opening database: " << error.domain << " " << error.code << std::endl;
        FLSliceResult msg= CBLError_Message(&error);
        if (!msg) {
            std::cerr << "No error message available." << std::endl;
            return;
        }
        std::cerr << "Error message: " << msg.buf << std::endl; 
        FLSliceResult_Release(msg);
        return ;

    }


    this->connected  = true ;
    // Set the callback if provided
    if (this->onConnectedCallback) {
        this->onConnectedCallback();
    }
}



bool CouchBaseLite::isConnected() const{
    return this->connected;
}


CBLDatabase *CouchBaseLite::getCouchBase(){
    return this->db;
}

bool CouchBaseLite::disconnect() {
    if (this->db) {
        CBLError error;
        if (CBLDatabase_Close(this->db, &error)) {
            CBLDatabase_Release(this->db);
            this->db = nullptr;
            this->connected = false;
            return true;
        } else {
            std::cerr << "Error closing database: " << error.domain << " " << error.code << std::endl;
            return false;
        }
    }
    return false;
}   
