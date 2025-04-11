/*
 * Copyright (C) 2023 Intel Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _WASM_SERIALIZE_FILE_H
#define _WASM_SERIALIZE_FILE_H

#include "wasm_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 実行環境をファイルにシリアライズします
 *
 * @param exec_env 実行環境
 * @param file_name 保存先のファイル名
 * @return true 成功した場合
 * @return false 失敗した場合
 */
bool
wasm_runtime_serialize_to_file(wasm_exec_env_t exec_env, const char *file_name);

/**
 * @brief ファイルから実行環境をデシリアライズします
 *
 * @param file_name 読み込むファイル名
 * @param error_buf エラーメッセージバッファ
 * @param error_buf_size エラーメッセージバッファのサイズ
 * @return wasm_exec_env_t 復元された実行環境、失敗した場合はNULL
 */
wasm_exec_env_t
wasm_runtime_deserialize_from_file(const char *file_name, char *error_buf,
                                   uint32_t error_buf_size);

#ifdef __cplusplus
}
#endif

#endif /* end of _WASM_SERIALIZE_FILE_H */ 