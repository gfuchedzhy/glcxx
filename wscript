def options(opt):
    opt.load('compiler_cxx')

def configure(cnf):
    cnf.load('compiler_cxx')
    cnf.check(features='cxx cxxstlib', defines='GL_GLEXT_PROTOTYPES', lib='GL', uselib_store='opengl')
    cnf.check_cxx(header_name='glm/glm.hpp')
    cnf.check_cxx(cxxflags='-std=c++14', uselib_store='glcxx')

def build(bld):
    bld(features = 'cxx cxxstlib',
        source   = bld.path.ant_glob(['src/*.cpp']),
        target   = 'glcxx',
        use      = ['cxxflags', 'opengl'])
