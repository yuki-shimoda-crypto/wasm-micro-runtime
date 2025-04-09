/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _WASM_SERIALIZE_H
#define _WASM_SERIALIZE_H

#include "wasm_runtime_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* シリアライズステートのマジックナンバー */
#define WASM_SERIALIZED_MAGIC 0x726D6177  /* "wamr" (逆順) */

/* シリアライズAPIの戻り値 */
typedef enum {
    WASM_SERIALIZE_OK,
    WASM_SERIALIZE_ERROR,
    WASM_SERIALIZE_BUFFER_TOO_SMALL,
} wasm_serialize_result_t;

/* シリアライズ状態の構造 */
typedef struct wasm_serialized_state_t {
    uint32_t magic;       /* WASM_SERIALIZED_MAGIC */
    uint32_t version;     /* バージョン、現在は1 */
    uint32_t total_size;  /* 全体のサイズ */
    
    /* メモリセクション */
    uint32_t memory_offset;
    uint32_t memory_size;
    
    /* その他の状態（将来の拡張用） */
} wasm_serialized_state_t;

/* 
 * Note: コールバック関数の型定義は wasm_export.h にあるため、
 * 重複定義を避けるためこちらでは定義しません
 */

#ifdef __cplusplus
}
#endif

#endif /* _WASM_SERIALIZE_H */ 