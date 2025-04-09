/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_serialize.h"
#include "wasm_runtime_common.h"
#include <string.h>

/**
 * 実行状態をシリアライズ（スタブ実装）
 *
 * @param exec_env 実行環境
 * @param buffer 出力バッファ
 * @param buffer_size バッファサイズ
 * 
 * @return 成功した場合はシリアライズされたデータのサイズ、失敗した場合は0
 */
uint32_t
wasm_runtime_serialize_state(wasm_exec_env_t exec_env, 
                           uint8_t *buffer, 
                           uint32_t buffer_size)
{
    /* 最小実装: 常に成功を返すスタブ */
    /* 実際の実装では、メモリデータの取得とシリアライズを行います */
    
    if (!exec_env || (buffer && buffer_size == 0))
        return 0;
    
    /* バッファサイズの計算（ヘッダーのみ） */
    uint32_t total_size = sizeof(wasm_serialized_state_t);
    
    /* サイズのみを返す場合 */
    if (!buffer)
        return total_size;
    
    /* バッファが小さすぎる場合 */
    if (buffer_size < total_size)
        return 0;
    
    /* ヘッダーを設定 */
    wasm_serialized_state_t *header = (wasm_serialized_state_t*)buffer;
    header->magic = WASM_SERIALIZED_MAGIC;
    header->version = 1;
    header->total_size = total_size;
    header->memory_offset = sizeof(wasm_serialized_state_t);
    header->memory_size = 0; /* 今回は実際のメモリをシリアライズしません */
    
    return total_size;
}

/**
 * シリアライズされた状態からデシリアライズ（スタブ実装）
 *
 * @param buffer シリアライズされたデータ
 * @param buffer_size バッファサイズ
 * @param exec_env_out 復元された実行環境へのポインタを格納
 * 
 * @return 成功した場合はtrue
 */
bool
wasm_runtime_deserialize_state(const uint8_t *buffer, 
                             uint32_t buffer_size,
                             wasm_exec_env_t *exec_env_out)
{
    /* 最小実装: 常に成功を返すスタブ */
    /* 実際の実装では、メモリデータの復元を行います */
    
    wasm_serialized_state_t *header;
    
    if (!buffer || buffer_size < sizeof(wasm_serialized_state_t) || !exec_env_out || !*exec_env_out)
        return false;
    
    /* ヘッダーを取得 */
    header = (wasm_serialized_state_t*)buffer;
    
    /* マジックナンバーを確認 */
    if (header->magic != WASM_SERIALIZED_MAGIC)
        return false;
    
    /* バージョンを確認 */
    if (header->version != 1)
        return false;
    
    /* サイズを確認 */
    if (header->total_size > buffer_size)
        return false;
    
    return true;
}

/**
 * シリアライズコールバックを登録（スタブ実装）
 *
 * @param exec_env 実行環境
 * @param callback コールバック関数
 * @param user_data ユーザーデータ
 * 
 * @return 成功した場合はtrue
 */
bool
wasm_runtime_register_serialize_callback(wasm_exec_env_t exec_env, 
                                       wasm_runtime_serialize_callback_f callback,
                                       void *user_data)
{
    /* 最小実装: 常に成功を返すスタブ */
    (void)exec_env;
    (void)callback;
    (void)user_data;
    return true;
}

/**
 * シリアライズフックを有効/無効化（スタブ実装）
 *
 * @param exec_env 実行環境
 * @param enable 有効化する場合はtrue
 */
void
wasm_runtime_enable_serialize_hook(wasm_exec_env_t exec_env, bool enable)
{
    /* 最小実装: 何もしない */
    (void)exec_env;
    (void)enable;
} 