include (../../../novel-downloader.pri)

TEMPLATE=aux

plugin_name = yunlaige

build_dir = $${build_packages_dir}/$${plugin_name}
win32 {
build_dir~=s,/,\\,g
}

plugin_dir = $$PWD
win32 {
plugin_dir~=s,/,\\,g
}

win32 {
foo.commands = $$quote((IF NOT EXIST $${build_dir} (MKDIR $${build_dir})) && XCOPY $${plugin_dir} $${build_dir} /E /Y)$$escape_expand(\n\t))
}

unix {
foo.commands = $$quote((test -d $${build_dir} || mkdir -p $${build_dir}) && cp -R $${plugin_dir} $${build_packages_dir}$$escape_expand(\n\t))
}

QMAKE_EXTRA_TARGETS += foo
PRE_TARGETDEPS += foo

DISTFILES += \
    yunlaige/__init__.py \
    yunlaige/command.py \
    package.json
