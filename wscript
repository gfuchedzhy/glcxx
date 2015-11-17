from waflib import Task, TaskGen
class strip(Task.Task):
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

def options(opt):
    opt.load('compiler_cxx')

def configure(cnf):
    cnf.load('compiler_cxx')
    cnf.find_program('strip')
    cnf.check(features='cxx cxxprogram', lib=['sfml-window', 'sfml-system'], uselib_store='sfml')
    cnf.check(features='cxx cxxprogram', defines=['GL_GLEXT_PROTOTYPES'], lib=['GL'], uselib_store='opengl')
    cnf.check_cxx(cxxflags='-std=c++14', uselib_store='cxxflags')
    cnf.check_cxx(header_name='glm/glm.hpp')
    cnf.check_cxx(cxxflags='-std=c++11', header_name='gli/load.hpp')

def build(bld):
    appname = 'avia'
    defines = ['APPNAME="' + appname + '"']
    bld(features = 'cxx cxxstlib',
        source   = bld.path.ant_glob('src/engine/*.cpp'),
        target   = 'engine',
        defines  = defines,
        use      = ['sfml', 'cxxflags', 'opengl'])

    bld(features = 'cxx cxxprogram strip',
        source   = bld.path.ant_glob('src/app/*.cpp'),
        target   = appname + '.out',
        includes = ['src/engine'],
        defines  = defines,
        use      = ['engine', 'cxxflags', 'sfml'])
