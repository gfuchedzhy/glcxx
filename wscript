def options(opt):
    opt.load('compiler_cxx')

def configure(cnf):
    cnf.load('compiler_cxx')
    cnf.check(features='cxx cxxprogram', lib=['sfml-window', 'sfml-system'], uselib_store='sfml')

def build(bld):

    bld(features='cxx cxxstlib',
        source='src/engine/CEngine.cpp',
        target='engine',
        use=['sfml'])

    bld(features='cxx cxxprogram',
        source='src/app/main.cpp',
        includes=['src'],
        target='avia',
        use=['engine'])
