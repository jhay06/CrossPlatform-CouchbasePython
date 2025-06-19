from . import ffi, lib 

ffi.cdef("""

    typedef struct CouchBaseLite CouchBaseLite;
    typedef struct CouchBaseCollection CouchBaseCollection;
        
    typedef struct CBLCollection CBLCollection;
    typedef void (*CollectionChangeCallback)(void);
    typedef void (*CollectionOpenedCallback)();
    CouchBaseCollection* CouchBaseCollection_new();
    void CouchBaseCollection_free(CouchBaseCollection* c);
    void CouchBaseCollection_setCollection(CouchBaseCollection* c, const char* name);
    void CouchBaseCollection_setScope(CouchBaseCollection* c, const char* scope);
    void CouchBaseCollection_setCouchbaseDB(CouchBaseCollection* c, CouchBaseLite* couchbase);
    const char* CouchBaseCollection_getCollection(CouchBaseCollection* c);
    const char* CouchBaseCollection_getScope(CouchBaseCollection* c);
    CouchBaseLite* CouchBaseCollection_getCouchbaseDB(CouchBaseCollection* c);
    int CouchBaseCollection_openOrCreate(CouchBaseCollection* c);
    int CouchBaseCollection_close(CouchBaseCollection* c);
    char* CouchBaseCollection_saveDocument(CouchBaseCollection* c, const char* jsonDocument, const char* docId);
    char* CouchBaseCollection_query(CouchBaseCollection* c, const char* query, const char* filterKey, int dontFilter);
    void CouchBaseCollection_useDefaultCollection(CouchBaseCollection* c, int useDefault);
    int CouchBaseCollection_isDefaultCollection(CouchBaseCollection* c);
    CBLCollection* CouchBaseCollection_getCollectionInstance(CouchBaseCollection* c);
    void CouchBaseCollection_collectionOpened(CouchBaseCollection* c, CollectionOpenedCallback cb);
    void CouchBaseCollection_collectionChanged(CouchBaseCollection* c, CollectionChangeCallback cb);
""")


class CBLCollection:
    def __init__(self):
        self._collection = lib.CouchBaseCollection_new()
        if not self._collection:
            raise RuntimeError("Failed to create CouchBaseCollection instance")
        self._collection_opened_callback = None
        self._collection_changed_callback = None

    def set_collection(self, name):
        lib.CouchBaseCollection_setCollection(self._collection, name.encode('utf-8'))

    def set_scope(self, scope):
        lib.CouchBaseCollection_setScope(self._collection, scope.encode('utf-8'))

    def set_couchbase_db(self, couchbase):
        lib.CouchBaseCollection_setCouchbaseDB(self._collection, couchbase._db)

    def open_or_create(self):
        return lib.CouchBaseCollection_openOrCreate(self._collection)

    def close(self):
        return lib.CouchBaseCollection_close(self._collection)

    def save_document(self, json_document, doc_id=None):
        return lib.CouchBaseCollection_saveDocument(self._collection, json_document.encode('utf-8'), doc_id.encode('utf-8') if doc_id else ffi.NULL)

    def query(self, query, filter_key=None, dont_filter=0):
        q=  lib.CouchBaseCollection_query(self._collection, query.encode('utf-8'), filter_key.encode('utf-8') if filter_key else ffi.NULL, dont_filter)
        import json 
        if q:
            try:
                return json.loads(ffi.string(q).decode('utf-8'))
            except json.JSONDecodeError as e:
                print(f"Error decoding JSON: {e}")
                return None
        return None
            
    def use_default_collection(self, use_default):
        lib.CouchBaseCollection_useDefaultCollection(self._collection, int(use_default))

    def is_default_collection(self):
        return bool(lib.CouchBaseCollection_isDefaultCollection(self._collection))

    def get_collection_instance(self):
        cbl_collection = lib.CouchBaseCollection_getCollectionInstance(self._collection)
        if not cbl_collection:
            raise RuntimeError("Failed to get CBL collection instance")
        return cbl_collection

    def collection_opened(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        
        self._collection_opened_callback = callback
        
        @ffi.callback("void()")
        def c_collection_opened_callback():
            if self._collection_opened_callback:
                self._collection_opened_callback()
        self._c_collection_changed_callback = c_collection_opened_callback
        lib.CouchBaseCollection_collectionOpened(self._collection, c_collection_opened_callback)

    def collection_changed(self, callback):
        if not callable(callback):
            raise ValueError("Callback must be a callable function")
        self._collection_changed_callback = callback
        @ffi.callback("void()")
        def c_collection_changed_callback():
            if self._collection_changed_callback:
                self._collection_changed_callback()
        self._c_collection_changed_callback = c_collection_changed_callback
        lib.CouchBaseCollection_collectionChanged(self._collection, c_collection_changed_callback)
        
    def __del__(self):
        if self._collection:
            lib.CouchBaseCollection_free(self._collection)
            self._collection = None
        else:
            print("CBLCollection instance already freed or not initialized.")