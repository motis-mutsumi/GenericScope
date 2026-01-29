#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""分析 ai.txt 中的成功请求格式"""

import json

# 读取 ai.txt
with open(r'd:\app\App_GeneriScope\GenericScope\ai.txt', 'r', encoding='utf-8') as f:
    data = json.load(f)

print("=" * 80)
print("AI.TXT 完整请求格式分析")
print("=" * 80)

# 1. 顶层字段
print("\n【1】顶层字段列表:")
for key in data.keys():
    value = data[key]
    value_type = type(value).__name__
    if isinstance(value, list):
        print(f"  - {key}: {value_type} (长度={len(value)})")
    elif isinstance(value, dict):
        print(f"  - {key}: {value_type} (键={list(value.keys())})")
    else:
        print(f"  - {key}: {value_type} = {value}")

# 2. system 字段详情
print("\n【2】system 字段 (系统提示):")
if 'system' in data and len(data['system']) > 0:
    print(f"  类型: {type(data['system']).__name__}")
    print(f"  数量: {len(data['system'])}")
    print(f"  第一项结构:")
    first_system = data['system'][0]
    print(f"    类型: {type(first_system)}")
    if isinstance(first_system, dict):
        print(f"    键: {list(first_system.keys())}")
        for k, v in first_system.items():
            if isinstance(v, str):
                print(f"    {k}: {v[:100]}..." if len(v) > 100 else f"    {k}: {v}")
    elif isinstance(first_system, str):
        print(f"    内容预览: {first_system[:200]}...")

# 3. tools 字段详情
print("\n【3】tools 字段:")
if 'tools' in data:
    print(f"  数量: {len(data['tools'])}")

# 4. thinking 字段详情
print("\n【4】thinking 字段:")
if 'thinking' in data:
    print(f"  内容: {data['thinking']}")

# 5. stream 字段
print("\n【5】stream 字段:")
if 'stream' in data:
    print(f"  值: {data['stream']}")

# 6. metadata 字段
print("\n【6】metadata 字段:")
if 'metadata' in data:
    print(f"  内容: {data['metadata']}")

# 7. messages 结构
print("\n【7】messages 字段:")
print(f"  数量: {len(data['messages'])}")
if len(data['messages']) > 0:
    print(f"  第一条消息:")
    msg = data['messages'][0]
    print(f"    role: {msg['role']}")
    print(f"    content类型: {type(msg['content']).__name__}")

# 8. 生成建议的代码
print("\n" + "=" * 80)
print("【建议】当前代码缺少的字段:")
print("=" * 80)
current_fields = {'model', 'max_tokens', 'temperature', 'messages'}
aitxt_fields = set(data.keys())
missing_fields = aitxt_fields - current_fields
print(f"缺少字段: {missing_fields}")

print("\n【重要】temperature 值:")
print(f"  ai.txt 中: {data.get('temperature')} (null)")
print(f"  当前代码: 1.0")
print(f"  建议: 不设置 temperature 字段（让API使用默认值）")
