/**
 * @file    sm4_ctr.h
 * @author  RUST HUEY
 * @brief   SM4 分组密码和 CTR 模式公共 API。
 * @version 1.00
 * @date    2026-04-29
 *
 * @copyright Copyright (c) 2026
 */
#ifndef __SM4_CTR_H__
#define __SM4_CTR_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SM4_BLOCK_SIZE 16U
#define SM4_KEY_SIZE   16U

/**
 * @brief SM4 API 状态码类型。
 */
typedef int8_t sm4_status_t;

#define SM4_OK        ((sm4_status_t)0)
#define SM4_ERR_PARAM ((sm4_status_t)-1)

/**
 * @brief SM4 扩展密钥上下文。
 */
typedef struct sm4_context {
    uint32_t rk[32]; /**< SM4 加密轮密钥。 */
} sm4_context_t;

/**
 * @brief 将 128 位 SM4 密钥扩展为加密轮密钥。
 *
 * @param[out] ctx 待初始化的 SM4 上下文。
 * @param[in]  key 16 字节用户密钥。
 * @return 成功返回 SM4_OK；ctx 或 key 为 NULL 时返回 SM4_ERR_PARAM。
 */
sm4_status_t sm4_set_encrypt_key(sm4_context_t *ctx,
                                 const uint8_t *key);

/**
 * @brief 使用 SM4 CTR 模式处理数据。
 *
 * 加密和解密都调用此函数。input 和 output 可以指向同一块缓冲区，
 * 以支持原地加解密。
 *
 * @param[in]  ctx           已初始化的 SM4 加密上下文。
 * @param[in]  nonce_counter 16 字节初始计数器分组。
 * @param[in]  input         输入缓冲区。
 * @param[out] output        输出缓冲区。
 * @param[in]  length        需要处理的字节数。
 * @return 成功返回 SM4_OK；必要参数为 NULL 时返回 SM4_ERR_PARAM。
 */
sm4_status_t sm4_ctr_crypt(const sm4_context_t *ctx,
                           const uint8_t       *nonce_counter,
                           const uint8_t       *input,
                           uint8_t             *output,
                           size_t               length);

#ifdef __cplusplus
}
#endif

#endif /* __SM4_CTR_H__ */
