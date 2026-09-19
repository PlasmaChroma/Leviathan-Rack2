#pragma once
inline void testP8Protocol() {
    auto parsed=sibyl::parseCompositionJson(R"({"schemaVersion":4,"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"length":16,"steps":[]}},"arrangement":[{"id":"s","tracks":{"v":"p"}}]})",1);
    for(const char* profile:{"receipt","summary","full"}) {
        SibylModule module;
        module.acceptComposition(parsed.composition,sibyl::ApplyAt::IMMEDIATE,sibyl::PhasePolicy::RESTART_ALL);
        processOneSample(module);
        std::string response,error;
        check(module.handleSibylRequest(SibylControl::Operation::CAPABILITIES,R"({"format":"manifest"})",response,error)
            && response.size()<300,"P8 native manifest stays below 300 bytes");
        json_error_t jsonError;
        json_t* manifest=json_loads(response.c_str(),0,&jsonError);
        const std::string fingerprint=json_string_value(json_object_get(manifest,"fingerprint"));
        json_decref(manifest);
        std::string request=std::string(R"({"expected_revision":1,"response_profile":")")+profile+R"(","operations":[{"op":"insert_note_batch","pattern_id":"p","encoding":"columns_v1","columns":["degree"],"rows":[[0],[4]],"onsets":{"start":0,"spacing":4,"count":4},"defaults":{"gate":0.5}}]})";
        check(module.handleSibylRequest(SibylControl::Operation::EDIT,request,response,error),"P8 native response profile accepts negotiated batch");
        json_t* result=json_loads(response.c_str(),0,&jsonError);
        check(json_integer_value(json_object_get(result,"appliedOperations"))==1
            && json_object_get(result,"pendingRevision") && json_object_get(result,"applyAt")
            && json_object_get(result,"affectedObjects"),"P8 receipt proves acceptance and adoption boundary");
        if(std::string(profile)=="receipt") {
            check(response.size()<=500 && response.find("noteIds")==std::string::npos,"P8 receipt meets 500-byte verification gate without event mappings");
            std::cout << "[P8 BYTES] receipt=" << response.size() << "\n";
        } else if(std::string(profile)=="summary")
            check(json_array_size(json_object_get(result,"operationSummaries"))==1,"P8 summary retains per-operation counts");
        else check(json_is_array(json_object_get(result,"changes")),"P8 full profile retains detailed reports");
        json_decref(result);
        module.handleSibylRequest(SibylControl::Operation::CAPABILITIES,R"({"format":"manifest"})",response,error);
        manifest=json_loads(response.c_str(),0,&jsonError);
        check(fingerprint==json_string_value(json_object_get(manifest,"fingerprint")),"P8 contract fingerprint is independent of score revision");
        json_decref(manifest);
        const auto* accepted=module.m_acceptedCompositionPtr;
        check(!module.handleSibylRequest(SibylControl::Operation::EDIT,R"({"expected_revision":2,"response_profile":"wrong","operations":[{"op":"set_meta","path":"title","value":"bad"}]})",response,error)
            && module.m_acceptedCompositionPtr==accepted,"P8 invalid profile rejects before mutation");
        check(module.handleSibylRequest(SibylControl::Operation::VALIDATE,R"({"expected_revision":2,"prepare":true,"operations":[{"op":"set_meta","path":"title","value":"prepared"}]})",response,error),"P8 prepare fixture");
        result=json_loads(response.c_str(),0,&jsonError);
        std::string handle=json_string_value(json_object_get(result,"handle"));json_decref(result);
        module.m_preparedTransactions.at(handle).expiresAt=std::chrono::steady_clock::now()-std::chrono::seconds(1);
        check(!module.handleSibylRequest(SibylControl::Operation::EDIT,"{\"handle\":\""+handle+"\"}",response,error)
            && module.m_acceptedCompositionPtr==accepted,"P8 expired handle rejects without mutation");
    }
    SibylModule a,b;
    check(a.m_capabilityInstance!=b.m_capabilityInstance,"P8 module instances have distinct cache identities");
    json_t* response=json_pack("{s:b,s:[s]}","ok",1,"warnings","material warning");
    sibyl::projectEditResponse(response,"receipt");
    check(json_array_size(json_object_get(response,"warnings"))==1,"P8 receipt preserves material warnings");
    json_decref(response);
}
