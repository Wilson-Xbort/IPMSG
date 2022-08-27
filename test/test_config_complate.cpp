/**
 *  @date 2021-01-26
 *  @breief 成功完成解析
 */
#include "log.h"
#include "util.h"
#include "singleton.h"
#include "config.h"
#include <set>
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sys/syscall.h> // call for thread id
#include <yaml-cpp/yaml.h>

#if 1
/**
 *  配置系统的原则--约定优于配置：(可以减少配置量）
 *      g_int_value_config [system.port : 8080] 是一种约定，一般默认是在8080端口，不写出来也能够运行
 *      正常情况下是不需要进行修改的，配置是为了防止以后改变
 *      但有的时候需要进行变动，修改，这个时候可以通过配置去修改它
 *      g_float_value_config 就是一种配置
 */
ipmsg::ConfigVar<int>::ptr g_int_value_config =
    ipmsg::Config::Lookup("system.port", (int)8080, "system port");

ipmsg::ConfigVar<float>::ptr g_int_valuex_config =
    ipmsg::Config::Lookup("system.port", (float)111, "system port");

ipmsg::ConfigVar<float>::ptr g_float_value_config =
    ipmsg::Config::Lookup("system.value", (float)10.2f, "system value");

ipmsg::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config =
    ipmsg::Config::Lookup("system.int_vec", std::vector<int>{1, 2}, "system int vec");

ipmsg::ConfigVar<std::list<int> >::ptr g_int_list_value_config =
    ipmsg::Config::Lookup("system.int_list", std::list<int>{90, 30}, "system int list");

ipmsg::ConfigVar<std::set<int> >::ptr g_int_set_value_config =
    ipmsg::Config::Lookup("system.int_set", std::set<int>{5, 6, 7, 8}, "system int set");

ipmsg::ConfigVar<std::unordered_set<int> >::ptr g_int_uset_value_config =
    ipmsg::Config::Lookup("system.int_uset", std::unordered_set<int>{1, 7, 2, 9}, "system int unordered_set");

ipmsg::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config =
    ipmsg::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"k", 2}}, "system str-int map");

ipmsg::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_umap_value_config =
    ipmsg::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"k", 3}}, "system str-int umap");


void print_yaml(const YAML::Node& node, int level) {
    /**
     *      http://www.ruanyifeng.com/blog/2016/07/yaml.html
     *      YAML 可分为5类  [ Undefined, Null, Scalar, Sequence, Map]
     *      1. Undefined -- 未定义
     *      2. Null      -- 空值
     *      3. Scalar    -- 纯量 [字符串 布尔值 整数 浮点数 Null 时间 日期]
     *      4. Sequence  -- 序列 [数组 列表(List)]
     *      5. Map       -- 键值对 【 animal: pets 】
     */
    if(node.IsScalar()) {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
                    << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}
void test_yaml() {
    // YAML::Node node = YAML::LoadFile(name);
    /**
     *  @brief 通过LoadFile 方法把.yml文件加载进来，如果yml文件格式有问题则抛出异常
     */
    YAML::Node root = YAML::LoadFile("/home/vxe/IPMsg/log.yml");
    LOG_INFO(LOG_ROOT()) << root.size() << std::endl << std::endl;
    print_yaml(root, 0);
    // LOG_INFO(LOG_ROOT()) << root;
    // LOG_INFO(LOG_ROOT()) << root;
}

/**
 *   通过YAML文件加载配置，在解析的过程中查找相应的约定
 *
 */
void test_config() {
     LOG_INFO(LOG_ROOT()) << "before: " << g_int_value_config->getValue();
     LOG_INFO(LOG_ROOT()) << "before: " << g_float_value_config->toString();

#define XX(g_var, name, prefix) \
     { \
        auto v = g_var->getValue(); \
        for(auto& i : v) { \
            LOG_INFO(LOG_ROOT()) << #prefix " " #name ":" << i;  \
        } \
        LOG_INFO(LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
     }

#define XX_M(g_var, name, prefix) \
     { \
        auto v = g_var->getValue(); \
        for(auto& i : v) { \
            LOG_INFO(LOG_ROOT()) << #prefix " " #name ": {" \
                << i.first << " - " << i.second << "}"; \
        } \
        LOG_INFO(LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
     }


     XX(g_int_vec_value_config, "int_vec", "before");
     XX(g_int_list_value_config, "int_list", "before");
     XX(g_int_set_value_config, "int_set", "before");
     XX(g_int_uset_value_config, "int_uset", "before");
     XX_M(g_str_int_map_value_config, "str_int_map", "before");
     XX_M(g_str_int_umap_value_config, "str_int_umap", "before");


     YAML::Node root = YAML::LoadFile("/home/vxe/IPMsg/log.yml");
     //LOG_INFO(LOG_ROOT()) << root;
     ipmsg::Config::LoadFromYaml(root); /// fromString

     LOG_INFO(LOG_ROOT()) << "after: " << g_int_value_config->getValue();
     LOG_INFO(LOG_ROOT()) << "after: " << g_float_value_config->toString();

     XX(g_int_vec_value_config, "int_vec", "after");
     XX(g_int_list_value_config, "int_list", "after");
     XX(g_int_set_value_config, "int_set", "after");
     XX_M(g_str_int_map_value_config, "str_int_map", "after");
     XX_M(g_str_int_umap_value_config, "str_int_umap", "after");
     // XX(g_int_unordered_set_value_config, "int_unorederd_set", "after");
     // v = g_int_vec_value_config->getValue();
     // for(auto& i : v) {
     //    LOG_INFO(LOG_ROOT()) << "after: int vec" << i << std::endl;
     // }

}
#endif

class Person {
public:
    Person() = default;
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }

    bool operator==(const Person& ps) const {
        return m_name == ps.m_name
            && m_age == ps.m_age
            && m_sex == ps.m_sex;
    }

};

ipmsg::ConfigVar<Person>::ptr g_person =
    ipmsg::Config::Lookup("class.person", Person(), "class person");

/// 按人名查找
ipmsg::ConfigVar<std::map<std::string, Person> >::ptr g_person_map =
    ipmsg::Config::Lookup("class.map", std::map<std::string, Person>(), "class map person");

ipmsg::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
    ipmsg::Config::Lookup("class.vec_map", std::map<std::string, std::vector<Person> >(), "class map_vector person");


namespace ipmsg {
template<> /// 模板特化
class LexicalCast<std::string, Person> {
public:
    Person operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

/**
 *  @brief 对vector容器的偏特化，vector类型转换成string类型
 */
template<> /// 模板特化
class LexicalCast<Person, std::string> {
public:
    std::string operator()(const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}
void test_class() {
     LOG_INFO(LOG_ROOT()) << "before : " << g_person->getValue().toString() << " - "
        << g_person->toString();

#define XX_PM(g_var, prefix) \
     { \
        auto m = g_var->getValue(); \
        for(auto &i : m) { \
            LOG_INFO(LOG_ROOT()) << #prefix << ": " << i.first << " - " << i.second.toString(); \
        } \
        LOG_INFO(LOG_ROOT()) << #prefix << ": size=" << m.size(); \
     } \

     // g_person->addListener(10, [](const Person& old_value, const Person& new_value) {
     g_person->addListener([](const Person& old_value, const Person& new_value) {
        LOG_INFO(LOG_ROOT()) << "old_value = " << old_value.toString() << " new_value=" << new_value.toString();
     });


     XX_PM(g_person_map, "class.map before");
     LOG_INFO(LOG_ROOT()) << "before : " << g_person_vec_map->toString();


     YAML::Node root = YAML::LoadFile("/home/vxe/IPMsg/log.yml");
     //LOG_INFO(LOG_ROOT()) << root;
     ipmsg::Config::LoadFromYaml(root); /// fromString

     LOG_INFO(LOG_ROOT()) << "after : " << g_person->getValue().toString() << " - "
        << g_person->toString();

      XX_PM(g_person_map, "class.map after");
      LOG_INFO(LOG_ROOT()) << "after : " << g_person_vec_map->toString();

      g_person->clearListener();
}

void test_log() {
    static ipmsg::Logger::ptr system_log = LOG_NAME("system");
    LOG_INFO(system_log) << "hello system" << std::endl;
    std::cout << ipmsg::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/home/vxe/ipmsg/src/log.yml");
    // std::cout << root << std::endl;
    // std::cout << "====================" << std::endl;
    ipmsg::Config::LoadFromYaml(root); /// fromString
    std::cout << "====================" << std::endl;
    std::cout << ipmsg::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << root << std::endl;
    LOG_INFO(system_log) << "hello system" << std::endl;

    system_log->setFormatter("%d - %m%n");
    LOG_INFO(system_log) << "hello system" << std::endl;

    ipmsg::Config::Visit([](ipmsg::ConfigVarBase::ptr var) {
        LOG_INFO(LOG_ROOT()) << "name =" << var->getName()
            << " description = " << var->getDescription()
            << " typename =" << var->getTypeName()
            << " value =" << var->toString();
    });	
}




int main(int argc, char*argv[]) {
	// LOG_INFO(LOG_ROOT()) << "2021/1/11 Debug" << std::endl;
	// LOG_ERROR(LOG_ROOT()) << "2021/1/11 Error" << std::endl;

    // LOG_INFO(LOG_ROOT()) << g_int_value_config->getValue() << std::endl;
    // LOG_ERROR(LOG_ROOT()) << g_float_value_config->toString() << std::endl;
    // test_yaml();
    // test_config();
    // test_class();
    test_log();
	getchar();
}

