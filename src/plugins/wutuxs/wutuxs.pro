include (../../../novel-downloader.pri)

TEMPLATE=aux

plugin_name = wutuxs

build_dir = $${build_plugins_dir}/$${plugin_name}
build_dir~=s,/,\\,g

plugin_dir = $$PWD
plugin_dir~=s,/,\\,g

foo.commands = $$quote((IF NOT EXIST $${build_dir} (MKDIR $${build_dir})) && XCOPY $${plugin_dir} $${build_dir} /E /Y)$$escape_expand(\n\t))

QMAKE_EXTRA_TARGETS += foo
PRE_TARGETDEPS += foo

DISTFILES += \
    wutuxs/__init__.py \
    wutuxs/command.py \
    package.json
