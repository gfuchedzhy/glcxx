def options(opt):
    opt.load('compiler_cxx')

def configure(cnf):
    cnf.load('compiler_cxx')
    cnf.check(features='cxx cxxprogram', lib=['sfml-window', 'sfml-system'], uselib_store='sfml')
    cnf.check(features='cxx cxxprogram', lib=['GL'], uselib_store='opengl')
    cnf.check_cxx(cxxflags='-std=c++14', uselib_store='cxxflags')

def build(bld):
    appname = 'avia'
    bld(features = 'cxx cxxstlib',
        source   = bld.path.ant_glob('src/engine/*.cpp'),
        target   = 'engine',
        defines=['APPNAME="' + appname + '"', 'GL_GLEXT_PROTOTYPES'],
        use      = ['sfml', 'cxxflags', 'opengl'])

    bld(features = 'cxx cxxprogram',
        source   = bld.path.ant_glob('src/app/*.cpp'),
        target   = appname,
        includes = ['src'],
        defines=['APPNAME="' + appname + '"'],
        use      = ['engine', 'cxxflags'])
