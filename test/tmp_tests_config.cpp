/**
 *  @date 2021-01-26
 *  @breief 成功完成解析
 */
#include "log.h"
#include "util.h"
#include "singleton.h"
#include "config.h"
#include <iostream>
#include <sys/syscall.h> // call for thread id
#include <yaml-cpp/yaml.h>

ipmsg::ConfigVar<int>::ptr g_int_value_config =
    ipmsg::Config::Lookup("system.port", (int)8080, "system port");

ipmsg::ConfigVar<float>::ptr g_float_value_config =
    ipmsg::Config::Lookup("system.value", (float)10.2f, "system value");

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
    YAML::Node root = YAML::LoadFile("/home/vxe/ipmsg/src/log.yml");
    LOG_INFO(LOG_ROOT()) << root.size() << std::endl << std::endl;
    print_yaml(root, 0);
    // LOG_INFO(LOG_ROOT()) << root;
    // LOG_INFO(LOG_ROOT()) << root;
}

void test_config() {
     LOG_INFO(LOG_ROOT()) << "before: " << g_int_value_config->getValue();
     LOG_INFO(LOG_ROOT()) << "before: " << g_float_value_config->toString();

     YAML::Node root = YAML::LoadFile("/home/vxe/ipmsg/src/log.yml");
     //LOG_INFO(LOG_ROOT()) << root;
     ipmsg::Config::LoadFromYaml(root);

     LOG_INFO(LOG_ROOT()) << "after: " << g_int_value_config->getValue();
     LOG_INFO(LOG_ROOT()) << "after: " << g_float_value_config->toString();

}


int main(int argc, char*argv[]) {
	// LOG_INFO(LOG_ROOT()) << "2021/1/11 Debug" << std::endl;
	// LOG_ERROR(LOG_ROOT()) << "2021/1/11 Error" << std::endl;

    // LOG_INFO(LOG_ROOT()) << g_int_value_config->getValue() << std::endl;
    // LOG_ERROR(LOG_ROOT()) << g_float_value_config->toString() << std::endl;
    // test_yaml();
    test_config();
	getchar();
}

