/*
 * Copyright (C) 2023 Intel Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "wasm_serialize_file.h"
#include "wasm_serialize.h"
#include "wasm_runtime_common.h"
#include <stdio.h>

/**
 * 実行環境をファイルにシリアライズします
 */
bool
wasm_runtime_serialize_to_file(wasm_exec_env_t exec_env, const char *file_name)
{
    uint8_t *buffer = NULL;
    uint32_t size = 0;
    size_t written;
    FILE *file;
    bool ret = false;

    bh_assert(exec_env != NULL);
    bh_assert(file_name != NULL);

    /* シリアライズに必要なサイズを取得 */
    size = wasm_runtime_get_serialize_buffer_size(exec_env);
    if (size == 0) {
        LOG_ERROR("シリアライズバッファサイズの取得に失敗しました");
        return false;
    }

    /* バッファを確保 */
    buffer = wasm_runtime_malloc(size);
    if (!buffer) {
        LOG_ERROR("シリアライズバッファのメモリ確保に失敗しました（%u バイト）", size);
        return false;
    }

    /* シリアライズ実行 */
    if (!wasm_runtime_serialize_state(exec_env, buffer, size)) {
        LOG_ERROR("シリアライズに失敗しました");
        wasm_runtime_free(buffer);
        return false;
    }

    /* ファイルをオープン */
    file = fopen(file_name, "wb");
    if (!file) {
        LOG_ERROR("ファイル '%s' をオープンできませんでした", file_name);
        goto cleanup;
    }

    /* データを書き込み */
    written = fwrite(buffer, 1, size, file);
    if (written != size) {
        LOG_ERROR("ファイルへの書き込みに失敗しました: %zu / %u バイト", 
                  written, size);
        goto cleanup_file;
    }

    ret = true;

cleanup_file:
    fclose(file);
cleanup:
    wasm_runtime_free(buffer);
    return ret;
}

/**
 * ファイルから実行環境をデシリアライズします
 */
wasm_exec_env_t
wasm_runtime_deserialize_from_file(const char *file_name, char *error_buf,
                                   uint32_t error_buf_size)
{
    uint8_t *buffer = NULL;
    uint32_t size = 0;
    FILE *file = NULL;
    wasm_exec_env_t exec_env = NULL;
    long file_size;

    bh_assert(file_name != NULL);

    /* ファイルをオープン */
    file = fopen(file_name, "rb");
    if (!file) {
        if (error_buf)
            snprintf(error_buf, error_buf_size, 
                     "ファイル '%s' をオープンできませんでした", file_name);
        return NULL;
    }

    /* ファイルサイズを取得 */
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0) {
        if (error_buf)
            snprintf(error_buf, error_buf_size, "ファイルが空です");
        goto cleanup_file;
    }

    size = (uint32_t)file_size;

    /* バッファを確保 */
    buffer = wasm_runtime_malloc(size);
    if (!buffer) {
        if (error_buf)
            snprintf(error_buf, error_buf_size, 
                     "メモリの確保に失敗しました: %u バイト", size);
        goto cleanup_file;
    }

    /* データを読み込み */
    if (fread(buffer, 1, size, file) != size) {
        if (error_buf)
            snprintf(error_buf, error_buf_size, "ファイルの読み込みに失敗しました");
        goto cleanup_buffer;
    }

    /* デシリアライズ */
    if (!wasm_runtime_deserialize_state(buffer, size, &exec_env)) {
        if (error_buf)
            snprintf(error_buf, error_buf_size, "デシリアライズに失敗しました");
    }

cleanup_buffer:
    wasm_runtime_free(buffer);
cleanup_file:
    fclose(file);
    return exec_env;
} 