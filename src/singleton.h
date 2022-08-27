#pragma once
/**
 * @file singleton.h
 * @brief ����ģʽ��װ
 * @author ipmsg.yin
 * @email 564628276@qq.com
 * @date 2019-05-18
 * @copyright Copyright (c) 2019�� ipmsg.yin All rights reserved (www.ipmsg.top)
 */
#ifndef _SINGLETON_H__
#define _SINGLETON_H__

namespace ipmsg {

/**
 * @brief ����ģʽ��װ��
 * @details T ����
 *          X Ϊ�˴�����ʵ����Ӧ��Tag
 *          N ͬһ��Tag������ʵ������
 */
template<class T, class X = void, int N = 0>
class Singleton {
public:
    /**
     * @brief ���ص�����ָ��
     */
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

/**
 * @brief ����ģʽ����ָ���װ��
 * @details T ����
 *          X Ϊ�˴�����ʵ����Ӧ��Tag
 *          N ͬһ��Tag������ʵ������
 */
template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
    /**
     * @brief ���ص�������ָ��
     */
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};


}


#endif
