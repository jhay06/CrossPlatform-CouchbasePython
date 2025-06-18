import os 
from cffi import FFI
ffi = FFI()
lib_path = os.path.join(os.path.dirname(__file__), "lib", "couchbase_lite_cffi.dylib")
 
#chec if the library exists
if not os.path.exists(lib_path):
    raise FileNotFoundError(f"Library not found at {lib_path}. Please build the CFFI bindings first.")

lib = ffi.dlopen(lib_path)