
module MAStd
def initMAStd
	if(@CONFIG=="debug")
		@EXTRA_CFLAGS = " -DMOSYNCDEBUG"
	end
	@SOURCE_FILES ||= []
	@SOURCE_FILES << '../MAStd/crtlib.s'
	@SOURCE_FILES << '../MAStd/mapip2_builtins.s'
	@SOURCE_FILES << '../MAStd/mastack.s'

	@REQUIREMENTS = [
		copyHeaderFile('.', '../libsupc++/new'),
		copyHeaderFile('.', '../libsupc++/macpp.h'),
		copyHeaderFile('ResCompiler', '../ResCompiler/ResCompiler.h'),
	]
end
end
