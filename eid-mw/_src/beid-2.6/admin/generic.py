## Thomas Nagy, 2005
""" Run scons -h to display the associated help, or look below """

import os, re, types, sys, string, shutil, stat, glob
import SCons.Defaults
import SCons.Tool
import SCons.Util
from SCons.Script.SConscript import SConsEnvironment
from SCons.Options import Options, PathOption

def getreldir(lenv):
	cwd=os.getcwd()
	root=SCons.Node.FS.default_fs.Dir('#').abspath
	return cwd.replace(root,'').lstrip('/')

def dist(env, appname, version=None):
	### To make a tarball of your masterpiece, use 'scons dist'
	import os
	if 'dist' in sys.argv:
		if not version: VERSION=os.popen("cat VERSION").read().rstrip()
		else: VERSION=version
		FOLDER  = appname+'-'+VERSION
		TMPFOLD = ".tmp"+FOLDER
		ARCHIVE = FOLDER+'.tar.bz2'

		## check if the temporary directory already exists
		os.popen('rm -rf %s %s %s' % (FOLDER, TMPFOLD, ARCHIVE) )

		## create a temporary directory
		startdir = os.getcwd()
	
		os.popen("mkdir -p "+TMPFOLD)	
		os.popen("cp -R * "+TMPFOLD)
		os.popen("mv "+TMPFOLD+" "+FOLDER)

		## remove scons-local if it is unpacked
		os.popen("rm -rf "+FOLDER+"/scons "+FOLDER+"/sconsign "+FOLDER+"/scons-local-0.96.1")

		## remove our object files first
		os.popen("find "+FOLDER+" -name \"cache\" | xargs rm -rf")
		os.popen("find "+FOLDER+" -name \"build\" | xargs rm -rf")
		os.popen("find "+FOLDER+" -name \"*.pyc\" | xargs rm -f")

		## CVS cleanup
		os.popen("find "+FOLDER+" -name \"CVS\" | xargs rm -rf")
		os.popen("find "+FOLDER+" -name \".cvsignore\" | xargs rm -rf")

		## Subversion cleanup
		os.popen("find %s -name .svn -type d | xargs rm -rf" % FOLDER)

		## GNU Arch cleanup
		os.popen("find "+FOLDER+" -name \"{arch}\" | xargs rm -rf")
		os.popen("find "+FOLDER+" -name \".arch-i*\" | xargs rm -rf")

		## Create the tarball (coloured output)
		print "\033[92m"+"Writing archive "+ARCHIVE+"\033[0m"
		os.popen("tar cjf "+ARCHIVE+" "+FOLDER)

		## Remove the temporary directory
		os.popen('rm -rf '+FOLDER)
		env.Exit(0)

	if 'distclean' in sys.argv:
		## Remove the cache directory
		import os, shutil
		if os.path.isdir(env['CACHEDIR']): shutil.rmtree(env['CACHEDIR'])
		os.popen("find . -name \"*.pyc\" | xargs rm -rf")
		env.Exit(0)

colors= {
'BOLD'  :"\033[1m",
'RED'   :"\033[91m",
'GREEN' :"\033[92m",
'YELLOW':"\033[1m", #"\033[93m" # unreadable on white backgrounds
'CYAN'  :"\033[96m",
'NORMAL':"\033[0m",
}

def pprint(env, col, str, label=''):
	if env.has_key('NOCOLORS'):
		print "%s %s" % (str, label)
		return
	try: mycol=colors[col]
	except: mycol=''
	print "%s%s%s %s" % (mycol, str, colors['NORMAL'], label)

class genobj:
	def __init__(self, val, env):
		if not val in "program shlib kioslave staticlib".split():
			print "unknown genobj given: "+val
			env.Exit(1)

		self.type = val
		self.orenv = env
		self.env   = None
		self.executed = 0

		self.target=''
		self.src=None

		self.cxxflags=''
		self.cflags=''
		self.includes=''

		self.linkflags=''
		self.libpaths=''
		self.libs=''

		# vars used by shlibs
		self.vnum=''
		self.libprefix=''

		# a directory where to install the targets (optional)
		self.instdir=''

		# change the working directory before reading the targets
		self.chdir=''

		# unix permissions
		self.perms=''

		# these members are private
		self.chdir_lock=None
		self.dirprefix='./'
		self.old_os_dir=''
		self.old_fs_dir=''
		self.p_local_shlibs=[]
		self.p_local_staticlibs=[]
		self.p_global_shlibs=[]

		self.p_localsource=None
		self.p_localtarget=None

		# work directory
		self.workdir_lock=None
		self.orig_fs_dir=SCons.Node.FS.default_fs.getcwd()
		self.not_orig_fs_dir=''
		self.not_orig_os_dir=''

		if not env.has_key('USE_THE_FORCE_LUKE'): env['USE_THE_FORCE_LUKE']=[self]
		else: env['USE_THE_FORCE_LUKE'].append(self)

	def joinpath(self, val):
		if len(self.dirprefix)<3: return val
		dir=self.dirprefix
		thing=self.orenv.make_list(val)
		files=[]
		bdir="./"
		if self.orenv.has_key('_BUILDDIR_'): bdir=self.orenv['_BUILDDIR_']
		for v in thing: files.append( self.orenv.join(bdir, dir, v) )
		return files

	# a list of paths, with absolute and relative ones
	def fixpath(self, val):
		def reldir(dir):
			ndir    = SCons.Node.FS.default_fs.Dir(dir).srcnode().abspath
			rootdir = SCons.Node.FS.default_fs.Dir('#').abspath
			return ndir.replace(rootdir, '').lstrip('/')

		dir=self.dirprefix
		if not len(dir)>2: dir=reldir('.')

		thing=self.orenv.make_list(val)
		ret=[]
		bdir="./"
		if self.orenv.has_key('_BUILDDIR_'): bdir=self.orenv['_BUILDDIR_']
		for v in thing:
			#if v[:2] == "./" or v[:3] == "../":
			#	ret.append( self.orenv.join('#', bdir, dir, v) )
			#elif v[:1] == "#" or v[:1] == "/":
			#	ret.append( v )
			#else:
			#	ret.append( self.orenv.join('#', bdir, dir, v) )
			if v[:1] == "#" or v[:1] == "/":
				ret.append(v)
			else:
				ret.append( self.orenv.join('#', bdir, dir, v) )

		return ret

	def lockworkdir(self):
		if self.workdir_lock: return
		self.workdir_lock=1
		self.not_orig_fs_dir=SCons.Node.FS.default_fs.getcwd()
		self.not_orig_os_dir=os.getcwd()
		SCons.Node.FS.default_fs.chdir( self.orig_fs_dir, change_os_dir=1)

	def unlockworkdir(self):
		if not self.workdir_lock: return
		SCons.Node.FS.default_fs.chdir( self.not_orig_fs_dir, change_os_dir=0)
		os.chdir(self.not_orig_os_dir)
		self.workdir_lock=None

	def execute(self):
		if self.executed: return

		if self.orenv.has_key('DUMPCONFIG'):
			self.xml()
			self.executed=1
			return

		self.env = self.orenv.Copy()

		if not self.p_localtarget: self.p_localtarget = self.joinpath(self.target)
		if not self.p_localsource: self.p_localsource = self.joinpath(self.src)

		if (not self.src or len(self.src) == 0) and not self.p_localsource:
			self.env.pprint('RED',"no source file given to object - self.src")
			self.env.Exit(1)
		if not self.target:
			self.env.pprint('RED',"no target given to object - self.target")
			self.env.Exit(1)
		if not self.env.has_key('nosmart_includes'): self.env.AppendUnique(CPPPATH=['./'])
		if self.type == "kioslave": self.libprefix=''

		if len(self.includes)>0: self.env.AppendUnique(CPPPATH=self.fixpath(self.includes))
		if len(self.cxxflags)>0: self.env.AppendUnique(CXXFLAGS=self.env.make_list(self.cxxflags))
		if len(self.cflags)>0: self.env.AppendUnique(CCFLAGS=self.env.make_list(self.cflags))

		llist=self.env.make_list(self.libs)
		lext=['.so', '.la']
		sext='.a'.split()
		for l in llist:
			sal=SCons.Util.splitext(l)
			if len(sal)>1:
				if sal[1] in lext: self.p_local_shlibs.append(self.fixpath(sal[0]+'.so')[0])
				elif sal[1] in sext: self.p_local_staticlibs.append(self.fixpath(sal[0]+'.a')[0])
				else: self.p_global_shlibs.append(l)

		if len(self.p_global_shlibs)>0: self.env.AppendUnique(LIBS=self.p_global_shlibs)
		if len(self.libpaths)>0:   self.env.PrependUnique(LIBPATH=self.fixpath(self.libpaths))
		if len(self.linkflags)>0:  self.env.PrependUnique(LINKFLAGS=self.env.make_list(self.linkflags))
		if len(self.p_local_shlibs)>0:
			self.env.link_local_shlib(self.p_local_shlibs)
		if len(self.p_local_staticlibs)>0:
			self.env.link_local_staticlib(self.p_local_staticlibs)

		# the target to return - no more self.env modification is allowed after this part
		ret=None
		if self.type=='shlib' or self.type=='kioslave':
			ret=self.env.bksys_shlib(self.p_localtarget, self.p_localsource, self.instdir, 
				self.libprefix, self.vnum)
		elif self.type=='program':
			ret=self.env.Program(self.p_localtarget, self.p_localsource)
			if not self.env.has_key('NOAUTOINSTALL'):
				ins=self.env.bksys_install(self.instdir, ret)
				if self.perms: self.env.AddPostAction(ins, self.env.Chmod(ins, self.perms))
		elif self.type=='staticlib':
			ret=self.env.StaticLibrary(self.p_localtarget, self.p_localsource)

		# we link the program against a shared library made locally, add the dependency
		if len(self.p_local_shlibs)>0:
			if ret: self.env.Depends( ret, self.p_local_shlibs )
		if len(self.p_local_staticlibs)>0:
			if ret: self.env.Depends( ret, self.p_local_staticlibs )

		self.executed=1

## Copy function that honors symlinks
def copy_bksys(dest, source, env):
        if os.path.islink(source):
		#print "symlinking "+source+" "+dest
		if os.path.islink(dest):
			os.unlink(dest)
		os.symlink(os.readlink(source), dest)
	else:
		shutil.copy2(source, dest)
		st=os.stat(source)
		os.chmod(dest, stat.S_IMODE(st[stat.ST_MODE]) | stat.S_IWRITE)
	return 0

## Return a list of things
def make_list(env, s):
	if type(s) is types.ListType: return s
	else:
		try: return s.split()
		except AttributeError: return s

def join(lenv, s1, s2, s3=None, s4=None):
	if s4 and s3: return lenv.join(s1, s2, lenv.join(s3, s4))
	if s3 and s2: return lenv.join(s1, lenv.join(s2, s3))
	elif not s2: return s1
	# having s1, s2
	#print "path1 is "+s1+" path2 is "+s2+" "+os.path.join(s1,string.lstrip(s2,'/'))
	if not s1: s1="/"
	return os.path.join(s1,string.lstrip(s2,'/'))

def exists(env):
	return true

# record a dump of the environment
bks_dump='<?xml version="1.0" encoding="UTF-8"?>\n<bksys version="1">\n'
def add_dump(nenv, str):
	global bks_dump
	if str: bks_dump+=str
def get_dump(nenv):
	if not nenv.has_key('DUMPCONFIG'):
		nenv.pprint('RED','WARNING: trying to get a dump while DUMPCONFIG is not set - this will not work')
	global bks_dump
	return bks_dump+"</bksys>\n"

def generate(env):
	## Bksys requires scons 0.96
	env.EnsureSConsVersion(0, 96)

	SConsEnvironment.pprint = pprint
	SConsEnvironment.make_list = make_list
	SConsEnvironment.join = join
	SConsEnvironment.dist = dist
	SConsEnvironment.getreldir = getreldir
	SConsEnvironment.add_dump = add_dump
	SConsEnvironment.get_dump = get_dump

	env['HELP']=0
	if '--help' in sys.argv or '-h' in sys.argv or 'help' in sys.argv: env['HELP']=1
	if env['HELP']:
		p=env.pprint
		p('BOLD','*** Instructions ***')
		p('BOLD','--------------------')
		p('BOLD','* scons           ','to compile')
		p('BOLD','* scons -j4       ','to compile with several instances')
		p('BOLD','* scons install   ','to compile and install')
		p('BOLD','* scons -c install','to uninstall')
		p('BOLD','\n*** Generic options ***')
		p('BOLD','--------------------')
		p('BOLD','* debug        ','debug=1 (-g) or debug=full (-g3, slower) else use environment CXXFLAGS, or -O by default')
		p('BOLD','* prefix       ','the installation path')
		p('BOLD','* extraincludes','a list of paths separated by ":"')
		p('BOLD','* scons configure debug=full prefix=/usr/local extraincludes=/tmp/include:/usr/local')
		p('BOLD','* scons install prefix=/opt/local DESTDIR=/tmp/blah\n')
		return
	
	## Global cache directory
	# Put all project files in it so a rm -rf cache will clean up the config
	if not env.has_key('CACHEDIR'): env['CACHEDIR'] = env.join(os.getcwd(),'/cache/')
	if not os.path.isdir(env['CACHEDIR']): os.mkdir(env['CACHEDIR'])
	
	## SCons cache directory
	# This avoids recompiling the same files over and over again: 
	# very handy when working with cvs
	if os.getuid() != 0: env.CacheDir(os.getcwd()+'/cache/objects')

	#  Avoid spreading .sconsign files everywhere - keep this line
	env.SConsignFile(env['CACHEDIR']+'/scons_signatures')
	
	def makeHashTable(args):
		table = { }
		for arg in args:
			if len(arg) > 1:
				lst=arg.split('=')
				if len(lst) < 2: continue
				key=lst[0]
				value=lst[1]
				if len(key) > 0 and len(value) >0: table[key] = value
 		return table

	env['ARGS']=makeHashTable(sys.argv)

	SConsEnvironment.Chmod = SCons.Action.ActionFactory(os.chmod, lambda dest, mode: 'Chmod("%s", 0%o)' % (dest, mode))

	## Special trick for installing rpms ...
	env['DESTDIR']=''
	if 'install' in sys.argv:
		dd=''
		if os.environ.has_key('DESTDIR'): dd=os.environ['DESTDIR']
		if not dd:
			if env['ARGS'] and env['ARGS'].has_key('DESTDIR'): dd=env['ARGS']['DESTDIR']
		if dd:
			env['DESTDIR']=dd
			env.pprint('CYAN','** Enabling DESTDIR for the project ** ',env['DESTDIR'])

	## install symlinks for shared libraries properly
	env['INSTALL'] = copy_bksys

	## Use the same extension .o for all object files
	env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME'] = 1

	## no colors
	if os.environ.has_key('NOCOLORS'): env['NOCOLORS']=1

	## load the options
	cachefile=env['CACHEDIR']+'generic.cache.py'
	opts = Options(cachefile)
	opts.AddOptions(
		( 'GENCCFLAGS', 'C flags' ),
		( 'BKS_DEBUG', 'debug level: full, trace, or just something' ),
                ( 'GENCXXFLAGS', 'additional cxx flags for the project' ),
		( 'GENLINKFLAGS', 'additional link flags' ),
		( 'PREFIX', 'prefix for installation' ),
		( 'EXTRAINCLUDES', 'extra include paths for the project' ),
		( 'ISCONFIGURED', 'is the project configured' ),
	)
	opts.Update(env)
	
	# Use this to avoid an error message 'how to make target configure ?'
	env.Alias('configure', None)

	# Check if the following command line arguments have been given
	# and set a flag in the environment to show whether or not it was
	# given.
	if 'install' in sys.argv: env['_INSTALL']=1
	else: env['_INSTALL']=0
	if 'configure' in sys.argv: env['_CONFIGURE']=1
	else: env['_CONFIGURE']=0

	# Configure the environment if needed
	if not env['HELP'] and (env['_CONFIGURE'] or not env.has_key('ISCONFIGURED')):
		# be paranoid, unset existing variables
		for var in ['BKS_DEBUG', 'GENCXXFLAGS', 'GENCCFLAGS', 'GENLINKFLAGS', 'PREFIX', 'EXTRAINCLUDES', 'ISCONFIGURED', 'EXTRAINCLUDES']:
			if env.has_key(var): env.__delitem__(var)

		if env['ARGS'].get('debug', None):
			env['BKS_DEBUG'] = env['ARGS'].get('debug', None)
			env.pprint('CYAN','** Enabling debug for the project **')
		else:
			if os.environ.has_key('CXXFLAGS'):
				# user-defined flags (gentooers will be elighted)
				env['GENCXXFLAGS'] = SCons.Util.CLVar( os.environ['CXXFLAGS'] )
				env.Append( GENCXXFLAGS = ['-DNDEBUG', '-DNO_DEBUG'] )
			else:
				# SK: JNI module does not work with -O2 and -Os
				env.Append(GENCXXFLAGS = ['-O', '-DNDEBUG', '-DNO_DEBUG'])

		if os.environ.has_key('CFLAGS'): env['GENCCFLAGS'] = SCons.Util.CLVar( os.environ['CFLAGS'] )

		## FreeBSD settings (contributed by will at freebsd dot org)
		if os.uname()[0] == "FreeBSD":
			if os.environ.has_key('PTHREAD_LIBS'):
				env.AppendUnique( GENLINKFLAGS = SCons.Util.CLVar( os.environ['PTHREAD_LIBS'] ) )
		        else:
				syspf = os.popen('/sbin/sysctl kern.osreldate')
				osreldate = int(syspf.read().split()[1])
				syspf.close()
				if osreldate < 500016:
					env.AppendUnique( GENLINKFLAGS = ['-pthread'])
					env.AppendUnique( GENCXXFLAGS = ['-D_THREAD_SAFE'])
				elif osreldate < 502102:
					env.AppendUnique( GENLINKFLAGS = ['-lc_r'])
					env.AppendUnique( GENCXXFLAGS = ['-D_THREAD_SAFE'])
				else:
					env.AppendUnique( GENLINKFLAGS = ['-pthread'])

		# User-specified prefix
		if env['ARGS'].has_key('prefix'):
			env['PREFIX'] = os.path.abspath( env['ARGS'].get('prefix', '') )
			env.pprint('CYAN','** installation prefix for the project set to:',env['PREFIX'])

		# User-specified include paths
		env['EXTRAINCLUDES'] = env['ARGS'].get('extraincludes', None)
		if env['EXTRAINCLUDES']:
			env.pprint('CYAN','** extra include paths for the project set to:',env['EXTRAINCLUDES'])

		env['ISCONFIGURED']=1

		# And finally save the options in the cache
		opts.Save(cachefile, env)

	def bksys_install(lenv, subdir, files, destfile=None, perms=None):
		""" Install files on 'scons install' """
		if not env['_INSTALL']: return
		basedir = env['DESTDIR']
		install_list=None
		if not destfile: install_list = env.Install(lenv.join(basedir,subdir), lenv.make_list(files))
		elif subdir:     install_list = env.InstallAs(lenv.join(basedir,subdir,destfile), lenv.make_list(files))
		else:            install_list = env.InstallAs(lenv.join(basedir,destfile), lenv.make_list(files))
		if perms and install_list: lenv.AddPostAction(install_list, lenv.Chmod(install_list, perms))
		env.Alias('install', install_list)
		return install_list

	def build_la_file(target, source, env):
		""" Writes a .la file, used by libtool """
		dest=open(target[0].path, 'w')
		sname=source[0].name
		dest.write("# Generated by ltmain.sh - GNU libtool 1.5.18 - (pwn3d by bksys)\n#\n#\n")
		if len(env['BKSYS_VNUM'])>0:
			vnum=env['BKSYS_VNUM']
			nums=vnum.split('.')
			src=source[0].name
			name = src.split('so.')[0] + 'so'
			strn = src+" "+name+"."+str(nums[0])+" "+name
			dest.write("dlname='%s'\n" % (name+'.'+str(nums[0])) )
			dest.write("library_names='%s'\n" % (strn) )
		else:
			dest.write("dlname='%s'\n" % sname)
			dest.write("library_names='%s %s %s'\n" % (sname, sname, sname) )
		dest.write("old_library=''\ndependency_libs=''\ncurrent=0\n")
		dest.write("age=0\nrevision=0\ninstalled=yes\nshouldnotlink=no\n")
		dest.write("dlopen=''\ndlpreopen=''\n")
		dest.write("libdir='%s'" % env['BKSYS_DESTDIR'])
		dest.close()
		return 0

	def string_la_file(target, source, env):
		print "building '%s' from '%s'" % (target[0].name, source[0].name)
	la_file = env.Action(build_la_file, string_la_file, ['BKSYS_VNUM', 'BKSYS_DESTDIR'])
	env['BUILDERS']['LaFile'] = env.Builder(action=la_file,suffix='.la',src_suffix=env['SHLIBSUFFIX'])

	## Function for building shared libraries
	def bksys_shlib(lenv, ntarget, source, libdir, libprefix='lib', vnum='', noinst=None):
		""" Install a shared library.
		
		Installs a shared library, with or without a version number, and create a
		.la file for use by libtool.
		
		If library version numbering is to be used, the version number
		should be passed as a period-delimited version number (e.g.
		vnum = '1.2.3').  This causes the library to be installed
		with its full version number, and with symlinks pointing to it.
		
		For example, for libfoo version 1.2.3, install the file
		libfoo.so.1.2.3, and create symlinks libfoo.so and
		libfoo.so.1 that point to it.
		"""
		# parameter can be a list
		if type(ntarget) is types.ListType: target=ntarget[0]
		else: target=ntarget

		thisenv = lenv.Copy() # copying an existing environment is cheap
		thisenv['BKSYS_DESTDIR']=libdir
		thisenv['BKSYS_VNUM']=vnum
		thisenv['SHLIBPREFIX']=libprefix

		if len(vnum)>0:
			thisenv['SHLIBSUFFIX']='.so.'+vnum
			thisenv.Depends(target, thisenv.Value(vnum))
			num=vnum.split('.')[0]
			lst=target.split('/')
			tname=lst[len(lst)-1]
			libname=tname.split('.')[0]
			thisenv.AppendUnique(LINKFLAGS = ["-Wl,--soname=%s.so.%s" % (libname, num)] )

		# Fix against a scons bug - shared libs and ordinal out of range(128)
		if type(source) is types.ListType:
			src2=[]
			for i in source: src2.append( str(i) )
			source=src2

		library_list = thisenv.SharedLibrary(target, source)
		lafile_list  = thisenv.LaFile(target, library_list)

		## Install the libraries automatically
		if not thisenv.has_key('NOAUTOINSTALL') and not noinst:
			thisenv.bksys_install(libdir, library_list)
			thisenv.bksys_install(libdir, lafile_list)	

		## Handle the versioning
		if len(vnum)>0:
			nums=vnum.split('.')
			symlinkcom = ('cd $TARGET.dir && rm -f $TARGET.name && ln -s $SOURCE.name $TARGET.name')
			tg = target+'.so.'+vnum
			nm1 = target+'.so'
			nm2 = target+'.so.'+nums[0]
			thisenv.Command(nm1, tg, symlinkcom)
			thisenv.Command(nm2, tg, symlinkcom)
			thisenv.bksys_install(libdir, nm1)
			thisenv.bksys_install(libdir, nm2)
		return library_list

	# Declare scons scripts to process
	def subdirs(lenv, folderlist):
		flist=lenv.make_list(folderlist)
		for i in flist:
			lenv.SConscript(lenv.join(i, 'SConscript'))
		# take all objects - warn those who are not already executed
		if lenv.has_key('USE_THE_FORCE_LUKE'):
			for ke in lenv['USE_THE_FORCE_LUKE']:
				if ke.executed: continue
				#lenv.pprint('GREEN',"you forgot to execute object "+ke.target)
				ke.lockworkdir()
				ke.execute()
				ke.unlockworkdir()

        def link_local_shlib(lenv, str):
                """ Links against a shared library made in the project """
                lst = lenv.make_list(str)
		for file in lst:
			import re
			reg=re.compile("(.*)/lib(.*).(la|so)$")
			result=reg.match(file)
			if not result:
				reg = re.compile("(.*)/lib(.*).(la|so)\.(.)")
				result=reg.match(file)
				if not result:
					print "Unknown la file given "+file
					continue
				dir  = result.group(1)
				link = result.group(2)
			else:
				dir  = result.group(1)
				link = result.group(2)

			lenv.AppendUnique(LIBS = [link])
			lenv.PrependUnique(LIBPATH = [dir])

        def link_local_staticlib(lenv, str):
                """ Links against a shared library made in the project """
                lst = lenv.make_list(str)
		for file in lst:
			import re
			reg = re.compile("(.*)/(lib.*.a)")
			result = reg.match(file)
			if not result:
				print "Unknown archive file given "+file
				continue
			f=SCons.Node.FS.default_fs.File(file)
			lenv.Append(LINKFLAGS=[f.path])

	def set_build_dir(lenv, dirs, buildto):
		lenv.SetOption('duplicate', 'soft-copy')
		lenv['_BUILDDIR_']=buildto
		ldirs=lenv.make_list(dirs)
		for dir in ldirs:
			lenv.BuildDir(buildto+os.path.sep+dir, dir)

	#valid_targets = "program shlib kioslave staticlib".split()
        SConsEnvironment.bksys_install = bksys_install
	SConsEnvironment.bksys_shlib   = bksys_shlib
	SConsEnvironment.subdirs       = subdirs
	SConsEnvironment.link_local_shlib = link_local_shlib
	SConsEnvironment.link_local_staticlib = link_local_staticlib
	SConsEnvironment.genobj=genobj
	SConsEnvironment.set_build_dir=set_build_dir

	if env.has_key('GENCXXFLAGS'):  env.AppendUnique( CPPFLAGS = env['GENCXXFLAGS'] )
	if env.has_key('GENCCFLAGS'):   env.AppendUnique( CCFLAGS = env['GENCCFLAGS'] )
	if env.has_key('GENLINKFLAGS'): env.AppendUnique( LINKFLAGS = env['GENLINKFLAGS'] )

        if env.has_key('BKS_DEBUG'):
                if (env['BKS_DEBUG'] == "full"):
                        env.AppendUnique(CXXFLAGS = ['-DDEBUG', '-g3', '-Wall'])
                elif (env['BKS_DEBUG'] == "trace"):
                        env.AppendUnique(
                                LINKFLAGS=env.Split("-lmrwlog4cxxconfiguration -lmrwautofunctiontracelog4cxx -finstrument-functions"),
                                CXXFLAGS=env.Split("-DDEBUG -Wall -finstrument-functions -g3 -O0"))
                else:
                        env.AppendUnique(CXXFLAGS = ['-DDEBUG', '-g', '-Wall'])

	if env.has_key('EXTRAINCLUDES'):
		if env['EXTRAINCLUDES']:
			incpaths = []
			for dir in str(env['EXTRAINCLUDES']).split(':'): incpaths.append( dir )
			env.Append(CPPPATH = incpaths)

	env.Export('env')
