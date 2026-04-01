# 文档总览（Docs Index）

本目录文档较多，先按用途分为 4 类：

## 1) 规范与设计（Specs）
- [protocol-config-system-spec.md](./protocol-config-system-spec.md)  
  协议配置系统完整设计规格（核心总规范）
- [monitor-panel-tech-spec.md](./monitor-panel-tech-spec.md)  
  监控面板技术方案与实现细节

## 2) 开发与集成指南（Guides）
- [user-manual.md](./user-manual.md)
- [plugin-development-guide.md](./plugin-development-guide.md)
- [ai-protocol-generation-guide.md](./ai-protocol-generation-guide.md)
- [ai-protocol-cli-guide.md](./ai-protocol-cli-guide.md)
- [scopeuart-integration-guide.md](./scopeuart-integration-guide.md)
- [scopeuart-protocol-integration-example.md](./scopeuart-protocol-integration-example.md)
- [protocol-config-new-features-guide.md](./protocol-config-new-features-guide.md)

## 3) 评审与修复记录（Reviews & Fixes）
- [protocol-code-review-2026-01-28.md](./protocol-code-review-2026-01-28.md)
- [fixes/README.md](./fixes/README.md)

## 4) 任务文档（Tasks）
- [tasks/README.md](./tasks/README.md)
- [tasks/INDEX.md](./tasks/INDEX.md)（新增：按任务状态整理）

---

## 建议使用方式
- 想看“系统应该怎么做”：先看 `Specs`
- 想看“具体怎么接入”：看 `Guides`
- 想看“改了什么、为什么改”：看 `Reviews & Fixes`
- 想看“任务进展和交付”：看 `Tasks`

## 文档维护约定（建议）
- 新增设计文档：放 `docs/` 根目录（规格/指南）
- 新增修复记录：放 `docs/fixes/`
- 新增任务追踪：放 `docs/tasks/`
- 新文档命名：`topic-purpose-YYYY-MM-DD.md` 或 `TASK-xxx-主题.md`
