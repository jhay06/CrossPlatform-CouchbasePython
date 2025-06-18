
#include "couchbase_replicator.h"
#include "couchbase_collection.h"
 
#include "couchbase_lite.h"

 #include <iostream>


CouchbaseReplicator::~CouchbaseReplicator()
{

}


void CouchbaseReplicator::setReplicationType(ReplicationType type)
{
    this->replicationType = type;
}

void CouchbaseReplicator::setAutoPurge(bool autoPurge)
{
    this->autoPurge = autoPurge;
}

void CouchbaseReplicator::setCouchBaseDB(CouchBaseLite *couchBase)
{
    this->couchbase = couchBase;
}

void CouchbaseReplicator::setHeartbeat(int heartbeat){
    this->heartbeat = heartbeat;
}

void CouchbaseReplicator::setMaxAttemptWaitTime(int maxAttemptWaitTime){
    this->maxAttemptWaitTime = maxAttemptWaitTime;
}

void CouchbaseReplicator::setMaxAttempts(int maxAttempts){
    this->maxAttempts = maxAttempts;
}

void CouchbaseReplicator::setPassword(std::string password){
    this->password = password;
}

void CouchbaseReplicator::setTargetUrl(std::string url){
    this->targetUrl = url;
}

void CouchbaseReplicator::setUsername(std::string username){
    this->username = username;
}

CouchbaseReplicator::ReplicationType CouchbaseReplicator::getReplicationType()
{
    return this->replicationType;
}

bool CouchbaseReplicator::getAutoPurge()
{
    return this->autoPurge;
}

CouchBaseLite* CouchbaseReplicator::getCouchBaseDB()
{
    return this->couchbase;
}

int CouchbaseReplicator::getHeartbeat()
{
    return this->heartbeat;
}

int CouchbaseReplicator::getMaxAttemptWaitTime()
{
    return this->maxAttemptWaitTime;
}

int CouchbaseReplicator::getMaxAttempts()
{
    return this->maxAttempts;
}

std::string CouchbaseReplicator::getPassword()
{
    return this->password;
}

std::string CouchbaseReplicator::getTargetUrl()
{
    return this->targetUrl;
}

std::string CouchbaseReplicator::getUsername()
{
    return this->username;
}

void CouchbaseReplicator::stop(){
    CBLReplicator_Stop(this->serverReplicator);
    CBLReplicator_Release(this->serverReplicator);
    this->serverReplicator = nullptr;
}

void changeListener(void* context , CBLReplicator* repl, const CBLReplicatorStatus* status){
    auto replicator = (CouchbaseReplicator*) context;
    if(!replicator){
        return ;
    }

    switch(status->activity){
        case kCBLReplicatorStopped:
            std::cout << "Replicator stopped" << std::endl;
            if(replicator->onStatusChanged){
                replicator->onStatusChanged();
            }
            break;
        case kCBLReplicatorOffline:
            std::cout << "Replicator offline" << std::endl;
            if(replicator->onSyncOffline){
                replicator->onSyncOffline();
            } 
            break;
        case kCBLReplicatorConnecting:
            std::cout << "Replicator connecting" << std::endl;
            break;
        case kCBLReplicatorIdle:
            std::cout << "Replicator idle" << std::endl;
            if(replicator->onSyncIdle){
                replicator->onSyncIdle();
            } 
            break;
        case kCBLReplicatorBusy:
            std::cout << "Replicator busy" << std::endl;
            if(replicator->onSyncBusy){
                replicator->onSyncBusy();
            }
           
        default:
            break;

    }




}
 

void CouchbaseReplicator::start(){
     CBLLog_SetConsoleLevel(kCBLLogDebug);
  
    if(!this->couchbase->isConnected()){

        std::cerr << "Couchbase not connected" << std::endl;
        return;
    }
    CBLError err;
    std::cout << "Creating replicator" << std::endl;
    CBLEndpoint *targetEndpoint = CBLEndpoint_CreateWithURL(FLStr(this->targetUrl.c_str()), &err);

    if(!targetEndpoint){
        std::cerr << "Failed to create endpoint" << std::endl;
        std::cerr << "Error opening database: " << err.domain << " " << err.code << std::endl;
        FLSliceResult msg= CBLError_Message(&err);
        std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        FLSliceResult_Release(msg);
        return;

    }
    std::cout << "Endpoint created"  <<  this->targetUrl <<    std::endl;
    int len = this->collections.size();
    if(len == 0){
        std::cerr << "No collections added to replicator" << std::endl;
        return;
    }
    CBLReplicationCollection* collectionConfig = new CBLReplicationCollection[len];
//    collectionConfig.collection    =  this->collections.at(0)->getCollectionInstance();
    memset(collectionConfig, 0, sizeof(collectionConfig[0]));
    for(int i=0; i <  len; i++){
        CBLReplicationCollection col={0};
        col.collection = this->collections.at(i)->getCollectionInstance();

        collectionConfig[i] = col;

    }

    CBLReplicatorConfiguration replConfig = {0};
    memset(&replConfig, 0, sizeof(replConfig));
    replConfig.database ={0};
    replConfig.disableAutoPurge = !this->autoPurge;
    replConfig.endpoint = targetEndpoint;
    
    replConfig.collectionCount =len;
    

    replConfig.collections = collectionConfig;

    switch((int) this->replicationType){
    case ReplicationType::PUSH:
        replConfig.replicatorType = kCBLReplicatorTypePush;
        break;

    case ReplicationType::PULL:
        replConfig.replicatorType = kCBLReplicatorTypePull;
        break;

    case ReplicationType::PUSH_PULL:
       
        replConfig.replicatorType = kCBLReplicatorTypePushAndPull;
        break;
    }
    replConfig.continuous = true;

    replConfig.heartbeat = this->heartbeat;
    replConfig.maxAttemptWaitTime = this->maxAttemptWaitTime;
    replConfig.maxAttempts = this->maxAttempts;
    CBLAuthenticator *basicAuth = nullptr;
    if(!this->username.empty() && !this->password.empty()){
        std::cout << "Setting up basic authentication" << std::endl;
         
        basicAuth = CBLAuth_CreatePassword(FLStr(this->username.c_str()),
                                           FLStr(this->password.c_str()));
        replConfig.authenticator = basicAuth;

    }


    serverReplicator = CBLReplicator_Create(&replConfig, &err); 
    CBLEndpoint_Free(targetEndpoint);
    if(basicAuth != nullptr){
        CBLAuth_Free(basicAuth);
    }
    if(!serverReplicator){
        std::cerr << "Failed to create replicator" << std::endl;
        std::cerr << "Error opening database: " << err.domain << " " << err.code << std::endl;
        FLSliceResult msg= CBLError_Message(&err);
        std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        if(collectionConfig){
            delete[] collectionConfig;
        }
        if(serverReplicator){
            CBLReplicator_Release(serverReplicator);
            serverReplicator = nullptr;
        }
        if(basicAuth){
            CBLAuth_Free(basicAuth);
        } 
        FLSliceResult_Release(msg);
        return;
    }
    std::cout << "Replicator started" << std::endl;
    CBLListenerToken *token = CBLReplicator_AddChangeListener(serverReplicator, changeListener, this);
    CBLReplicator_Start(this->serverReplicator, true);



}


void CouchbaseReplicator::addCollection(CouchBaseCollection *collection){
    //check if collection is already added
    if(!collection){
        std::cerr << "Collection is null" << std::endl;
        return;
    }
    int len = this->collections.size();
    
    for(int i=0; i < len; i++){
        if(this->collections.at(i)->getCollectionInstance() == collection->getCollectionInstance()){
            std::cout << "Collection already added" << std::endl;
            return;
        }
    }
    this->collections.push_back(collection);
}

