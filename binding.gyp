{
  'targets': [
    {
      'target_name': 'shortduid',
      'product_name': 'shortduid',
      'sources': [
        'src/main.cpp',
        'src/shortduid.cpp',
        'hashids/hashids.cpp',
      ],
      'cflags': [
        '-std=c++11'
      ],
      'conditions': [
        [ 'OS=="mac"', {
          "xcode_settings": {
            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
            'OTHER_LDFLAGS': ['-stdlib=libc++'],
            'MACOSX_DEPLOYMENT_TARGET': '10.9'
          }
        }],
      ],
    },
  ],
}
