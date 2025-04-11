/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_serialize.h"
#include "wasm_runtime_common.h"
#include "../interpreter/wasm_serialize_hook.h"
#include <string.h>

/**
 * 実行状態をシリアライズ
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
    WASMModuleInstanceCommon *module_inst;
    uint32_t memory_size = 0, total_size;
    wasm_serialized_state_t *header;
    wasm_memory_inst_t memory;

    if (!exec_env || (buffer && buffer_size == 0))
        return 0;

    module_inst = wasm_runtime_get_module_inst(exec_env);
    if (!module_inst)
        return 0;

    /* メモリサイズの取得 */
    memory = wasm_runtime_get_memory(module_inst, 0);
    if (memory) {
        uint32 num_bytes_per_page = wasm_memory_get_bytes_per_page(memory);
        uint32 cur_page_count = wasm_memory_get_cur_page_count(memory);
        memory_size = num_bytes_per_page * cur_page_count;
    }
    
    /* 全体のサイズを計算（ヘッダー + メモリデータ） */
    total_size = sizeof(wasm_serialized_state_t) + memory_size;

    /* サイズのみを返す場合 */
    if (!buffer)
        return total_size;

    /* バッファが小さすぎる場合 */
    if (buffer_size < total_size)
        return 0;

    /* ヘッダーを設定 */
    header = (wasm_serialized_state_t*)buffer;
    header->magic = WASM_SERIALIZED_MAGIC;
    header->version = 1;
    header->total_size = total_size;
    header->memory_offset = sizeof(wasm_serialized_state_t);
    header->memory_size = memory_size;

    /* メモリデータをコピー */
    if (memory_size > 0) {
        void *buffer_ptr = wasm_runtime_addr_app_to_native(module_inst, 0);
        if (!buffer_ptr)
            return 0;
        memcpy(buffer + header->memory_offset, buffer_ptr, memory_size);
    }

    return total_size;
}

/**
 * シリアライズされた状態からデシリアライズ
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
    wasm_serialized_state_t *header;
    WASMModuleInstanceCommon *module_inst;
    wasm_memory_inst_t memory;

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

    /* モジュールインスタンスを取得 */
    module_inst = wasm_runtime_get_module_inst(*exec_env_out);
    if (!module_inst)
        return false;

    /* メモリデータを復元 */
    if (header->memory_size > 0) {
        memory = wasm_runtime_get_memory(module_inst, 0);
        if (!memory)
            return false;
            
        void *native_ptr = wasm_runtime_addr_app_to_native(module_inst, 0);
        if (!native_ptr)
            return false;
            
        /* メモリサイズの確認 */
        uint32 num_bytes_per_page = wasm_memory_get_bytes_per_page(memory);
        uint32 cur_page_count = wasm_memory_get_cur_page_count(memory);
        uint32 current_memory_size = num_bytes_per_page * cur_page_count;
        
        if (current_memory_size < header->memory_size)
            return false;
            
        memcpy(native_ptr, buffer + header->memory_offset, header->memory_size);
    }

    return true;
}

/**
 * シリアライズコールバックを登録
 *
 * @param exec_env 実行環境
 * @param callback コールバック関数
 * @param user_data コールバック用ユーザーデータ
 *
 * @return 成功した場合はtrue
 */
bool
wasm_runtime_register_serialize_callback(wasm_exec_env_t exec_env,
                                       wasm_runtime_serialize_callback_f callback,
                                       void *user_data)
{
    if (!exec_env || !callback)
        return false;

    /* フック関数にコールバックを登録 */
    wasm_serialize_register_callback(exec_env, callback, user_data);
    return true;
}

/**
 * シリアライズフックを有効/無効化
 *
 * @param exec_env 実行環境
 * @param enable 有効化する場合はtrue
 */
void
wasm_runtime_enable_serialize_hook(wasm_exec_env_t exec_env, bool enable)
{
    if (exec_env) {
        /* フックを有効/無効化する */
        wasm_serialize_enable_hook(exec_env, enable);
    }
}

/**
 * シリアライズに必要なバッファサイズを取得
 *
 * @param exec_env 実行環境インスタンス
 *
 * @return シリアライズに必要なバイト数、失敗した場合は0
 */
uint32_t
wasm_runtime_get_serialize_buffer_size(wasm_exec_env_t exec_env)
{
    /* NULLバッファでシリアライズを呼び出すとサイズのみを計算する */
    return wasm_runtime_serialize_state(exec_env, NULL, 0);
} 