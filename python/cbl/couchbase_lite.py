from . import ffi , lib 

ffi.cdef("""
        typedef struct CouchBaseLite CouchBaseLite;
        typedef struct CBLDatabase CBLDatabase;
        typedef void (*isConnectedCallback)(void);
        CouchBaseLite* CouchBaseLite_new();
        void CouchBaseLite_free(CouchBaseLite* db);
        void CouchBaseLite_setLocalDB(CouchBaseLite* db, const char* path);
        int CouchBaseLite_open(CouchBaseLite* db);
        CBLDatabase* CouchBaseLite_getCouchBase(CouchBaseLite* db);
        bool CouchBaseLite_isConnected(CouchBaseLite* db);
        bool CouchBaseLite_disconnect(CouchBaseLite* db);
        void CouchBaseLite_onConnected(CouchBaseLite* db, isConnectedCallback callback);
 """)

class CouchBaseLite:
    def __init__(self):
        self._connected_callback = None
        self._db = lib.CouchBaseLite_new()
        if not self._db:
            raise RuntimeError("Failed to create CouchBaseLite instance")
    
    def open(self): 
        if not lib.CouchBaseLite_open(self._db):
            raise RuntimeError("Failed to open CouchBaseLite database")
        return self._db
    
    def close(self):
        lib.CouchBaseLite_free(self._db)
        self._db = None
    
    def is_connected(self):
        return lib.CouchBaseLite_isConnected(self._db)
    
    def disconnect(self):
        return lib.CouchBaseLite_disconnect(self._db)
    
    def set_local_db(self, path):
        lib.CouchBaseLite_setLocalDB(self._db, path.encode('utf-8'))
    def get_couchbase(self):
        cbl_db = lib.CouchBaseLite_getCouchBase(self._db)
        if not cbl_db:
            raise RuntimeError("Failed to get Couchbase database instance")
        return cbl_db
    def __del__(self):
        if self._db:
            lib.CouchBaseLite_free(self._db)
            self._db = None
        else:
            print("CouchBaseLite instance already freed or not initialized.")
    
    def on_connected(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._connected_callback = callback
        
        # Define a C callback function that calls the Python callback
        @ffi.callback("void()")
        def c_connected_callback():
            if self._connected_callback:
                self._connected_callback()
        
        # Set the C callback in the CouchBaseLite instance
        self._c_connected_callback = c_connected_callback 
        lib.CouchBaseLite_onConnected(self._db, c_connected_callback)
    
