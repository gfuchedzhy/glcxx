from os import path
from waflib import Task, TaskGen
class strip(Task.Task):
	def keyword(self):
		return "Stripping"
	run_str = '${STRIP} -o ${TGT} ${SRC}'
	color   = 'CYAN'

@TaskGen.feature('strip')
@TaskGen.after('apply_link')
def add_strip_task(self):
	try:
		link_task = self.link_task
	except:
		return
	self.create_task('strip', self.link_task.outputs[0], self.link_task.outputs[0].change_ext(''))

class glsl(Task.Task):
    color = 'PINK'
    def keyword(self):
        return "Processing"
    def run(self):
        with open(self.outputs[0].abspath(), 'w') as out, open(self.inputs[0].abspath()) as inp:
           print('#include "Utils.hpp"', file=out)
           print('template<typename TName>', file=out)
           print('const char* programSrc();\n', file=out)
           print('template<>', file=out)
           print('const char* programSrc<cts("' + path.splitext(path.basename(self.inputs[0].abspath()))[0] + '")>()', file=out)
           print('{\n\tstatic char retval[] = R"(', file=out)
           for line in inp:
               print(line, file=out, end='')
           print(')";\n\treturn retval;}', file=out)

@TaskGen.extension('.glsl')
def process(self, node):
    cpp_node = node.change_ext('.cpp')
    self.create_task('glsl', node, cpp_node)
    self.source.append(cpp_node)

gl_logging_values = ['none', 'err', 'all']
def options(opt):
    opt.recurse('res')
    opt.load('compiler_cxx')
    opt.add_option('--gl-log', action='store', default="none", help='controlls opengl call logging, possible values ' + str(gl_logging_values))

def configure(cnf):
    if not cnf.options.gl_log in gl_logging_values:
        cnf.fatal("--gl-log should be one of " + str(gl_logging_values))
    cnf.env.GL_LOG = cnf.options.gl_log

    cnf.recurse('res')
    cnf.load('compiler_cxx')
    cnf.find_program('strip')
    cnf.check(features='cxx cxxprogram', lib=['sfml-window', 'sfml-system'], uselib_store='sfml')
    cnf.check(features='cxx cxxprogram', lib='assimp', uselib_store='assimp')
    cnf.check(features='cxx cxxprogram', defines='GL_GLEXT_PROTOTYPES', lib='GL', uselib_store='opengl')
    cnf.check_cxx(cxxflags='-std=c++14', uselib_store='cxxflags')
    cnf.check_cxx(header_name='glm/glm.hpp')
    cnf.check_cxx(cxxflags='-std=c++11', header_name='gli/load.hpp')

def build(bld):
    bld.recurse('res')

    appname = 'aersy'
    defines = ['APPNAME="' + appname + '"', 'GL_LOG_' + bld.env.GL_LOG.upper()]
    bld(features = 'cxx cxxstlib',
        source   = bld.path.ant_glob(['src/engine/*.cpp', 'src/engine/*.glsl']),
        target   = 'engine',
        includes = ['src/engine'],
        defines  = defines,
        use      = ['sfml', 'cxxflags', 'opengl'])

    bld(features = 'cxx cxxprogram strip',
        source   = bld.path.ant_glob('src/app/*.cpp'),
        target   = appname + '.out',
        includes = ['src/engine'],
        defines  = defines,
        use      = ['engine', 'cxxflags', 'sfml', 'assimp'])
