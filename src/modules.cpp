#include <modules.hpp>

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <regex>
#include <stdexcept>

namespace fs = std::filesystem;
using std::queue;
using std::regex;
using std::vector;
using std::string;
using std::ifstream;
using std::unordered_map;

namespace module {

ModuleInfo parse_module(const fs::path& file_name){
    ModuleInfo mod;
    ifstream file(file_name.string());
    regex parse_regex(R"(\b(import|export\s+module)\s+([a-zA-Z_][\w\.]*)\s*;)");
    mod.file = file_name;

    if(!file){
        throw std::runtime_error("Couldnt open module file");
    }

    string line;
    while(std::getline(file, line)){
        std::smatch match;
        if(std::regex_search(line, match, parse_regex)){
            // match[1] == import | export module
            // match[2] == mod.name
            if(match[1] == "import"){
                mod.imports.push_back(match[2]);
            }else if(match[1] == "export module"){
                mod.name = match[2];
            }
        }
    }

    return mod;
}

// Kahn sort
vector<ModuleInfo> topology_sort(const vector<ModuleInfo>& mods){
    unordered_map<string, ModuleInfo> modMap;
    unordered_map<string, int> indegree;
    unordered_map<string, vector<string>> graph;
    queue<string> ready;
    vector<ModuleInfo> res;
    // map from module name to ModuleInfo
    for(const auto& mod : mods){
        modMap[mod.name] = mod;
        indegree[mod.name] += 0;
    }
    // build graph: edge count and adjust
    for(const auto& mod : mods){
        for(const auto& included : mod.imports){
            graph[included].push_back(mod.name);
            indegree[mod.name]++;
        }
    }
    // queue for mods that dont have incoming edge
    for(const auto& [name, degree] : indegree){
        if(degree == 0){
            ready.push(name);
        }
    }

    while(!ready.empty()){
        string curr = ready.front();
        ready.pop();

        res.push_back(modMap[curr]);

        for(const auto& dep : graph[curr]){
            indegree[dep]--;
            if(indegree[dep] == 0){
                ready.push(dep);
            }
        }
    }

    if(res.size() != mods.size()){
        throw std::runtime_error("Cycle detected in module import");
    }

    return res;
}

}