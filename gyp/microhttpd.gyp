# Copyright 2016 Google Inc.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'variables': {
    'skia_warnings_as_errors': 0,
  },
  'targets': [{
    'target_name': 'microhttpd',
    'type': 'static_library',

    'direct_dependent_settings': {
      'include_dirs': [ '../third_party/externals/microhttpd/src/include' ]
    },
    'include_dirs': [
      '../third_party/externals/microhttpd/src/include',
      '../third_party/libmicrohttpd',
    ],
    'sources': [
        '../third_party/externals/microhttpd/src/microhttpd/base64.c',
        '../third_party/externals/microhttpd/src/microhttpd/basicauth.c',
        '../third_party/externals/microhttpd/src/microhttpd/connection.c',
        '../third_party/externals/microhttpd/src/microhttpd/daemon.c',
        '../third_party/externals/microhttpd/src/microhttpd/digestauth.c',
        '../third_party/externals/microhttpd/src/microhttpd/internal.c',
        '../third_party/externals/microhttpd/src/microhttpd/md5.c',
        '../third_party/externals/microhttpd/src/microhttpd/memorypool.c',
        '../third_party/externals/microhttpd/src/microhttpd/postprocessor.c',
        '../third_party/externals/microhttpd/src/microhttpd/reason_phrase.c',
        '../third_party/externals/microhttpd/src/microhttpd/response.c',
        '../third_party/externals/microhttpd/src/microhttpd/tsearch.c',
    ],
    'conditions': [
      # For each platform, run configure and scrape the generated MHD_config.h
      # to get a list of platform specific defines
      # the options to use for configure are:
      # --disable-doc --disable-exapmles --enable-https=no --disable-curl
      # --enable-spdy=no --enable-shared=no
      ['skia_os == "linux"', {
        'cflags': [ '-w' ],
        'libraries': [
            '-lpthread',
        ],
        'direct_dependent_settings': {
            'libraries': [ '-lpthread' ],
        },
        'defines=': [             # equals sign throws away most Skia defines (just noise)
            "_GNU_SOURCE=1",
            "BAUTH_SUPPORT=1",
            "DAUTH_SUPPORT=1",
            "EPOLL_SUPPORT=1",
            "HAVE_ACCEPT4=1",
            "HAVE_ARPA_INET_H=1",
            "HAVE_CLOCK_GETTIME=1",
            "HAVE_DECL_SOCK_NONBLOCK=1",
            "HAVE_DECL_TCP_CORK=1",
            "HAVE_DECL_TCP_NOPUSH=0",
            "HAVE_DLFCN_H=1",
            "HAVE_EPOLL_CREATE1=1",
            "HAVE_ERRNO_H=1",
            "HAVE_FCNTL_H=1",
            "HAVE_FSEEKO=1",
            "HAVE_GCRYPT_H=1",
            "HAVE_INET6=1",
            "HAVE_INTTYPES_H=1",
            "HAVE_LIMITS_H=1",
            "HAVE_LISTEN_SHUTDOWN=1",
            "HAVE_LOCALE_H=1",
            "HAVE_MATH_H=1",
            "HAVE_MEMMEM=1",
            "HAVE_MEMORY_H=1",
            "HAVE_MESSAGES=1",
            "HAVE_NETDB_H=1",
            "HAVE_NETINET_IN_H=1",
            "HAVE_NETINET_TCP_H=1",
            "HAVE_POLL=1",
            "HAVE_POLL_H=1",
            "HAVE_POSTPROCESSOR=1",
            "HAVE_PTHREAD_H=1",
            "HAVE_PTHREAD_PRIO_INHERIT=1",
            "HAVE_PTHREAD_SETNAME_NP=1",
            "HAVE_SEARCH_H=1",
            "HAVE_SOCK_NONBLOCK=1",
            "HAVE_STDINT_H=1",
            "HAVE_STDIO_H=1",
            "HAVE_STDLIB_H=1",
            "HAVE_STRINGS_H=1",
            "HAVE_STRING_H=1",
            "HAVE_SYS_MMAN_H=1",
            "HAVE_SYS_MSG_H=1",
            "HAVE_SYS_SELECT_H=1",
            "HAVE_SYS_SOCKET_H=1",
            "HAVE_SYS_STAT_H=1",
            "HAVE_SYS_TIME_H=1",
            "HAVE_SYS_TYPES_H=1",
            "HAVE_TIME_H=1",
            "HAVE_UNISTD_H=1",
            "HTTPS_SUPPORT=0",
            "LINUX=1",
            'LT_OBJDIR=".libs/"',
            "MHD_USE_POSIX_THREADS=1",
            'PACKAGE="libmicrohttpd"',
            'PACKAGE_BUGREPORT="libmicrohttpd@gnu.org"',
            'PACKAGE_NAME="libmicrohttp"',
            'PACKAGE_STRING="libmicrohttpd 0.9.42"',
            'PACKAGE_TARNAME="libmicrohttpd"',
            'PACKAGE_URL=""',
            'PACKAGE_VERSION="0.9.42"',
            "SPDY_SUPPORT=0",
            "STDC_HEADERS=1",
            'VERSION="0.9.42"',
            '_MHD_EXTERN=__attribute__((visibility("default"))) extern',
        ],
      }],
      ['skia_os == "mac"', {
        'cflags': [ '-w' ],
        'libraries': [
            '-lpthread',
        ],
        'defines=': [             # equals sign throws away most Skia defines (just noise)
            "_GNU_SOURCE=1",
            "BAUTH_SUPPORT=1",
            "DAUTH_SUPPORT=1",
            "EPOLL_SUPPORT=0",
            "HAVE_ARPA_INET_H=1",
            "HAVE_CLOCK_GETTIME=1",
            "HAVE_DECL_SOCK_NONBLOCK=0",
            "HAVE_DECL_TCP_CORK=0",
            "HAVE_DECL_TCP_NOPUSH=1",
            "HAVE_DLFCN_H=1",
            "HAVE_ERRNO_H=1",
            "HAVE_FCNTL_H=1",
            "HAVE_FSEEKO=1",
            "HAVE_INET6=1",
            "HAVE_INTTYPES_H=1",
            "HAVE_LIMITS_H=1",
            "HAVE_LOCALE_H=1",
            "HAVE_MATH_H=1",
            "HAVE_MEMMEM=1",
            "HAVE_MEMORY_H=1",
            "HAVE_MESSAGES=1",
            "HAVE_NETDB_H=1",
            "HAVE_NETINET_IN_H=1",
            "HAVE_NETINET_TCP_H=1",
            "HAVE_POLL=1",
            "HAVE_POLL_H=1",
            "HAVE_POSTPROCESSOR=1",
            "HAVE_PTHREAD_H=1",
            "HAVE_PTHREAD_PRIO_INHERIT=1",
            "HAVE_SEARCH_H=1",
            "HAVE_STDINT_H=1",
            "HAVE_STDIO_H=1",
            "HAVE_STDLIB_H=1",
            "HAVE_STRINGS_H=1",
            "HAVE_STRING_H=1",
            "HAVE_SYS_MMAN_H=1",
            "HAVE_SYS_MSG_H=1",
            "HAVE_SYS_SELECT_H=1",
            "HAVE_SYS_SOCKET_H=1",
            "HAVE_SYS_STAT_H=1",
            "HAVE_SYS_TIME_H=1",
            "HAVE_SYS_TYPES_H=1",
            "HAVE_TIME_H=1",
            "HAVE_UNISTD_H=1",
            "HTTPS_SUPPORT=0",
            "OSX=1",
            'LT_OBJDIR=".libs/"',
            "MHD_USE_POSIX_THREADS=1",
            'PACKAGE="libmicrohttpd"',
            'PACKAGE_BUGREPORT="libmicrohttpd@gnu.org"',
            'PACKAGE_NAME="libmicrohttp"',
            'PACKAGE_STRING="libmicrohttpd 0.9.42"',
            'PACKAGE_TARNAME="libmicrohttpd"',
            'PACKAGE_URL=""',
            'PACKAGE_VERSION="0.9.42"',
            "SPDY_SUPPORT=0",
            "STDC_HEADERS=1",
            'VERSION="0.9.42"',
            '_MHD_EXTERN=__attribute__((visibility("default"))) extern',
        ],
      }],
      ['skia_os == "win"', {
        'sources': [
            '../third_party/externals/microhttpd/src/platform/w32functions.c',
        ],
        'msvs_disabled_warnings': [4244, 4996],
        'all_dependent_settings': {
          'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalDependencies': [ 'ws2_32.lib' ],
            },
          },
        },
        'defines=': [             # equals sign throws away most Skia defines (just noise)
            "_GNU_SOURCE=1",
            "BAUTH_SUPPORT=1",
            "DAUTH_SUPPORT=1",
            "EPOLL_SUPPORT=0",
            "HAVE_DECL_SOCK_NONBLOCK=0",
            "HAVE_DECL_TCP_CORK=0",
            "HAVE_DECL_TCP_NOPUSH=0",
            "HAVE_ERRNO_H=1",
            "HAVE_FCNTL_H=1",
            "HAVE_INET6=1",
            "HAVE_INTTYPES_H=1",
            "HAVE_LIMITS_H=1",
            "HAVE_LOCALE_H=1",
            "HAVE_MATH_H=1",
            "HAVE_MEMORY_H=1",
            "HAVE_MESSAGES=1",
#            "HAVE_POLL=0",
            "HAVE_POSTPROCESSOR=1",
            "HAVE_PTHREAD_H=0",
            "HAVE_STDINT_H=1",
            "HAVE_STDIO_H=1",
            "HAVE_STDLIB_H=1",
            "HAVE_STRINGS_H=1",
            "HAVE_STRING_H=1",
            "HAVE_SYS_STAT_H=1",
            "HAVE_SYS_TYPES_H=1",
            "HAVE_TIME_H=1",
            "HAVE_WINSOCK2_H=1",
            "HAVE_WS2TCPIP_H=1",
            "HTTPS_SUPPORT=0",
            "MSVC=1",
            "WINDOWS=1",
            'LT_OBJDIR=".libs/"',
            "MHD_DONT_USE_PIPES=1",
            "MHD_USE_W32_THREADS=1",
            'PACKAGE="libmicrohttpd"',
            'PACKAGE_BUGREPORT="libmicrohttpd@gnu.org"',
            'PACKAGE_NAME="libmicrohttp"',
            'PACKAGE_STRING="libmicrohttpd 0.9.42"',
            'PACKAGE_TARNAME="libmicrohttpd"',
            'PACKAGE_URL=""',
            'PACKAGE_VERSION="0.9.42"',
            "SPDY_SUPPORT=0",
            "STDC_HEADERS=1",
            'VERSION="0.9.42"',
            '_MHD_EXTERN=extern',
        ],
      }],
    ]
  }]
}
