TEMPLATE = subdirs

# 定义构建顺序（按依赖关系排列）
CONFIG += ordered

# 核心模块
SUBDIRS += \
    algorithm \
    util \
    protocol \
    transfer \
    common_component \
    core_plugin \
    app

# 模块依赖关系
protocol.depends = util
common_component.depends = algorithm util
transfer.depends = util common_component protocol
core_plugin.depends = util transfer common_component protocol
app.depends = algorithm util protocol transfer common_component core_plugin

# 全局配置
include(global.pri)
