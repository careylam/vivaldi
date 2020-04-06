# DO NOT EDIT EXCEPT FOR LOCAL TESTING.

hooks = [
    {
    'action': [
      'python',
      'vivaldi/chromium/build/landmines.py'
      ],
    'pattern':
         '.',
    'name':
         'landmines'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/linux/sysroot_scripts/install-sysroot.py',
      '--running-as-hook'
      ],
    'pattern':
         '.',
    'name':
         'sysroot'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/third_party/binutils/download.py'
    ],
    'pattern':
         'vivaldi/chromium/third_party/binutils',
    'name':
         'binutils'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/tools/clang/scripts/update.py',
      '--if-needed'
      ],
    'pattern':
         '.',
    'name':
         'clang'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/util/lastchange.py',
      '--git-hash-only',
      '-o',
      'vivaldi/chromium/build/util/LASTCHANGE'
       ],
    'pattern':
         '.',
    'name':
         'lastchange'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/util/lastchange.py',
      '--git-hash-only',
      '-s',
      'vivaldi/chromium/third_party/WebKit',
      '-o',
      'vivaldi/chromium/build/util/LASTCHANGE.blink'
      ],
    'pattern':
         '.',
    'name':
         'lastchange_blink'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/util/lastchange.py',
      '--git-hash-only',
      '-s',
      'vivaldi/.',
      '--name-suffix',
      '_VIVALDI',
      '-o',
      'vivaldi/chromium/build/util/LASTCHANGE.vivaldi'
      ],
    'pattern':
         '.',
    'name':
         'lastchange_vivaldi'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=win32',
      '--no_auth',
      '--bucket',
      'chromium-gn',
      '-s',
      'vivaldi/chromium/buildtools/win/gn.exe.sha1'
      ],
    'pattern':
         '.',
    'name':
         'gn_win'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=darwin',
      '--no_auth',
      '--bucket',
      'chromium-gn',
      '-s',
      'vivaldi/chromium/buildtools/mac/gn.sha1'
      ],
    'pattern':
         '.',
    'name':
         'gn_mac'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=linux*',
      '--no_auth',
      '--bucket',
      'chromium-gn',
      '-s',
      'vivaldi/chromium/buildtools/linux64/gn.sha1'
      ],
    'pattern':
         '.',
    'name':
         'gn_linux64'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=win32',
      '--no_auth',
      '--bucket',
      'chromium-clang-format',
      '-s',
      'vivaldi/chromium/buildtools/win/clang-format.exe.sha1'
      ],
    'pattern':
         '.',
    'name':
         'clang_format_win'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=darwin',
      '--no_auth',
      '--bucket',
      'chromium-clang-format',
      '-s',
      'vivaldi/chromium/buildtools/mac/clang-format.sha1'
    ],
    'pattern':
         '.',
    'name':
         'clang_format_mac'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=linux*',
      '--no_auth',
      '--bucket',
      'chromium-clang-format',
      '-s',
      'vivaldi/chromium/buildtools/linux64/clang-format.sha1'
      ],
    'pattern':
         '.',
    'name':
         'clang_format_linux'
    },
    # Pull the prebuilt libc++ static library for mac.
    {
      'name': 'libcpp_mac',
      'pattern': '.',
      'action': [ 'download_from_google_storage',
                  '--no_resume',
                  '--platform=darwin',
                  '--no_auth',
                  '--bucket', 'chromium-libcpp',
                  '-s', 'vivaldi/chromium/third_party/libc++-static/libc++.a.sha1',
      ],
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=win32',
      '--no_auth',
      '--bucket',
      'chromium-luci',
      '-d',
      'vivaldi/chromium/tools/luci-go/win64'
    ],
    'pattern':
         '.',
    'name':
         'luci-go_win'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=darwin',
      '--no_auth',
      '--bucket',
      'chromium-luci',
      '-d',
      'vivaldi/chromium/tools/luci-go/mac64'
      ],
    'pattern':
         '.',
    'name':
         'luci-go_mac'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=linux*',
      '--no_auth',
      '--bucket',
      'chromium-luci',
      '-d',
      'vivaldi/chromium/tools/luci-go/linux64'
      ],
    'pattern':
         '.',
    'name':
         'luci-go_linux'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=linux*',
      '--no_auth',
      '--bucket',
      'chromium-eu-strip',
      '-s',
      'vivaldi/chromium/build/linux/bin/eu-strip.sha1'
      ],
    'pattern':
         '.',
    'name':
         'eu-strip'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=win32',
      '--no_auth',
      '--bucket',
      'chromium-drmemory',
      '-s',
      'vivaldi/chromium/third_party/drmemory/drmemory-windows-sfx.exe.sha1'
      ],
    'pattern':
         '.',
    'name':
         'drmemory'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/get_syzygy_binaries.py',
      '--output-dir',
      'vivaldi/chromium/third_party/syzygy/binaries',
      '--revision=24abcb05aa6cc35545111d244378ef37b5d5218c',
      '--overwrite'
      ],
    'pattern':
         '.',
    'name':
         'syzygy-binaries'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/build/get_syzygy_binaries.py',
      '--output-dir',
      'vivaldi/chromium/third_party/kasko/binaries',
      '--revision=266a18d9209be5ca5c5dcd0620942b82a2d238f3',
      '--resource=kasko.zip',
      '--resource=kasko_symbols.zip',
      '--overwrite'
      ],
    'pattern':
         '.',
    'name':
         'kasko'
    },
    {
    'action': [
      'download_from_google_storage',
      '--no_resume',
      '--platform=win32',
      '--directory',
      '--recursive',
      '--no_auth',
      '--num_threads=16',
      '--bucket',
      'chromium-apache-win32',
      'vivaldi/chromium/third_party/apache-win32'
    ],
    'pattern':
         '\\.sha1',
    'name':
         'apache_win32'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/third_party/instrumented_libraries/scripts/download_binaries.py'
      ],
    'pattern':
         '\\.sha1',
    'name':
         'instrumented_libraries'
    },
    {
    'action': [
      'python',
      'vivaldi/chromium/tools/remove_stale_pyc_files.py',
      'vivaldi/chromium/android_webview/tools',
      'vivaldi/chromium/gpu/gles2_conform_support',
      'vivaldi/src/infra',
      'vivaldi/chromium/ppapi',
      'vivaldi/chromium/printing',
      'vivaldi/chromium/third_party/closure_compiler/build',
      'vivaldi/chromium/tools'
      ],
    'pattern':
         '.',
    'name':
         'remove_stale_pyc_files'
    },
    {
    'action': [
      'python',
      '-O',
      'vivaldi/chromium/build/gyp_chromium',
      '--depth',
      'vivaldi/chromium/.',
      '-I',
      'vivaldi/vivaldi_common.gypi',
      '-G',
      'output_dir=../out',
      'vivaldi/all.gyp'
    ],
    'pattern':
         '.',
    'name':
         'gyp'
    }
]
