TEMPLATE = subdirs

SUBDIRS += \
    core_plugins_manager \
    core_plugins

# 插件依赖管理器
core_plugins.depends = core_plugins_manager
