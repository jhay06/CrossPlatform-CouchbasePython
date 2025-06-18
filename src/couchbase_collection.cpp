#include "couchbase_collection.h"
 
#include "couchbase_lite.h" 
#include <fleece/Fleece.h>
#include <fleece/FLMutable.h> 
#include "json.hpp"
#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <future>
#include <regex>
std::string toCompactJsonString(const nlohmann::json& value) {
    // Step 1: Dump compact JSON string
    std::string jsonString = value.dump();  // compact by default

    // Step 2: Remove newlines and trim leading/trailing spaces
    jsonString.erase(std::remove(jsonString.begin(), jsonString.end(), '\n'), jsonString.end());

    // Trim leading/trailing whitespace
    auto start = jsonString.find_first_not_of(" \t\r");
    auto end = jsonString.find_last_not_of(" \t\r");

    if (start == std::string::npos || end == std::string::npos)
        return "";

    return jsonString.substr(start, end - start + 1);
}
std::vector<std::string> getKeys(const nlohmann::json& j) {
    std::vector<std::string> keys;
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            keys.push_back(it.key());
        }
    }
    return keys;
}

std::string generateUUID() {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 15);
    std::stringstream ss;

    ss << std::hex;
    for (int i = 0; i < 32; ++i) {
        ss << dist(rd);
    }

    // Optional: Format with hyphens like a standard UUID
    std::string raw = ss.str();
    return raw.substr(0, 8) + "-" + raw.substr(8, 4) + "-" + raw.substr(12, 4) + "-" +
           raw.substr(16, 4) + "-" + raw.substr(20, 12);
}

CouchBaseCollection::~CouchBaseCollection(){

    this->close();
    if(this->listenerToken){
        CBLListener_Remove(this->listenerToken);
        this->listenerToken = nullptr;
    }
    if(this->collection){
        CBLCollection_Release(this->collection);
        this->collection = nullptr;
    }
    this->couchbase = nullptr;
    this->collectionName.clear();
    this->scopeName.clear();
    this->useDefault = false;
    this->onChange = nullptr;
    this->onCollectionOpened = nullptr; 


}

void CouchBaseCollection::setCollection(const std::string collectionName)
{
    this->collectionName = collectionName;
}

void CouchBaseCollection::setScope(const std::string scope){
    this->scopeName = scope;
}

void CouchBaseCollection::setCouchbaseDB(CouchBaseLite *couchBase){

    this->couchbase = couchBase;
}

std::string CouchBaseCollection::getCollection()
{
    return this->collectionName;
}

 std::string CouchBaseCollection::getScope()
{
    return this->scopeName;
}

CouchBaseLite* CouchBaseCollection::getCouchbaseDB()
{
    return this->couchbase;
}


bool CouchBaseCollection::openOrCreate(){
    if(!this->couchbase){
        std::cerr << "No couchbase database connected" << std::endl;
        return false;
    }

    if(!this->couchbase->isConnected()){
        std::cerr << "Database not connected" << std::endl;
        return false;
    }
    if(!this->collectionName.length()){
        std::cerr << "Collection name not set" << std::endl;
        return false;
    }
    if(!this->scopeName.length()){
        std::cerr << "Scope name not set" << std::endl;
        return false;
    }
    if(!this->couchbase->getCouchBase()){
        std::cerr << "Couchbase database not initialized" << std::endl;
        return false;
    } 
    auto couchDB = this->couchbase->getCouchBase();
    if(this->useDefault){
        //override collection name
        CBLError error;
        this->collection=CBLDatabase_DefaultCollection(couchDB, &error);
        if(!this->collection){
            std::cerr << "Error getting default collection: " << error.domain << " : " << error.code << std::endl;
            FLSliceResult msg= CBLError_Message(&error);
            std::cerr << "Error message: " << (const char* ) msg.buf<< std::endl;
            if (msg.buf) {
                std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
            }
            FLSliceResult_Release(msg);
            return false;
        }
        if(this->onCollectionOpened){
            this->onCollectionOpened();
        }

        return true;

    }

    //check if collection exist
    bool exist = collectionExist();
    if(!exist){
        //create
        std::cout << "Collection does not exist, creating new collection: " << this->collectionName.c_str() << std::endl;
        createCollection();
        return true;
    }else{
        std::cout << "Collection already exists, opening collection: " << this->collectionName.c_str() << std::endl;
        return openCollection();

    }


    return false;

}


bool CouchBaseCollection::collectionExist(){
    CBLError error;
    auto couchDB = this->couchbase->getCouchBase();
    FLMutableArray scopes = CBLDatabase_ScopeNames(couchDB, &error);

    if(!scopes){
        return false;

    }
    //list all the scopes names
    //iterrate scopes using FLArrayIterator
    FLArrayIterator iter;
    FLArrayIterator_Begin(scopes, &iter);

    while(FLArrayIterator_Next(&iter)){

        FLSlice scopeName = FLValue_AsString(FLArrayIterator_GetValue(&iter));
        std::cout << "Scope: " << (const char*) scopeName.buf << std::endl;
        FLMutableArray collections = CBLDatabase_CollectionNames(couchDB,
                                                                 scopeName,&error
                                                                 );

        if(!collections){

            std::cerr << "Error getting collections for scope: " << (const char*) scopeName.buf << std::endl;
            std::cerr << "Error code: " << error.domain << " : " << error.code << std::endl;
            //print error message
            FLSliceResult msg= CBLError_Message(&error);
            std::cerr << "Error message: " << (const char*)msg.buf << std::endl;

            FLSliceResult_Release(msg);

        }else{
            FLArrayIterator collectionIter;
            FLArrayIterator_Begin(collections, &collectionIter);
            int i=0;

            while(i < FLArrayIterator_GetCount(&collectionIter)){
                auto col = FLValue_AsString(FLArrayIterator_GetValueAt(&collectionIter, i));
                i++;
                 
                if(col == FLStr(this->collectionName.c_str())){

                    return true;
                }
              
            }

        }

    }
    return false;

}


void CouchBaseCollection::createCollection(){
    CBLError error;
    auto couchDB = this->couchbase->getCouchBase();
    this->collection = CBLDatabase_CreateCollection(
        couchDB,
        FLStr(this->collectionName.c_str()),
        FLStr(this->scopeName.c_str()),
        &error
        ) ;
    if(!this->collection){
        std::cerr << "Error creating collection: " << error.domain << " : " << error.code << std::endl;
        FLSliceResult msg= CBLError_Message(&error);
        std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        if (msg.buf) {
            std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        }
        //release the message
        FLSliceResult_Release(msg);
        return;
    }
    this->listenerToken = CBLCollection_AddChangeListener(
        this->collection,
        [](void* context, const CBLCollectionChange* change){
            //check if context is instance of CouchbaseCollection
            CouchBaseCollection* self = static_cast<CouchBaseCollection*>(context);
           if(self->onChange){
                self->onChange();
            }
        },this
    );
    if(this->onCollectionOpened){
        this->onCollectionOpened();
    }


}



bool CouchBaseCollection::openCollection(){
    CBLError error;
    auto couchDB = this->couchbase->getCouchBase();

    this->collection = CBLDatabase_Collection(couchDB,
                                              FLStr(this->collectionName.c_str()),
                                              FLStr(this->scopeName.c_str()),
                                              &error);

    if(!this->collection){
        std::cerr << "Error opening collection: " << error.domain << " : " << error.code << std::endl;
        FLSliceResult msg= CBLError_Message(&error);
        std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        if (msg.buf) {
            std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
        }
        //release the message

        FLSliceResult_Release(msg);
        return false;
    }
    this->listenerToken = CBLCollection_AddChangeListener(
        this->collection,
        [](void* context, const CBLCollectionChange* change){
            //check if context is instance of CouchbaseCollection
            CouchBaseCollection* self = static_cast<CouchBaseCollection*>(context);
            if(self->onChange){
                self->onChange();
            }
        },this
        );

    if(this->onCollectionOpened){
        this->onCollectionOpened();
    }

    return true;

}



FLMutableArray createArrayObject(const nlohmann::json& val) {
    FLMutableArray mutableArray = FLMutableArray_New();
    FLError flerror;

    for (const auto& currentVal : val) {
        if (currentVal.is_array()) {
            FLMutableArray_AppendArray(mutableArray, createArrayObject(currentVal));
        } else if (currentVal.is_object()) {
            std::string jsonString = currentVal.dump();  // compact JSON string
            FLMutableDict dict = FLMutableDict_NewFromJSON(FLStr(jsonString.c_str()), &flerror);
            if (dict) {
                FLMutableArray_AppendDict(mutableArray, dict);
            } else {
                // Optionally handle parse error
            }
        } else if (currentVal.is_string()) {
            FLMutableArray_AppendString(mutableArray, FLStr(currentVal.get<std::string>().c_str()));
        } else if (currentVal.is_boolean()) {
            FLMutableArray_AppendBool(mutableArray, currentVal.get<bool>());
        } else if (currentVal.is_number_float()) {
            FLMutableArray_AppendDouble(mutableArray, currentVal.get<double>());
        } else if (currentVal.is_number_integer()) {
            FLMutableArray_AppendInt(mutableArray, currentVal.get<int64_t>());
        } else if (currentVal.is_number_unsigned()) {
            FLMutableArray_AppendUInt(mutableArray, currentVal.get<uint64_t>());
        } else if (currentVal.is_null()) {
            FLMutableArray_AppendNull(mutableArray);
        }
    }

    return mutableArray;
}





std::string CouchBaseCollection::saveDocument(nlohmann::json jsonDocument,std::string docId){
    if(!this->couchbase){
        std::cerr << "No couchbase database connected" << std::endl;
        return "";
    }

    if(!this->couchbase->isConnected()){
        std::cerr << "Database not connected" << std::endl;
        return "";
    }

    if(!this->collection){
        std::cerr << "No open collection" << std::endl;
        return ""; 
    }
    if(jsonDocument.is_null()){
        std::cerr << "Json document is null" << std::endl;
        return "";
    }

    std::string  oldDocId = docId;

    CBLError error;
      docId = this->scopeName + ":" + this->collectionName + "_" + generateUUID();
     if(!oldDocId.empty()){
        //generate a new UUID 
        docId = oldDocId;
     }
        

        CBLDocument* doc = CBLDocument_CreateWithID(FLStr(docId.c_str()));

        FLMutableDict docData = CBLDocument_MutableProperties(doc);
        //get the keys
        auto keys = getKeys(jsonDocument);
        for(const auto& key : keys){
            auto value = jsonDocument[key];
            
            if(value.is_array()){
                FLError flerror;
                FLMutableArray array = createArrayObject(value);
                FLMutableDict_SetArray(docData, FLStr(key.c_str()), array);
            }
            if(value.is_object()){
                std::string jsonString = toCompactJsonString(value);
                std::cout << "JSON String: " << jsonString << std::endl;
                //create a mutable dict from json string
                //using FLMutableDict_NewFromJSON
                FLError flerror;
                FLMutableDict data = FLMutableDict_NewFromJSON(FLStr(jsonString.c_str()), &flerror);
                FLMutableDict_SetDict(docData, FLStr(key.c_str()), data);
            }
            if(value.is_string()){
                FLMutableDict_SetString(docData, FLStr(key.c_str()), FLStr(value.get<std::string>().c_str()));
            }
            if(value.is_boolean()){
                FLMutableDict_SetBool(docData, FLStr(key.c_str()), value.get<bool>());
            }
            if(value.is_number_float() || value.is_number_integer() || value.is_number_unsigned()){
                FLMutableDict_SetDouble(docData, FLStr(key.c_str()), value.get<double>());
            }
            if(value.is_null()){
                FLMutableDict_SetNull(docData, FLStr(key.c_str()));

            }

        }



        auto saved = CBLCollection_SaveDocument(this->collection, doc,  &error);
        if(!saved){
            std::cerr << "Error saving document: " << error.domain << " : " << error.code << std::endl;
            FLSliceResult msg= CBLError_Message(&error); 
            std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
            if (msg.buf) {
                std::cerr << "Error message: " << (const char*)msg.buf << std::endl;
            }
            //release the message
            FLSliceResult_Release(msg);
            CBLDocument_Release(doc);
        }
            
        
        
        std::cout << "Document saved with ID: " << docId << std::endl;
        CBLDocument_Release(doc);
       
 
    //  CBLDocument_Release(doc);
    return docId;


}

void CouchBaseCollection::useDefaultCollection(bool useDefault){
    this->useDefault = useDefault;
}

bool CouchBaseCollection::isDefaultCollection(){
    return this->useDefault;

}

CBLCollection *CouchBaseCollection::getCollectionInstance(){
    return this->collection;
}

std::vector<nlohmann::json> CouchBaseCollection::query(std::string query, std::string filterKey, bool dontFilter) {
    if (query.find("${sc}") != std::string::npos) {
        query = std::regex_replace(query, std::regex("\\$\\{sc\\}"), this->scopeName + "." + this->collectionName);
    }

    std::string finalFilterKey = (!dontFilter && query.find("${sc}") != std::string::npos) ? this->collectionName : filterKey;

    if (!this->couchbase || !this->couchbase->isConnected()) {
        std::cerr << "No Couchbase database connected or database not open.\n";
        return {};
    }

    if (!this->collection) {
        std::cerr << "No open collection.\n";
        return {};
    }

    auto future = std::async(std::launch::async, [this, query, finalFilterKey, dontFilter]() -> std::vector<nlohmann::json> {
        std::vector<nlohmann::json> resultArray;

        int errorPos;
        CBLError error;
        auto couchDB = this->couchbase->getCouchBase();
        CBLQuery* q = CBLDatabase_CreateQuery(
            couchDB,
            kCBLN1QLLanguage,
            FLStr(query.c_str()),
            &errorPos,
            &error
        );

        if (!q) {
            std::cerr << "Error creating query: " << error.domain << " : " << error.code << "\n";
            FLSliceResult msg = CBLError_Message(&error);
            std::cerr << "Error message: " << std::string((const char*)msg.buf, msg.size) << "\n";
            FLSliceResult_Release(msg);
            return resultArray;
        }

        CBLResultSet* results = CBLQuery_Execute(q, &error);
        if (!results) {
            std::cerr << "Error executing query: " << error.domain << " : " << error.code << "\n";
            FLSliceResult msg = CBLError_Message(&error);
            std::cerr << "Error message: " << std::string((const char*)msg.buf, msg.size) << "\n";
            FLSliceResult_Release(msg);
            CBLQuery_Release(q);
            return resultArray;
        }

        while (CBLResultSet_Next(results)) {
            FLDict result = CBLResultSet_ResultDict(results);
            FLStringResult jsonResult = FLValue_ToJSON((FLValue)result);
            std::string jsonString((const char*)jsonResult.buf, jsonResult.size);
            FLSliceResult_Release(jsonResult);

            try {
                nlohmann::json parsed =nlohmann:: json::parse(jsonString);

                if (!finalFilterKey.empty() && !dontFilter) {
                    if (parsed.contains(finalFilterKey) && parsed[finalFilterKey].is_object()) {
                        resultArray.push_back(parsed[finalFilterKey]);
                    }
                } else {
                    resultArray.push_back(parsed);
                }

            } catch (const std::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << "\n";
            }
        }

        CBLResultSet_Release(results);
        CBLQuery_Release(q);
        return resultArray;
    });

    return future.get();
}

bool CouchBaseCollection::close() {
    if (this->listenerToken) {
        CBLListener_Remove(this->listenerToken);
        this->listenerToken = nullptr;
    }
    if (this->collection) {
        CBLCollection_Release(this->collection);
        this->collection = nullptr;
    }
    this->useDefault = false;
    this->collectionName.clear();
    this->scopeName.clear();
    this->couchbase = nullptr;
    return true;

}