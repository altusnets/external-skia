{
  'variables': {
    'skia_warnings_as_errors': 0,
  },
  'targets': [{
    # Draws pictures cross-process.
    'target_name': 'nanomsg_picture_demo',
    'type': 'executable',
    'dependencies': [
      'skia_lib.gyp:skia_lib',
      'flags.gyp:flags',
      'libnanomsg',
    ],
    'sources': [ '../experimental/nanomsg/picture_demo.cpp' ],
  },{
    'target_name': 'libnanomsg',
    'type': 'static_library',

    # Clients can include nanomsg public header foo.h with #include "nanomsg/src/foo.h"
    'direct_dependent_settings': {
      'include_dirs': [ '../third_party/externals' ]
    },

    # To refresh: cd gyp; find ../third_party/externals/nanomsg/src -name "*.c"
    'sources': [
      '../third_party/externals/nanomsg/src/aio/ctx.c',
      '../third_party/externals/nanomsg/src/aio/fsm.c',
      '../third_party/externals/nanomsg/src/aio/poller.c',
      '../third_party/externals/nanomsg/src/aio/pool.c',
      '../third_party/externals/nanomsg/src/aio/timer.c',
      '../third_party/externals/nanomsg/src/aio/timerset.c',
      '../third_party/externals/nanomsg/src/aio/usock.c',
      '../third_party/externals/nanomsg/src/aio/worker.c',
      '../third_party/externals/nanomsg/src/core/device.c',
      '../third_party/externals/nanomsg/src/core/ep.c',
      '../third_party/externals/nanomsg/src/core/epbase.c',
      '../third_party/externals/nanomsg/src/core/global.c',
      '../third_party/externals/nanomsg/src/core/pipe.c',
      '../third_party/externals/nanomsg/src/core/poll.c',
      '../third_party/externals/nanomsg/src/core/sock.c',
      '../third_party/externals/nanomsg/src/core/sockbase.c',
      '../third_party/externals/nanomsg/src/core/symbol.c',
      '../third_party/externals/nanomsg/src/protocols/bus/bus.c',
      '../third_party/externals/nanomsg/src/protocols/bus/xbus.c',
      '../third_party/externals/nanomsg/src/protocols/pair/pair.c',
      '../third_party/externals/nanomsg/src/protocols/pair/xpair.c',
      '../third_party/externals/nanomsg/src/protocols/pipeline/pull.c',
      '../third_party/externals/nanomsg/src/protocols/pipeline/push.c',
      '../third_party/externals/nanomsg/src/protocols/pipeline/xpull.c',
      '../third_party/externals/nanomsg/src/protocols/pipeline/xpush.c',
      '../third_party/externals/nanomsg/src/protocols/pubsub/pub.c',
      '../third_party/externals/nanomsg/src/protocols/pubsub/sub.c',
      '../third_party/externals/nanomsg/src/protocols/pubsub/trie.c',
      '../third_party/externals/nanomsg/src/protocols/pubsub/xpub.c',
      '../third_party/externals/nanomsg/src/protocols/pubsub/xsub.c',
      '../third_party/externals/nanomsg/src/protocols/reqrep/rep.c',
      '../third_party/externals/nanomsg/src/protocols/reqrep/req.c',
      '../third_party/externals/nanomsg/src/protocols/reqrep/xrep.c',
      '../third_party/externals/nanomsg/src/protocols/reqrep/xreq.c',
      '../third_party/externals/nanomsg/src/protocols/survey/respondent.c',
      '../third_party/externals/nanomsg/src/protocols/survey/surveyor.c',
      '../third_party/externals/nanomsg/src/protocols/survey/xrespondent.c',
      '../third_party/externals/nanomsg/src/protocols/survey/xsurveyor.c',
      '../third_party/externals/nanomsg/src/protocols/utils/dist.c',
      '../third_party/externals/nanomsg/src/protocols/utils/excl.c',
      '../third_party/externals/nanomsg/src/protocols/utils/fq.c',
      '../third_party/externals/nanomsg/src/protocols/utils/lb.c',
      '../third_party/externals/nanomsg/src/protocols/utils/priolist.c',
      '../third_party/externals/nanomsg/src/transports/inproc/binproc.c',
      '../third_party/externals/nanomsg/src/transports/inproc/cinproc.c',
      '../third_party/externals/nanomsg/src/transports/inproc/inproc.c',
      '../third_party/externals/nanomsg/src/transports/inproc/ins.c',
      '../third_party/externals/nanomsg/src/transports/inproc/msgqueue.c',
      '../third_party/externals/nanomsg/src/transports/inproc/sinproc.c',
      '../third_party/externals/nanomsg/src/transports/ipc/aipc.c',
      '../third_party/externals/nanomsg/src/transports/ipc/bipc.c',
      '../third_party/externals/nanomsg/src/transports/ipc/cipc.c',
      '../third_party/externals/nanomsg/src/transports/ipc/ipc.c',
      '../third_party/externals/nanomsg/src/transports/ipc/sipc.c',
      '../third_party/externals/nanomsg/src/transports/tcp/atcp.c',
      '../third_party/externals/nanomsg/src/transports/tcp/btcp.c',
      '../third_party/externals/nanomsg/src/transports/tcp/ctcp.c',
      '../third_party/externals/nanomsg/src/transports/tcp/stcp.c',
      '../third_party/externals/nanomsg/src/transports/tcp/tcp.c',
      '../third_party/externals/nanomsg/src/transports/utils/backoff.c',
      '../third_party/externals/nanomsg/src/transports/utils/dns.c',
      '../third_party/externals/nanomsg/src/transports/utils/iface.c',
      '../third_party/externals/nanomsg/src/transports/utils/literal.c',
      '../third_party/externals/nanomsg/src/transports/utils/port.c',
      '../third_party/externals/nanomsg/src/transports/utils/streamhdr.c',
      '../third_party/externals/nanomsg/src/utils/alloc.c',
      '../third_party/externals/nanomsg/src/utils/atomic.c',
      '../third_party/externals/nanomsg/src/utils/chunk.c',
      '../third_party/externals/nanomsg/src/utils/chunkref.c',
      '../third_party/externals/nanomsg/src/utils/clock.c',
      '../third_party/externals/nanomsg/src/utils/closefd.c',
      '../third_party/externals/nanomsg/src/utils/efd.c',
      '../third_party/externals/nanomsg/src/utils/err.c',
      '../third_party/externals/nanomsg/src/utils/glock.c',
      '../third_party/externals/nanomsg/src/utils/hash.c',
      '../third_party/externals/nanomsg/src/utils/list.c',
      '../third_party/externals/nanomsg/src/utils/msg.c',
      '../third_party/externals/nanomsg/src/utils/mutex.c',
      '../third_party/externals/nanomsg/src/utils/queue.c',
      '../third_party/externals/nanomsg/src/utils/random.c',
      '../third_party/externals/nanomsg/src/utils/sem.c',
      '../third_party/externals/nanomsg/src/utils/sleep.c',
      '../third_party/externals/nanomsg/src/utils/stopwatch.c',
      '../third_party/externals/nanomsg/src/utils/thread.c',
      '../third_party/externals/nanomsg/src/utils/wire.c',
    ],

    # TODO(mtklein): Support Windows?
    # To refresh: cd third_party/externals/nanomsg; ./autogen.sh; ./configure; copy from Makefile.
    'conditions': [
      ['skia_os == "linux"', {
        'cflags': [ '-Wno-missing-field-initializers' ],
        'libraries': [ '-lanl' ],       # Provides getaddrinfo_a and co.
        'direct_dependent_settings': {
            'libraries': [ '-lanl' ],
        },
        'defines=': [             # equals sign throws away most Skia defines (just noise)
          'HAVE_ACCEPT4',
          'HAVE_ARPA_INET_H',
          'HAVE_CLOCK_GETTIME',
          'HAVE_DLFCN_H',
          'HAVE_EPOLL_CREATE',
          'HAVE_EVENTFD',
          'HAVE_GETIFADDRS',
          'HAVE_INTTYPES_H',
          'HAVE_MEMORY_H',
          'HAVE_NETDB_H',
          'HAVE_NETINET_IN_H',
          'HAVE_PIPE',
          'HAVE_PIPE2',
          'HAVE_POLL',
          'HAVE_PTHREAD_PRIO_INHERIT',
          'HAVE_STDINT_H',
          'HAVE_STDLIB_H',
          'HAVE_STRINGS_H',
          'HAVE_STRING_H',
          'HAVE_SYS_IOCTL_H',
          'HAVE_SYS_SOCKET_H',
          'HAVE_SYS_STAT_H',
          'HAVE_SYS_TYPES_H',
          'HAVE_UNISTD_H',
          'HAVE_UNISTD_H',
          'NN_HAVE_ACCEPT4',
          'NN_HAVE_CLANG',
          'NN_HAVE_EVENTFD',
          'NN_HAVE_GCC',
          'NN_HAVE_GETADDRINFO_A',
          'NN_HAVE_LINUX',
          'NN_HAVE_PIPE',
          'NN_HAVE_PIPE2',
          'NN_HAVE_POLL',
          'NN_HAVE_SEMAPHORE',
          'NN_HAVE_SOCKETPAIR',
          'NN_USE_EPOLL',
          'NN_USE_EVENTFD',
          'NN_USE_IFADDRS',
          'STDC_HEADERS',
          '_GNU_SOURCE',
        ],
      }],
      ['skia_os == "mac"', {
        'defines=': [             # equals sign throws away most Skia defines (just noise)
          'HAVE_ARPA_INET_H',
          'HAVE_DLFCN_H',
          'HAVE_GETIFADDRS',
          'HAVE_INTTYPES_H',
          'HAVE_KQUEUE',
          'HAVE_MEMORY_H',
          'HAVE_NETDB_H',
          'HAVE_NETINET_IN_H',
          'HAVE_PIPE',
          'HAVE_POLL',
          'HAVE_PTHREAD_PRIO_INHERIT',
          'HAVE_STDINT_H',
          'HAVE_STDLIB_H',
          'HAVE_STRINGS_H',
          'HAVE_STRING_H',
          'HAVE_SYS_IOCTL_H',
          'HAVE_SYS_SOCKET_H',
          'HAVE_SYS_STAT_H',
          'HAVE_SYS_TYPES_H',
          'HAVE_UNISTD_H',
          'NN_HAVE_CLANG',
          'NN_HAVE_GCC',
          'NN_HAVE_OSX',
          'NN_HAVE_PIPE',
          'NN_HAVE_POLL',
          'NN_HAVE_SEMAPHORE',
          'NN_HAVE_SOCKETPAIR',
          'NN_USE_IFADDRS',
          'NN_USE_KQUEUE',
          'NN_USE_PIPE',
          'STDC_HEADERS',
          '_THREAD_SAFE',
        ],
      }],
    ]
  }]
}
