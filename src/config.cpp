#include "config.h"

namespace ipmsg {

// Config::ConVarBaseMap Config::s_datas;
ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
    RWMutexType::ReadLock lock(GetMutex());
    /// typedef std::map<std::string, ConfigVarBase::ptr> ConVarBaseMap
    /// static ConVarBaseMap& GetDatas()
    auto it = GetDatas().find(name);
    /// 打印出logs
    // if(it != GetDatas().end()) std::cout << it->first << std::endl;
    return it == GetDatas().end() ? nullptr : it->second; /// 如果找到配置参数【system.port / system.value】,返回迭代器指针(s-data [key - string, value - ConfigVarBase::ptr]
}


///"A.B", 10
/// A:
///     B: 10
///     C: str
static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node> >& output) {

     /// @brief 如果前缀【配置参数】有非法字符 -- 不属于这段字串【abcdefghikjlmnopqrstuvwxyz._012345678】
     if(prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
            != std::string::npos) {
        LOG_ERROR(LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }

    /// @brief 如果没有非法字符,添加到list里面去,2次，第一次prefix : "" 第二次 prefix:"logs"
    output.push_back(std::make_pair(prefix, node));
    // std::cout << node << std::endl;
    /// @brief 如果是Map[Object] -- xx: , 遍历它
    if(node.IsMap()) {
        for(auto it = node.begin(); it != node.end(); ++it) {
            ListAllMember(prefix.empty() ? it->first.Scalar()
                    : prefix + "." + it->first.Scalar(), it->second, output);
        }
    }

}

void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    // std::cout << root << std::endl;
    ListAllMember("", root, all_nodes);

    for(auto& i : all_nodes) {
        std::string key = i.first;
        if(key.empty()) {
            continue;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key); /// 根据map查找key

        /// 如果找到配置参数
        if(var) {
            /// 如果不是纯量,可能是yml文件里面的相应位置有 【非空格】 键 例如 Tab
            if(i.second.IsScalar()){
                var->fromString(i.second.Scalar());
            }else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
    RWMutexType::ReadLock lock(GetMutex());
    /// typedef std::map<std::string, ConfigVarBase::ptr> ConVarBaseMap
    /// static ConVarBaseMap& GetDatas()
    ConVarBaseMap& m = GetDatas();
    for(auto it = m.begin(); it != m.end(); ++it) {
        cb(it->second);
    }
}



}
