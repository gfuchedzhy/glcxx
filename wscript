gl_logging_values = ['none', 'err', 'all']
def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--gl-log', action='store', default="none", help='controlls opengl call logging, possible values ' + str(gl_logging_values))

def configure(cnf):
    if not cnf.options.gl_log in gl_logging_values:
        cnf.fatal("--gl-log should be one of " + str(gl_logging_values))
    cnf.env.GL_LOG = cnf.options.gl_log

    cnf.load('compiler_cxx')
    cnf.check(features='cxx cxxstlib cxxshlib', defines='GL_GLEXT_PROTOTYPES', lib='GL', uselib_store='opengl')
    cnf.check_cxx(cxxflags='-std=c++14', uselib_store='cxxflags')
    cnf.check_cxx(header_name='glm/glm.hpp')

def build(bld):
    bld(features = 'cxx cxxstlib cxxshlib',
        source   = bld.path.ant_glob(['src/*.cpp']),
        target   = 'glcxx',
        # includes = ['src/engine'],
        use      = ['cxxflags', 'opengl'])
