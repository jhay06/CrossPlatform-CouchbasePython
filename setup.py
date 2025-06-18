from setuptools import setup, find_packages
import os
import shutil
import platform 
 
 



setup(
    name="CouchbaseLitePython",
    version="3.2.1",
    description="Couchbase Lite in CFFI bindings",
    author="Jhay Mendoza",
    author_email="jrockhackerz@example.com",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    package_data={
        "cbl": ["couchbase_lite_cffi.so", "libcblite.so"],
    },
    include_package_data=True,
    install_requires=[],
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: Android",
    ],
    zip_safe=False,
)

