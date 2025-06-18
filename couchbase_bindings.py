from cffi import FFI
import os
import time

ffi = FFI()
ffi.cdef(""" 
 typedef void (*CollectionChangeCallback)(const char* docId);
typedef struct CouchBaseLite CouchBaseLite;
typedef struct CouchBaseCollection CouchBaseCollection;
typedef struct CouchbaseReplicator CouchbaseReplicator;

CouchBaseLite* CouchBaseLite_new();
void CouchBaseLite_free(CouchBaseLite* db);
int CouchBaseLite_open(CouchBaseLite* db, const char* path);

CouchBaseCollection* CouchBaseCollection_new();
void CouchBaseCollection_free(CouchBaseCollection* c);
int CouchBaseCollection_open(CouchBaseCollection* c, CouchBaseLite* db, const char* name, const char* scope);
int CouchBaseCollection_create(CouchBaseCollection* c, CouchBaseLite* db, const char* name, const char* scope);
char* CouchBaseCollection_save_json(CouchBaseCollection* col, const char* docId, const char* json);
char* CouchBaseCollection_get_by_id(CouchBaseCollection* col, const char* docId);
char* CouchBaseCollection_query_n1ql(CouchBaseCollection* col, const char* whereClause);
int CouchBaseCollection_delete_by_id(CouchBaseCollection* col, const char* docId);

void couchbase_free_string(char* str);
void CouchBaseCollection_setChangeCallback(CouchBaseCollection* col, CollectionChangeCallback cb);
 

CouchbaseReplicator* CouchbaseReplicator_new();
void CouchbaseReplicator_free(CouchbaseReplicator* r);
void CouchbaseReplicator_setCouchbase(CouchbaseReplicator* r, CouchBaseLite* db);
void CouchbaseReplicator_setTargetUrl(CouchbaseReplicator* r, const char* url);
void CouchbaseReplicator_setUsername(CouchbaseReplicator* r, const char* user);
void CouchbaseReplicator_setPassword(CouchbaseReplicator* r, const char* pass);
void CouchbaseReplicator_setReplicationType(CouchbaseReplicator* r, int type);
void CouchbaseReplicator_setAutoPurge(CouchbaseReplicator* r, int purge);
void CouchbaseReplicator_setProxy(CouchbaseReplicator* r, int enable, const char* host, int port);
void CouchbaseReplicator_addCollection(CouchbaseReplicator* r, CouchBaseCollection* col);
void CouchbaseReplicator_setStatusCallback(CouchbaseReplicator* r, void (*cb)(int));
void CouchbaseReplicator_start(CouchbaseReplicator* r);
void CouchbaseReplicator_stop(CouchbaseReplicator* r);
    
""")

lib_path = os.path.join(os.path.dirname(__file__), "lib", "couchbase_lite_cffi.dylib")
lib = ffi.dlopen(lib_path)

# Optional: Callback for replicator status
@ffi.callback("void(int)")
def status_callback(status):
    print(f"[Python] Replicator status changed: {status}")

# -- Example usage (you can uncomment for testing) -- 
db = lib.CouchBaseLite_new()
assert lib.CouchBaseLite_open(db, b"mydb")

col = lib.CouchBaseCollection_new()
assert lib.CouchBaseCollection_create(col, db, b"QueueingSystem", b"CaptivePortal")
assert lib.CouchBaseCollection_open(col, db, b"QueueingSystem", b"CaptivePortal")
#show all data in collection  using N1QL query
result = lib.CouchBaseCollection_query_n1ql(col,b"")
 

# Step 3: Create and configure replicator
replicator = lib.CouchbaseReplicator_new()
lib.CouchbaseReplicator_setCouchbase(replicator, db)
lib.CouchbaseReplicator_setTargetUrl(replicator, b"wss://gcclu0kvaikcdkql.apps.cloud.couchbase.com:4984/queue_manager")
lib.CouchbaseReplicator_setUsername(replicator, b"queue_admin")
lib.CouchbaseReplicator_setPassword(replicator, b"P@ssw0rd123")
lib.CouchbaseReplicator_setReplicationType(replicator, 3)  # 1 = push, 2 = pull, 3 = both
 
lib.CouchbaseReplicator_setStatusCallback(replicator, status_callback)
lib.CouchbaseReplicator_addCollection(replicator, col)

# Step 4: Start replication
lib.CouchbaseReplicator_start(replicator)
print("[INFO] Replication started.")

# Step 5: Let it run for a while
try:
    while True:
        time.sleep(2)
except KeyboardInterrupt:
    print("[INFO] Stopping replication...")
    lib.CouchbaseReplicator_stop(replicator)

# Cleanup
lib.CouchbaseReplicator_free(replicator)
lib.CouchBaseCollection_free(col)
lib.CouchBaseLite_free(db)