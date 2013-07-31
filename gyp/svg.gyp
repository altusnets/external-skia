{
  'targets': [
    {
      'target_name': 'svg',
      'product_name': 'skia_svg',
      'type': 'static_library',
      'standalone_static_library': 1,
      'dependencies': [
        'skia_lib.gyp:skia_lib',
        'xml.gyp:*',
      ],
      'include_dirs': [
        '../include/svg',
      ],
      'sources': [
        '../include/svg/SkSVGAttribute.h',
        '../include/svg/SkSVGBase.h',
        '../include/svg/SkSVGPaintState.h',
        '../include/svg/SkSVGParser.h',
        '../include/svg/SkSVGTypes.h',

        '../src/svg/SkSVGCircle.cpp',
        '../src/svg/SkSVGCircle.h',
        '../src/svg/SkSVGClipPath.cpp',
        '../src/svg/SkSVGClipPath.h',
        '../src/svg/SkSVGDefs.cpp',
        '../src/svg/SkSVGDefs.h',
        '../src/svg/SkSVGElements.cpp',
        '../src/svg/SkSVGElements.h',
        '../src/svg/SkSVGEllipse.cpp',
        '../src/svg/SkSVGEllipse.h',
        '../src/svg/SkSVGFeColorMatrix.cpp',
        '../src/svg/SkSVGFeColorMatrix.h',
        '../src/svg/SkSVGFilter.cpp',
        '../src/svg/SkSVGFilter.h',
        '../src/svg/SkSVGG.cpp',
        '../src/svg/SkSVGG.h',
        '../src/svg/SkSVGGradient.cpp',
        '../src/svg/SkSVGGradient.h',
        '../src/svg/SkSVGGroup.cpp',
        '../src/svg/SkSVGGroup.h',
        '../src/svg/SkSVGImage.cpp',
        '../src/svg/SkSVGImage.h',
        '../src/svg/SkSVGLine.cpp',
        '../src/svg/SkSVGLine.h',
        '../src/svg/SkSVGLinearGradient.cpp',
        '../src/svg/SkSVGLinearGradient.h',
        '../src/svg/SkSVGMask.cpp',
        '../src/svg/SkSVGMask.h',
        '../src/svg/SkSVGMetadata.cpp',
        '../src/svg/SkSVGMetadata.h',
        '../src/svg/SkSVGPaintState.cpp',
        '../src/svg/SkSVGParser.cpp',
        '../src/svg/SkSVGPath.cpp',
        '../src/svg/SkSVGPath.h',
        '../src/svg/SkSVGPolygon.cpp',
        '../src/svg/SkSVGPolygon.h',
        '../src/svg/SkSVGPolyline.cpp',
        '../src/svg/SkSVGPolyline.h',
        '../src/svg/SkSVGRadialGradient.cpp',
        '../src/svg/SkSVGRadialGradient.h',
        '../src/svg/SkSVGRect.cpp',
        '../src/svg/SkSVGRect.h',
        '../src/svg/SkSVGStop.cpp',
        '../src/svg/SkSVGStop.h',
        '../src/svg/SkSVGSVG.cpp',
        '../src/svg/SkSVGSVG.h',
        '../src/svg/SkSVGSymbol.cpp',
        '../src/svg/SkSVGSymbol.h',
        '../src/svg/SkSVGText.cpp',
        '../src/svg/SkSVGText.h',
        '../src/svg/SkSVGUse.cpp',
      ],
      'sources!' : [
          '../src/svg/SkSVG.cpp', # doesn't compile, maybe this is test code?
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../include/svg',
        ],
      },
    },
  ],
}

# Local Variables:
# tab-width:2
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=2 shiftwidth=2:
