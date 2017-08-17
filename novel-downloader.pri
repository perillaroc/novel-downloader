source_base_dir = $$PWD

sub_dir = $$_PRO_FILE_PWD_
sub_dir ~= s,^$$re_escape($$PWD),,
build_base_dir = $$clean_path($$OUT_PWD)
build_base_dir ~= s,$$re_escape($$sub_dir)$,,

build_dist_dir = $$build_base_dir/dist

build_bin_dir = $$build_dist_dir/bin
build_lib_dir = $$build_dist_dir/lib
build_plugins_dir = $$build_dist_dir/plugins
build_vendor_dir = $$build_dist_dir/vendor
