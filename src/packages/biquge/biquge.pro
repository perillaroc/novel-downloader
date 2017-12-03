include (../../../novel-downloader.pri)

TEMPLATE=aux

plugin_name = biquge

build_dir = $${build_packages_dir}/$${plugin_name}
build_dir~=s,/,\\,g

plugin_dir = $$PWD
plugin_dir~=s,/,\\,g

foo.commands = $$quote((IF NOT EXIST $${build_dir} (MKDIR $${build_dir})) && XCOPY $${plugin_dir} $${build_dir} /E /Y)$$escape_expand(\n\t))

QMAKE_EXTRA_TARGETS += foo
PRE_TARGETDEPS += foo

DISTFILES += \
    biquge/__init__.py \
    biquge/command.py \
    package.json