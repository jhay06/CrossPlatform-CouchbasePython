#ifndef COUCHBASEREPLICATOR_H
#define COUCHBASEREPLICATOR_H
 

#include <cbl++/CouchbaseLite.hh>
#include <vector>
typedef void (*SyncStatusChangedCallback)();
typedef void (*SyncStoppedCallback)();
typedef void (*SyncActiveCallback)();
typedef void (*SyncOfflineCallback)();
typedef void (*SyncIdleCallback)();
typedef void (*SyncBusyCallback)();

class CouchBaseCollection;
class CouchBaseLite;
class CouchbaseReplicator
{
    
public:
    explicit CouchbaseReplicator()=default;
    ~CouchbaseReplicator();
    enum ReplicationType{
        PUSH,
        PULL,
        PUSH_PULL
    }; 
public:
     void setCouchBaseDB(CouchBaseLite *couchbase);
     void setTargetUrl(std::string url);
     void setUsername(std::string username);
     void setPassword(std::string password);
     void setHeartbeat(int heartbeat);
      void setMaxAttempts(int maxAttempts);
     void setMaxAttemptWaitTime(int maxAttemptWaitTime);
     void setAutoPurge(bool autoPurge);
     void setReplicationType(ReplicationType type);
     void addCollection(CouchBaseCollection *collection);
     CouchBaseLite *getCouchBaseDB();
     std::string getTargetUrl();
     std::string getUsername();
     std::string getPassword();
     int getHeartbeat();
     int getMaxAttempts();
     int getMaxAttemptWaitTime();
     bool getAutoPurge();
     ReplicationType getReplicationType();

     void start();
     void stop();

    void syncChanged(SyncStatusChangedCallback callback){
        this->onStatusChanged = callback;
    }
    void syncStopped(SyncStoppedCallback callback){
        this->onSyncStopped = callback;
    }
    void syncActive(SyncActiveCallback callback){
        this->onSyncActive = callback;
    }
    void syncOffline(SyncOfflineCallback callback){
        this->onSyncOffline = callback;     
    }
    void syncIdle(SyncIdleCallback callback){
        this->onSyncIdle = callback;
    }
    void syncBusy(SyncBusyCallback callback){
        this->onSyncBusy = callback;
    } 

    SyncStatusChangedCallback onStatusChanged = nullptr;
    SyncStoppedCallback onSyncStopped = nullptr;
    SyncActiveCallback onSyncActive = nullptr;
    SyncOfflineCallback onSyncOffline = nullptr;
    SyncIdleCallback onSyncIdle = nullptr;
    SyncBusyCallback onSyncBusy = nullptr;
private:
    CouchBaseLite *couchbase;
    std::string targetUrl;
    std::string username;
    std::string password;
    ReplicationType replicationType;
    int heartbeat;
    int maxAttempts;
    int maxAttemptWaitTime;
    bool autoPurge;
    std::vector<CouchBaseCollection*> collections;
    CBLReplicator *serverReplicator;
    
};

#endif // COUCHBASEREPLICATOR_H
