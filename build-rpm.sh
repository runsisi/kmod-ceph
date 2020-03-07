#!/bin/sh -xe
# runsisi AT hust.edu.cn

kc_build_dir=''

trap '{ kc_cleanup; }' EXIT

# helpers

kc_prepare_khdrs() {
    (
        true
    )
}

kc_prepare_src() {
    kc_cleanup_src

    autoconf
    ./configure
    tar czf kmod-ceph.tar.gz kmod-ceph/

    cp find-requires.ksyms $kc_build_dir/SOURCES/
    cp kmod-ceph.tar.gz $kc_build_dir/SOURCES/
    cp kmod-ceph.spec $kc_build_dir/SPECS
}

kc_prepare_build_env() {
    kc_build_dir=$(mktemp -d --suffix .kmod-ceph)
    
    (cd $kc_build_dir && mkdir BUILD  BUILDROOT  RPMS  SOURCES  SPECS  SRPMS)
}

kc_cleanup_src() {
    git add -u
    git reset --hard
    git clean -df -e output
}

kc_cleanup_build_env() {
    rm -rf $kc_build_dir
}

# prepare -> build -> upload -> cleanup

kc_prepare() {
    kc_prepare_build_env
    kc_prepare_khdrs
    kc_prepare_src
}

kc_build_rpm() {
    DIST=$(rpm --eval %{?dist} | sed 's/.centos//')
    rpmbuild --define "_topdir $kc_build_dir" -ba --define "dist ${DIST}" kmod-ceph.spec
}

kc_upload_rpm() {
    rm -rf output/
    mkdir output/
    cp $kc_build_dir/RPMS/x86_64/*.rpm output/
}

kc_cleanup() {
    kc_cleanup_build_env
    kc_cleanup_src
}

# entry

main() {
    kc_prepare
    kc_build_rpm
    kc_upload_rpm
}

main
