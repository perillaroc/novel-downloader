include (../../../novel-downloader.pri)

TEMPLATE=aux

build_dir = $${build_plugins_dir}/epub_maker
build_dir~=s,/,\\,g

plugin_dir = $$PWD\\epub_maker
plugin_dir~=s,/,\\,g

foo.commands = $$quote((IF NOT EXIST $${build_dir} (MKDIR $${build_dir})) && XCOPY $${plugin_dir} $${build_dir} /E /Y)$$escape_expand(\n\t))

QMAKE_EXTRA_TARGETS += foo
PRE_TARGETDEPS += foo

DISTFILES += \
    epub_maker/__init__.py \
    epub_maker/epub_maker.py
