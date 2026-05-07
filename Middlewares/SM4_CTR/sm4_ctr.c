 /**
 * @file    sm4_ctr.c
 * @author  Haoyu Yu
 * @brief   SM4 分组密码和 CTR 模式实现。
 * @version 1.00
 * @date    2026-04-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "sm4_ctr.h"

#include <string.h>

/**
 * @brief SM4 标准 S 盒。
 */
static const uint8_t sm4_sbox[256] = {
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7,
    0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
    0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a,
    0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95,
    0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
    0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b,
    0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2,
    0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
    0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5,
    0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55,
    0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
    0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f,
    0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f,
    0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
    0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e,
    0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20,
    0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
};

/**
 * @brief SM4 密钥扩展系统参数 FK。
 */
static const uint32_t sm4_fk[4] = {
    0xa3b1bac6U, 0x56aa3350U, 0x677d9197U, 0xb27022dcU
};

/**
 * @brief SM4 密钥扩展固定参数 CK。
 */
static const uint32_t sm4_ck[32] = {
    0x00070e15U, 0x1c232a31U, 0x383f464dU, 0x545b6269U,
    0x70777e85U, 0x8c939aa1U, 0xa8afb6bdU, 0xc4cbd2d9U,
    0xe0e7eef5U, 0xfc030a11U, 0x181f262dU, 0x343b4249U,
    0x50575e65U, 0x6c737a81U, 0x888f969dU, 0xa4abb2b9U,
    0xc0c7ced5U, 0xdce3eaf1U, 0xf8ff060dU, 0x141b2229U,
    0x30373e45U, 0x4c535a61U, 0x686f767dU, 0x848b9299U,
    0xa0a7aeb5U, 0xbcc3cad1U, 0xd8dfe6edU, 0xf4fb0209U,
    0x10171e25U, 0x2c333a41U, 0x484f565dU, 0x646b7279U
};

/**
 * @brief 将 32 位数循环左移。
 *
 * @param[in] value    待循环左移的数值。
 * @param[in] bits     左移位数，范围为 1..31。
 * @return 循环左移后的数值。
 */
static uint32_t rotl32(uint32_t value, unsigned int bits)
{
    return (value << bits) | (value >> (32U - bits));
}

/**
 * @brief 读取一个大端序 32 位字。
 *
 * @param[in] input    4 字节输入缓冲区。
 * @return 读取到的 32 位字。
 */
static uint32_t load_be32(const uint8_t *input)
{
    if (NULL == input) {
        return 0U;
    }

    return ((uint32_t)input[0] << 24) |
           ((uint32_t)input[1] << 16) |
           ((uint32_t)input[2] << 8)  |
           (uint32_t)input[3];
}

/**
 * @brief 按大端序写入一个 32 位字。
 *
 * @param[in]  value   待写入的 32 位字。
 * @param[out] output  4 字节输出缓冲区。
 */
static void store_be32(uint32_t value, uint8_t *output)
{
    if (NULL == output) {
        return;
    }

    output[0] = (uint8_t)(value >> 24);
    output[1] = (uint8_t)(value >> 16);
    output[2] = (uint8_t)(value >> 8);
    output[3] = (uint8_t)value;
}

/**
 * @brief 执行 SM4 非线性字节替换。
 *
 * @param[in] input    32 位输入字。
 * @return 替换后的 32 位字。
 */
static uint32_t sm4_tau(uint32_t input)
{
    uint8_t bytes[4];

    store_be32(input, bytes);
    bytes[0] = sm4_sbox[bytes[0]];
    bytes[1] = sm4_sbox[bytes[1]];
    bytes[2] = sm4_sbox[bytes[2]];
    bytes[3] = sm4_sbox[bytes[3]];

    return load_be32(bytes);
}

/**
 * @brief 执行 SM4 加密线性变换。
 *
 * @param[in] input    32 位输入字。
 * @return 变换后的 32 位字。
 */
static uint32_t sm4_l(uint32_t input)
{
    return input ^ rotl32(input, 2) ^ rotl32(input, 10) ^
           rotl32(input, 18) ^ rotl32(input, 24);
}

/**
 * @brief 执行 SM4 密钥扩展线性变换。
 *
 * @param[in] input    32 位输入字。
 * @return 变换后的 32 位字。
 */
static uint32_t sm4_l_prime(uint32_t input)
{
    return input ^ rotl32(input, 13) ^ rotl32(input, 23);
}

/**
 * @brief 执行 SM4 加密轮变换。
 *
 * @param[in] input    32 位输入字。
 * @return 变换后的 32 位字。
 */
static uint32_t sm4_t(uint32_t input)
{
    return sm4_l(sm4_tau(input));
}

/**
 * @brief 执行 SM4 密钥扩展轮变换。
 *
 * @param[in] input    32 位输入字。
 * @return 变换后的 32 位字。
 */
static uint32_t sm4_t_prime(uint32_t input)
{
    return sm4_l_prime(sm4_tau(input));
}

/**
 * @brief 将 CTR 计数器按 128 位大端整数递增。
 *
 * @param[in,out] counter 原地更新的 16 字节计数器。
 */
static void ctr_inc(uint8_t *counter)
{
    uint8_t i;

    if (NULL == counter) {
        return;
    }

    for (i = SM4_BLOCK_SIZE; 0U < i; i--) {
        counter[i - 1U]++;
        if (0U != counter[i - 1U]) {
            break;
        }
    }
}

/**
 * @brief 使用 SM4 加密一个 16 字节分组。
 *
 * @param[in]  ctx     已初始化的 SM4 上下文。
 * @param[in]  input   16 字节明文分组。
 * @param[out] output  16 字节密文分组。
 * @return 成功返回 SM4_OK；任一参数为空时返回 SM4_ERR_PARAM。
 */
static sm4_status_t sm4_encrypt_block(const sm4_context_t *ctx,
                                      const uint8_t       *input,
                                      uint8_t             *output)
{
    uint32_t x[36];
    uint8_t  i;

    if ((NULL == ctx) || (NULL == input) || (NULL == output)) {
        return SM4_ERR_PARAM;
    }

    x[0] = load_be32(input);
    x[1] = load_be32(input + 4);
    x[2] = load_be32(input + 8);
    x[3] = load_be32(input + 12);

    for (i = 0U; 32U > i; i++) {
        x[i + 4U] = x[i] ^ sm4_t(x[i + 1U] ^
                                 x[i + 2U] ^
                                 x[i + 3U] ^
                                 ctx->rk[i]);
    }

    store_be32(x[35], output);
    store_be32(x[34], output + 4);
    store_be32(x[33], output + 8);
    store_be32(x[32], output + 12);

    return SM4_OK;
}

/**
 * @brief 将 128 位 SM4 密钥扩展为加密轮密钥。
 *
 * @param[out] ctx     待初始化的 SM4 上下文。
 * @param[in]  key     16 字节用户密钥。
 * @return 成功返回 SM4_OK；ctx 或 key 为空时返回 SM4_ERR_PARAM。
 */
sm4_status_t sm4_set_encrypt_key(sm4_context_t *ctx,
                                 const uint8_t *key)
{
    uint32_t k[36];
    uint8_t  i;

    if ((NULL == ctx) || (NULL == key)) {
        return SM4_ERR_PARAM;
    }

    k[0] = load_be32(key)      ^ sm4_fk[0];
    k[1] = load_be32(key + 4)  ^ sm4_fk[1];
    k[2] = load_be32(key + 8)  ^ sm4_fk[2];
    k[3] = load_be32(key + 12) ^ sm4_fk[3];

    for (i = 0U; 32U > i; i++) {
        k[i + 4U] = k[i] ^ sm4_t_prime(k[i + 1U] ^
                                       k[i + 2U] ^
                                       k[i + 3U] ^
                                       sm4_ck[i]);
        ctx->rk[i] = k[i + 4U];
    }

    return SM4_OK;
}

/**
 * @brief 使用 SM4 CTR 模式处理数据。
 *
 * @param[in]  ctx           已初始化的 SM4 加密上下文。
 * @param[in]  nonce_counter 16 字节初始计数器分组。
 * @param[in]  input         输入缓冲区。
 * @param[out] output        输出缓冲区。
 * @param[in]  length        需要处理的字节数。
 * @return 成功返回 SM4_OK；必要参数为空时返回 SM4_ERR_PARAM。
 */
sm4_status_t sm4_ctr_crypt(const sm4_context_t *ctx,
                           const uint8_t       *nonce_counter,
                           const uint8_t       *input,
                           uint8_t             *output,
                           size_t               length)
{
    uint8_t counter[SM4_BLOCK_SIZE];
    uint8_t stream[SM4_BLOCK_SIZE];
    uint8_t block_len;
    uint8_t i;

    if ((NULL == ctx)           || 
        (NULL == nonce_counter) ||
        ((0U < length)          && 
        ((NULL == input)        || 
        (NULL == output)))) 
    {
        return SM4_ERR_PARAM;
    }

    memcpy(counter, nonce_counter, sizeof(counter));

    while (0U < length) {
        if (SM4_OK != sm4_encrypt_block(ctx, counter, stream)) {
            return SM4_ERR_PARAM;
        }

        block_len = SM4_BLOCK_SIZE > length ?
                    (uint8_t)length :
                    SM4_BLOCK_SIZE;
        for (i = 0U; block_len > i; i++) {
            output[i] = input[i] ^ stream[i];
        }

        input  += block_len;
        output += block_len;
        length -= block_len;
        ctr_inc(counter);
    }

    return SM4_OK;
}
