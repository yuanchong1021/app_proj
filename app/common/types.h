/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-05-07 16:08:57
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-05-07 16:09:18
 * @FilePath: \instrument\app\common\types.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file    types.h
 * @brief   公共基础类型定义（项目内全局通用）
 *
 * 该头文件不依赖任何其它项目内部头文件，可以被任意模块包含。
 * 项目里所有 C 源码统一通过此处定义的简写类型来描述位宽固定的整型。
 *
 * 设计约束：
 *   - 不要在此文件中加入业务相关的类型定义（如 ClusterSystem_t 等），
 *     业务上下文请放到 cluster_context.h 中。
 *   - 不要在此文件中包含模块对外接口声明，避免循环包含。
 */

#ifndef CLUSTER_COMMON_TYPES_H
#define CLUSTER_COMMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/*===========================================================================
 *  位宽固定的基础整型
 *  统一项目内整数类型命名风格，便于跨平台移植
 *===========================================================================*/
typedef uint8_t     u8;     /**< 无符号 8 位整型  */
typedef uint16_t    u16;    /**< 无符号 16 位整型 */
typedef uint32_t    u32;    /**< 无符号 32 位整型 */
typedef int8_t      s8;     /**< 有符号 8 位整型  */
typedef int16_t     s16;    /**< 有符号 16 位整型 */
typedef int32_t     s32;    /**< 有符号 32 位整型 */

/*===========================================================================
 *  通用返回码
 *===========================================================================*/
/**
 * @brief 函数返回码（用于 HAL / 服务层接口）
 */
typedef enum {
    RET_OK      = 0,    /**< 操作成功         */
    RET_ERROR   = 1,    /**< 一般错误         */
    RET_BUSY    = 2,    /**< 设备/资源忙      */
    RET_TIMEOUT = 3     /**< 超时             */
} RetCode_t;

/**
 * @brief 数据有效性标志
 *        约定：VALID=0，INVALID=1，与部分 CAN 信号 valid 位语义对齐
 */
typedef enum {
    VALID   = 0,        /**< 数据有效   */
    INVALID = 1         /**< 数据无效   */
} Validity_t;

/*===========================================================================
 *  NULL 兼容定义（部分编译器/标准头未提供）
 *===========================================================================*/
#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* CLUSTER_COMMON_TYPES_H */
