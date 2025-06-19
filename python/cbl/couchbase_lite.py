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
        char* CouchBaseLite_CurrentDatabaseDirectory(CouchBaseLite* db);
        void CouchBaseLite_SetDatabaseDirectory(CouchBaseLite* db, const char* path);
         
        void initContext(CouchBaseLite *db, const char* tempDir, const char* pathDir);
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
    
    def current_database_directory(self):
        try:
            dir_path = lib.CouchBaseLite_CurrentDatabaseDirectory(self._db)
            print(f"Current database directory: {dir_path}")
            if not dir_path:
                raise RuntimeError("Failed to get current database directory")
            return ffi.string(dir_path).decode('utf-8')
        except Exception as e: 
            print(f"Error getting current database directory: {e}")
            return None
    def set_database_directory(self, path):
        if not path:
            raise ValueError("Path cannot be empty")
        n_path = ffi.new("char[]", path)
        lib.CouchBaseLite_SetDatabaseDirectory(self._db, n_path)
        print(f"Database directory set to: {path}")
    
    def init_context(self, temp_dir, path_dir):
        if not temp_dir or not path_dir:
            raise ValueError("Temporary directory and path directory cannot be empty")
        n_tempdir = ffi.new("char[]", temp_dir)
        n_pathdir = ffi.new("char[]", path_dir)
        lib.initContext(self._db, n_tempdir, n_pathdir)
        print(f"Context initialized with temp_dir: {temp_dir} and path_dir: {path_dir}")
    
