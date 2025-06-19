from . import ffi , lib 
ffi.cdef("""
typedef struct CouchBaseLite CouchBaseLite;
typedef struct CouchBaseCollection CouchBaseCollection;
typedef struct CBLCollection CBLCollection;
typedef struct CouchbaseReplicator CouchbaseReplicator;
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

""")
import enum 
class ReplicationType(enum.Enum):
    PUSH = 0
    PULL = 1
    PUSH_AND_PULL = 2

class CBLReplicator:
    def __init__(self):
        self._replicator = lib.CouchbaseReplicator_new()
        if not self._replicator:
            raise RuntimeError("Failed to create CouchbaseReplicator instance")
        self._sync_status_changed_callback = None
        self._sync_stopped_callback = None
        self._sync_active_callback = None
        self._sync_offline_callback = None
        self._sync_idle_callback = None
        self._sync_busy_callback = None

    def set_couchbase_db(self, couchbase):
        lib.CouchbaseReplicator_setCouchBaseDB(self._replicator, couchbase._db)

    def set_target_url(self, url):
        lib.CouchbaseReplicator_setTargetUrl(self._replicator, url.encode('utf-8'))

    def set_username(self, username):
        lib.CouchbaseReplicator_setUsername(self._replicator, username.encode('utf-8'))

    def set_password(self, password):
        lib.CouchbaseReplicator_setPassword(self._replicator, password.encode('utf-8'))

    def set_heartbeat(self, heartbeat):
        lib.CouchbaseReplicator_setHeartbeat(self._replicator, heartbeat)

    def set_max_attempts(self, max_attempts):
        lib.CouchbaseReplicator_setMaxAttempts(self._replicator, max_attempts)

    def set_max_attempt_wait_time(self, max_wait_time):
        lib.CouchbaseReplicator_setMaxAttemptWaitTime(self._replicator, max_wait_time)

    def set_auto_purge(self, auto_purge):
        lib.CouchbaseReplicator_setAutoPurge(self._replicator, int(auto_purge))

    def set_replication_type(self, replication_type:ReplicationType):
        lib.CouchbaseReplicator_setReplicationType(self._replicator, replication_type.value)

    def add_collection(self, collection):
        lib.CouchbaseReplicator_addCollection(self._replicator, collection._collection)

    def start(self):
        lib.CouchbaseReplicator_start(self._replicator)

    def stop(self):
        lib.CouchbaseReplicator_stop(self._replicator)

    def sync_changed(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_status_changed_callback = callback
        @ffi.callback("void()")
        def c_sync_status_changed_callback():
            if self._sync_status_changed_callback:
                self._sync_status_changed_callback()
        self._c_sync_status_changed_callback = c_sync_status_changed_callback
        lib.CouchbaseReplicator_syncChanged(self._replicator, c_sync_status_changed_callback)
    def sync_stopped(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_stopped_callback = callback
        @ffi.callback("void()")
        def c_sync_stopped_callback():
            if self._sync_stopped_callback:
                self._sync_stopped_callback()
        self._c_sync_stopped_callback = c_sync_stopped_callback
        lib.CouchbaseReplicator_syncStopped(self._replicator, c_sync_stopped_callback)
    def sync_active(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_active_callback = callback
        @ffi.callback("void()")
        def c_sync_active_callback():
            if self._sync_active_callback:
                self._sync_active_callback()
        self._c_sync_active_callback = c_sync_active_callback
        lib.CouchbaseReplicator_syncActive(self._replicator, c_sync_active_callback)
    def sync_offline(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_offline_callback = callback
        @ffi.callback("void()")
        def c_sync_offline_callback():
            if self._sync_offline_callback:
                self._sync_offline_callback()
        self._c_sync_offline_callback = c_sync_offline_callback
        lib.CouchbaseReplicator_syncOffline(self._replicator, c_sync_offline_callback)
    def sync_idle(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_idle_callback = callback
        @ffi.callback("void()")
        def c_sync_idle_callback():
            if self._sync_idle_callback:
                self._sync_idle_callback()
        self._c_sync_idle_callback = c_sync_idle_callback
        lib.CouchbaseReplicator_syncIdle(self._replicator, c_sync_idle_callback)
    def sync_busy(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._sync_busy_callback = callback
        @ffi.callback("void()")
        def c_sync_busy_callback():
            if self._sync_busy_callback:
                self._sync_busy_callback()
        self._c_sync_busy_callback = c_sync_busy_callback
        lib.CouchbaseReplicator_syncBusy(self._replicator, c_sync_busy_callback)    
    def get_couchbase_db(self):
        cbl_db = lib.CouchbaseReplicator_getCouchBaseDB(self._replicator)
        if not cbl_db:
            raise RuntimeError("Failed to get Couchbase database instance")
        return cbl_db
    def get_target_url(self):
        url = lib.CouchbaseReplicator_getTargetUrl(self._replicator)
        if not url:
            raise RuntimeError("Failed to get target URL")
        return ffi.string(url).decode('utf-8')
    def get_username(self):
        username = lib.CouchbaseReplicator_getUsername(self._replicator)
        if not username:
            raise RuntimeError("Failed to get username")
        return ffi.string(username).decode('utf-8')
    
    def get_password(self):
        password = lib.CouchbaseReplicator_getPassword(self._replicator)
        if not password:
            raise RuntimeError("Failed to get password")
        return ffi.string(password).decode('utf-8')
    
    def get_heartbeat(self):
        return lib.CouchbaseReplicator_getHeartbeat(self._replicator)
    
    def get_max_attempts(self):
        return lib.CouchbaseReplicator_getMaxAttempts(self._replicator)
    
    def get_max_attempt_wait_time(self):
        return lib.CouchbaseReplicator_getMaxAttemptWaitTime(self._replicator)
    
    def get_auto_purge(self):
        return bool(lib.CouchbaseReplicator_getAutoPurge(self._replicator))
    
    def get_replication_type(self):
        return lib.CouchbaseReplicator_getReplicationType(self._replicator)
    
    def __del__(self):
        if self._replicator:
            lib.CouchbaseReplicator_free(self._replicator)
            self._replicator = None
        else:
            print("CouchbaseReplicator instance already freed or not initialized.")