import os 
from cffi import FFI
import platform 
ffi = FFI()
def detect_arch():
    machine = platform.machine().lower()
    if "aarch64" in machine or "arm64" in machine:
        return "arm64-v8a"
    elif "arm" in machine:
        return "armeabi-v7a"
    elif "x86_64" in machine:
        return "x86_64"
    else:
        raise RuntimeError(f"Unsupported architecture: {machine}")
    
platform_arch = detect_arch()

lib_path = os.path.join(os.path.dirname(__file__), "libs", platform_arch, "couchbase_lite_cffi.so")
 
#chec if the library exists
if not os.path.exists(lib_path):
    raise FileNotFoundError(f"Library not found at {lib_path}. Please build the CFFI bindings first.")



lib = ffi.dlopen(lib_path)