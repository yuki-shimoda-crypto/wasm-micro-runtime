/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_serialize_hook.h"
#include "../common/wasm_serialize.h"
#include "../common/wasm_exec_env.h"
#include "../common/wasm_runtime_common.h"

/* シリアライズフックが有効かどうかを示すフラグ */
static bool serialize_hook_enabled = false;

/* コールバック関数とユーザーデータ */
static wasm_runtime_serialize_callback_f serialize_callback = NULL;
static void *serialize_user_data = NULL;

/**
 * インタープリタフックの初期化
 *
 * @param exec_env 実行環境
 * 
 * @return 成功した場合はtrue、失敗した場合はfalse
 */
bool
wasm_serialize_init_hook(WASMExecEnv *exec_env)
{
    if (!exec_env)
        return false;

    serialize_hook_enabled = false;
    serialize_callback = NULL;
    serialize_user_data = NULL;
    return true;
}

/**
 * シリアライズフックの有効/無効を設定
 *
 * @param exec_env 実行環境
 * @param enable 有効化する場合はtrue
 */
void
wasm_serialize_enable_hook(WASMExecEnv *exec_env, bool enable)
{
    (void)exec_env;
    serialize_hook_enabled = enable;
}

/**
 * シリアライズコールバックを登録
 *
 * @param exec_env 実行環境
 * @param callback コールバック関数
 * @param user_data ユーザーデータ
 */
void
wasm_serialize_register_callback(WASMExecEnv *exec_env,
                               wasm_runtime_serialize_callback_f callback,
                               void *user_data)
{
    (void)exec_env;
    serialize_callback = callback;
    serialize_user_data = user_data;
}

/**
 * シリアライズをチェックするフック関数
 * この関数はインタープリタの主要ループ内から呼び出される
 *
 * @param exec_env 実行環境
 * 
 * @return シリアライズが実行された場合はtrue
 */
bool
wasm_serialize_check_hook(WASMExecEnv *exec_env)
{
    uint8_t *buffer;
    uint32_t buffer_size;
    uint32_t serialized_size;
    bool ret = false;

    if (!exec_env || !serialize_hook_enabled || !serialize_callback)
        return false;

    /* コールバックを呼び出してシリアライズが必要かチェック */
    if (!serialize_callback((wasm_exec_env_t)exec_env, serialize_user_data))
        return false;

    /* ここからシリアライズを実行 */
    
    /* 必要なバッファサイズを取得 */
    serialized_size = wasm_runtime_serialize_state((wasm_exec_env_t)exec_env, 
                                             NULL, 0);
    if (serialized_size == 0)
        return false;
        
    /* バッファを確保 */
    buffer = wasm_runtime_malloc(serialized_size);
    if (!buffer)
        return false;
        
    /* シリアライズを実行 */
    if (wasm_runtime_serialize_state((wasm_exec_env_t)exec_env, 
                                buffer, serialized_size) > 0) {
        /* シリアライズに成功 - ここで保存や転送の処理を行うこともできます */
        /* 最小実装では、成功のみを返します */
        ret = true;
    }
    
    /* バッファを解放 */
    wasm_runtime_free(buffer);
    
    return ret;
} 