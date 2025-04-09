/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _WASM_SERIALIZE_HOOK_H
#define _WASM_SERIALIZE_HOOK_H

#include "wasm.h"
#include "../common/wasm_exec_env.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * インタープリタフックの初期化
 *
 * @param exec_env 実行環境
 * 
 * @return 成功した場合はtrue、失敗した場合はfalse
 */
bool
wasm_serialize_init_hook(WASMExecEnv *exec_env);

/**
 * シリアライズフックの有効/無効を設定
 *
 * @param exec_env 実行環境
 * @param enable 有効化する場合はtrue
 */
void
wasm_serialize_enable_hook(WASMExecEnv *exec_env, bool enable);

/**
 * シリアライズをチェックするフック関数
 * この関数はインタープリタの主要ループ内から呼び出される
 *
 * @param exec_env 実行環境
 * 
 * @return シリアライズが実行された場合はtrue
 */
bool
wasm_serialize_check_hook(WASMExecEnv *exec_env);

#ifdef __cplusplus
}
#endif

#endif /* _WASM_SERIALIZE_HOOK_H */ 