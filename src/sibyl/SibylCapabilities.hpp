#pragma once
#include <jansson.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace sibyl {
inline std::string newCapabilityInstance() {
    static std::atomic<uint64_t> counter{0};
    return std::to_string(std::chrono::system_clock::now().time_since_epoch().count())
        + ":" + std::to_string(++counter);
}
inline std::string capabilityResponse(const std::string& full, const std::string& request,
                                      const std::string& instance) {
    json_error_t error;
    json_t* root=json_loads(full.c_str(),0,&error);
    json_t* query=json_loads(request.c_str(),0,&error);
    json_t* caps=json_object_get(json_object_get(root,"capabilities"),"sibyl");
    json_t* p8=json_pack("{s:i,s:[s,s,s],s:b,s:b,s:b,s:b}","version",1,
        "responseProfiles","receipt","summary","full","batchOnsets",1,"sparseOverrides",1,"columnarReads",1,"manifest",1);
    json_object_set_new(caps,"p8",p8);
    json_t* contract=json_deep_copy(caps);
    json_object_del(contract,"revision");
    char* canonical=json_dumps(contract,JSON_COMPACT|JSON_SORT_KEYS);
    uint64_t hash=1469598103934665603ULL;
    for(const unsigned char* c=reinterpret_cast<const unsigned char*>(canonical);c && *c;++c) hash=(hash^*c)*1099511628211ULL;
    free(canonical);json_decref(contract);
    const std::string fingerprint=std::to_string(hash);
    json_t* output=root;
    const char* format=json_string_value(json_object_get(query,"format"));
    const char* topic=json_string_value(json_object_get(query,"topic"));
    if(!json_is_object(query) || (json_object_get(query,"format") && (!format || (std::string(format)!="manifest" && std::string(format)!="full")))
            || (json_object_get(query,"topic") && (!topic || !json_object_get(caps,topic)))) {
        output=json_pack("{s:b,s:{s:s,s:s}}","ok",0,"error","code","invalid_request","message","Invalid capability format or topic");
    } else if(topic) {
        output=json_pack("{s:b,s:s,s:O}","ok",1,"topic",topic,"contract",json_object_get(caps,topic));
    } else if(format && std::string(format)=="manifest") {
        output=json_pack("{s:b,s:O,s:O,s:O,s:{s:i,s:b}}","ok",1,
            "apiVersion",json_object_get(caps,"apiVersion"),"schemaVersion",json_object_get(caps,"schemaVersion"),
            "revision",json_object_get(caps,"revision"),"features","p8",1,"preparedTransactions",1);
    }
    json_object_set_new(output,"instance",json_string(instance.c_str()));
    json_object_set_new(output,"fingerprint",json_string(fingerprint.c_str()));
    char* encoded=json_dumps(output,JSON_COMPACT);
    std::string result=encoded ? encoded : "{}";
    free(encoded); if(output!=root)json_decref(output);json_decref(root);json_decref(query);
    return result;
}
} // namespace sibyl
