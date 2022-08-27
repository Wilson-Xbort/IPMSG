#pragma once
/**
 * @file config.h
 * @brief 配置模块
 * @author ipmsg.yin
 * @email 564628276@qq.com
 * @date 2019-05-22
 * @copyright Copyright (c) 2019年 ipmsg.yin All rights reserved (www.ipmsg.top)
 */

#ifndef _CONFIG_H__
#define _CONFIG_H__
#include <memory>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <functional> /// 回调函数
#include <string>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream> /// ostream 用于operator<< 重载
#include "log.h"
#include "util.h"

namespace ipmsg {

/**
 * @brief 配置变量的基类
 */
class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    /**
     * @brief 构造函数
     * @param[in] name 配置参数名称[0-9a-z_.]
     * @param[in] description 配置参数描述
     */
    ConfigVarBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
        // std::cout << "Enter ConfigVarBase - " << name << " - " << description << std::endl;
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower); /// 转换成小写
    }

    /**
     * @brief 析构函数
     */
    virtual ~ConfigVarBase() {}

    /**
     * @brief 返回配置参数名称
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief 返回配置参数的描述
     */
    const std::string& getDescription() const { return m_description; }

    /**
     * @brief 转成字符串
     */
    virtual std::string toString() = 0;

    /**
     * @brief 从字符串初始化值
     */
    virtual bool fromString(const std::string& val) = 0;

    virtual std::string getTypeName() const = 0;

private:
    // 配置参数的名称
    std::string m_name;
    // 配置参数的描述
    std::string m_description;
};

/**
 *  @brief 类型转换模板类(F 源类型, T 目标类型)
 *         F - FromType
 *         T - ToType
 *         FromType 转换成 ToType
 */
template<class F, class T>
class LexicalCast {
public:
    /**
     * @brief 类型转换
     * @param[in] v 源类型值
     * @return 返回v转换后的目标类型
     * @exception 当类型不可转换时抛出异常
     */
    T operator() (const F& v) {
        // std::cout << "base_partical_template" << std::endl;
        return boost::lexical_cast<T>(v);
    }
};

/**
 *  @brief 对vector容器的偏特化，String类型转换成vector类型
 */
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::vector<T> vec;
        for(auto i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/**
 *  @brief 对vector容器的偏特化，vector类型转换成string类型
 */
template<class T>
class LexicalCast<std::vector<T>, std::string > {
public:

    std::string operator() (const std::vector<T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }

        ss << node;
        return ss.str();
    }
};


/**
 *  @brief 对list容器的偏特化，String类型转换成list类型
 */
template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::list<T> vec;
        for(auto i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/**
 *  @brief 对list容器的偏特化，list类型转换成string类型
 */
template<class T>
class LexicalCast<std::list<T>, std::string > {
public:

    std::string operator() (const std::list<T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }

        ss << node;
        return ss.str();
    }
};


/**
 *  @brief 对set容器的偏特化，String类型转换成set类型
 */
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::set<T> vec;
        for(auto i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/**
 *  @brief 对set容器的偏特化，set类型转换成string类型
 */
template<class T>
class LexicalCast<std::set<T>, std::string > {
public:

    std::string operator() (const std::set<T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }

        ss << node;
        return ss.str();
    }
};

/**
 *  @brief 对unordered_set容器的偏特化，String类型转换成unordered_set类型
 */
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_set<T> vec;
        for(auto i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/**
 *  @brief 对unordered_set容器的偏特化，unordered_set类型转换成string类型
 */
template<class T>
class LexicalCast<std::unordered_set<T>, std::string > {
public:

    std::string operator() (const std::unordered_set<T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }

        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::map<std::string, T> vec;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }

        return vec;
    }
};

/**
 *  @brief 对unordered_set容器的偏特化，unordered_set类型转换成string类型
 */
template<class T>
class LexicalCast<std::map<std::string, T>, std::string > {
public:

    std::string operator() (const std::map<std::string, T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }

        ss << node;
        return ss.str();
    }
};


template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v) {
        // std::cout << "LexicalCast partical template<>: String To Vector" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_map<std::string, T> vec;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }

        return vec;
    }
};

/**
 *  @brief 对unordered_set容器的偏特化，unordered_set类型转换成string类型
 */
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string > {
public:

    std::string operator() (const std::unordered_map<std::string, T> & v) {
        // std::cout << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << " - " << v << std::endl;
        std::stringstream ss;
        // ss << "LexicalCast partical template<>: Vector To String" << __FILE__ << " - " << __LINE__ << std::endl;
        YAML::Node node;
        for(auto &i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }

        ss << node;
        return ss.str();
    }
};



/**
 * @brief 配置参数模板子类,保存对应类型的参数值
 * @details T 参数的具体类型
 *          FromStr 从std::string转换成T类型的仿函数
 *          ToStr 从T转换成std::string的仿函数
 *          std::string 为YAML格式的字符串
 *
 * @details class FromStr = LexicalCast<std::string, T> 模板特例化
 *          class ToStr = LexicalCast<T, std::string>   模板特例化
 */
template<class T, class FromStr = LexicalCast<std::string, T>,
                   class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr; // [T=int] 不是类型名
    typedef RWMutex RWMutexType;
    /**
     * @brief 更改配置时调用回调函数,通知系统
     * @param[in] old_value 原值
     * @param[in] new_value 新值
     */
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_back;

    /**
     * @brief 通过参数名,参数值,描述构造ConfigVar
     * @param[in] name 参数名称有效字符为[0-9a-z_.]
     * @param[in] default_value 参数的默认值
     * @param[in] description 参数的描述
     */
    ConfigVar(const std::string& name,
        const T& default_value,
        const std::string& description = "")
        :ConfigVarBase(name, description)
        ,m_val(default_value)
    {
        // std::cout << "Enter ConfigVar - " << name << " - " << description << " - " << std::endl;
    }

    /**
     * @brief 将参数值转换成YAML String
     * @exception 当转换失败抛出异常
     */
    std::string toString() override {
        try {
            // return boost::lexical_cast<std::string>(m_val);
            RWMutexType::ReadLock lock(m_mutex);
            /**
             *  ToStr std::string operator() (const T&)
             *  但是基础类型转换可以交给LexicalCast 来做
             */
            return ToStr()(m_val);
        }
        catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::toString() exception" << e.what()
                << " convert : " << typeid(m_val).name() << " to string ";
        }

        return "";
    }



    bool fromString(const std::string& val) override{
        try {
            // m_val = boost::lexical_cast<T>(val);
            /**
             *  FromStr T operator() (const std::string)
             *  但是基础类型转换可以交给LexicalCast 来做
             */
            setValue(FromStr()(val));
            // std::cout << type
        }
        catch (std::exception& e) {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::fromString() exception" << e.what()
                << " convert : string to " << typeid(m_val).name()
                << " - " << val;
        }
        return false;
    }

    const T getValue() {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }

    void setValue(const T& v) {
        {
            RWMutexType::ReadLock lock(m_mutex);
            if(v == m_val) {  /// 需要在main.cpp 中的Person类下重载== 操作符
                return;
            }
            for(auto& i : m_cbs) {
                i.second(m_val, v); /// 调用回调函数【oldvalue, newvalue】
            }
        } /// 出了域释放（析构）ReadLock
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }

    std::string getTypeName() const override { return typeid(T).name(); }

    /**
     * @brief 添加变化回调函数
     * @return 返回该回调函数对应的唯一id,用于删除回调
     */
    // void addListener(uint64_t key, on_change_back ocb) {
    uint64_t addListener(on_change_back ocb) {
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = ocb;
        return s_fun_id;
    }

    /**
     * @brief 删除回调函数
     * @param[in] key 回调函数的唯一id
     */
    void delListener(uint64_t key) {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }

    /**
     * @brief 获取回调函数
     * @param[in] key 回调函数的唯一id
     * @return 如果存在返回对应的回调函数,否则返回nullptr
     */
    on_change_back getListener(uint64_t key) {
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key); // return iterator
        return it == m_cbs.end() ? nullptr : it->second;
    }


    /**
     * @brief 清理所有的回调函数
     */
    void clearListener() {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }

private:
    T m_val;
    RWMutexType m_mutex;
    /**
     *  @brief 变更回调函数组, uint64_t key,要求唯一，一般可以用hash值
     *  @detail 回调函数【functional】没法判断是否是同一个funcion（）
     *      使用Map判断,当Key相同的时候可以删除
     *
     *  functional 没有比较函数，所以我们使用map用key删除
     */
    std::map<uint64_t, on_change_back> m_cbs;
};

/**
 * @brief ConfigVar的管理类
 * @details 提供便捷的方法创建/访问ConfigVar
 */
class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConVarBaseMap;
    typedef RWMutex RWMutexType;
    /**
    * @brief 获取/创建对应参数名的配置参数
    * @param[in] name 配置参数名称
    * @param[in] default_value 参数默认值
    * @param[in] description 参数描述
    * @details 获取参数名为name的配置参数,如果存在直接返回
    *          如果不存在,创建参数配置并用default_value赋值
    * @return 返回对应的配置参数,如果参数名存在但是类型不匹配则返回nullptr
    * @exception 如果参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
    */
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description = "") {
        /// 如果读到了那直接返回, 没读到创建后返回
        RWMutexType::WriteLock lock(GetMutex());
        auto it = GetDatas().find(name);
        /// 如果查找成功
        if(it != GetDatas().end()) {
            /** 基类指针转子类指针， it : ConfigVarBase::ptr(基类指针) **/
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            /** 如果转换成功**/
            if(tmp) {
                LOG_INFO(LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            }else {
                LOG_ERROR(LOG_ROOT()) << "Lookup name=" << name << " exists but types not "
                    <<  typeid(T).name() << " real type=" << it->second->getTypeName()
                    << " " << it->second->toString(); /// 调用ConfigVar::ToString() -> 模板类LexicalCast<std::shared_ptr<ipmsg::ConfigVarBase>, String>
                return nullptr;
            }
        }

        /**
         *  @func  find_first_not_of()
         *  @brief 返回在字符串中首次出现的不匹配str中的任何一个字符的首字符索引,
         *         从index开始搜索, 如果全部匹配则返回string::npos。
         *  past : "abcdefghikjlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789"
         */
        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._0123456789") /// key不再需要小写，强转为大写 line 37
            != std::string::npos) {
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }

    /**
     * @brief 查找配置参数
     * @param[in] name 配置参数名称
     * @return 返回配置参数名为name的配置参数
     */
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    /**
     * @brief 使用YAML::Node初始化配置模块
     */
    static void LoadFromYaml(const YAML::Node& root);

    /**
     * @brief 查找配置参数,返回配置参数的基类
     * @param[in] name 配置参数名称
     */
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    /**
     * @brief 遍历配置模块里面所有配置项
     * @param[in] cb 配置项回调函数
     */
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    /**
     * @brief 返回所有的配置项
     * @details ConVarBaseMap : typedef std::map<std::string, ConfigVarBase::ptr> ConVarBaseMap
     *          sdatas : 是个Map
     */
     // static ConVarBaseMap s_datas;
    static ConVarBaseMap& GetDatas() {
        static ConVarBaseMap s_datas;
        return s_datas;
    }

    /** 类在全局创建全局变量(配置 例如： g_log_defines)，全局变量初始化没有严格的顺序，如果不是静态方法而是静态成员，
     *          如果初始化的顺序比它要执行的方法/创建的成员对象晚，有可能锁还没构建成功，出现内存错误
     */
    static RWMutexType& GetMutex() {
        static RWMutexType s_mutex;
        return s_mutex;
    }

};


}


#endif // !_CONFIG_H__
