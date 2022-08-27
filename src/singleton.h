#pragma once
/**
 * @file singleton.h
 * @brief 单例模式封装
 * @author ipmsg.yin
 * @email 564628276@qq.com
 * @date 2019-05-18
 * @copyright Copyright (c) 2019年 ipmsg.yin All rights reserved (www.ipmsg.top)
 */
#ifndef _SINGLETON_H__
#define _SINGLETON_H__

namespace ipmsg {

/**
 * @brief 单例模式封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */
template<class T, class X = void, int N = 0>
class Singleton {
public:
    /**
     * @brief 返回单例裸指针
     */
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

/**
 * @brief 单例模式智能指针封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */
template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
    /**
     * @brief 返回单例智能指针
     */
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};


}


#endif
