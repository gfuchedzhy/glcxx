def options(opt):
    opt.load('compiler_cxx')

def configure(cnf):
    cnf.load('compiler_cxx')
    cnf.check(features='cxx cxxprogram', lib=['sfml-window', 'sfml-system'], uselib_store='sfml')

def build(bld):
    appname = 'avia'
    bld(features = 'cxx cxxstlib',
        source   = bld.path.ant_glob('src/engine/*.cpp'),
        target   = 'engine',
        defines=['APPNAME="' + appname + '"'],
        use      = ['sfml'])

    bld(features = 'cxx cxxprogram',
        source   = bld.path.ant_glob('src/app/*.cpp'),
        includes = ['src'],
        defines=['APPNAME="' + appname + '"'],
        target   = appname,
        use      = ['engine'])
