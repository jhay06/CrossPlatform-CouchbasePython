from python.cbl.couchbase_lite import CouchBaseLite
from python.cbl.cbl_collection import CBLCollection, ffi
from python.cbl.cbl_replicator import CBLReplicator,ReplicationType
db = CouchBaseLite()
db.set_local_db("mydb")
db.on_connected(lambda: print("Connected to Couchbase Lite database!"))
db.open()
if db.is_connected():
    print("Connected to Couchbase Lite database.")


# Example usage of CBLCollection
collection = CBLCollection()
collection.set_collection('collection_name')  # Set the collection name
collection.set_scope("scope_name")
collection.set_couchbase_db(db)
if collection.open_or_create():
    print("Collection opened or created successfully.")

replicator = CBLReplicator()
replicator.add_collection(collection)
replicator.set_couchbase_db(db)
#replicator.set_replication_type(ReplicationType.PUSH_AND_PULL) 
replicator.set_target_url("wss://example.com/db")
replicator.set_username("username")
replicator.set_password("password")
replicator.set_heartbeat(10)
replicator.set_max_attempts(10)
replicator.set_max_attempt_wait_time(30)
replicator.set_auto_purge(True)
replicator.set_replication_type(ReplicationType.PUSH_AND_PULL)  # Set replication type to PUSH_AND_PULL
replicator.sync_changed(lambda: print("Sync status changed!"))
replicator.sync_stopped(lambda: print("Sync stopped!"))
replicator.sync_active(lambda: print("Sync is active!"))
replicator.sync_offline(lambda: print("Sync is offline!"))
replicator.sync_idle(lambda: print("Sync is idle!"))
replicator.sync_busy(lambda: print("Sync is busy!"))
replicator.start()
result = collection.query("SELECT * FROM ${sc}")
if result:
    print("Query executed successfully. Result:", result) 
    #convert cdata char * to Python string
    result_str = ffi.string(result).decode('utf-8') if result else "No result"
    print("Query Result:", result_str)



collection.collection_opened(lambda: print("Collection opened!"))
collection.collection_changed(lambda: print("Collection changed!"))

input("Press Enter to disconnect from the Couchbase Lite database...")
print('This is to make sure the db , collection and replicator are still connected before disconnecting.')
if db.is_connected():
    print("Disconnecting from Couchbase Lite database...")
replicator.stop()
if collection.close():
    print("Collection closed successfully.")

db.disconnect()
db.close()
print("Disconnected from Couchbase Lite database.")

 