# load local_config.rb, if it exists.
lc = "#{File.dirname(__FILE__)}/local_config.rb"
require lc if(File.exists?(lc))

require "#{File.dirname(__FILE__)}/util.rb"

# These are default values. Users should not modify them.
# Instead, users should create local_config.rb and put their settings there.

default_const(:PRINT_FLAG_CHANGES, true)
default_const(:USE_COMPILER_VERSION_IN_BUILDDIR_NAME, true)
default_const(:EXIT_ON_ERROR, true)
default_const(:PRINT_WORKING_DIRECTORY, false)
default_const(:CONFIG_PRINT_FILETASK_BACKTRACE, false)

default_const(:CONFIG_CCOMPILE_DEFAULT, 'release')

# If true, the output of LibWorks and DllWorks will be located in /rules/../build/,
# for use in LOCAL_LIBS and LOCAL_DLLS.
# If false, LOCAL_LIBS and LOCAL_DLLS are not allowed.
# This should never be overridden by local_config.rb.
default_const(:CONFIG_HAVE_COMMON_BUILDDIR, true)

# If true, implement Work.invoke_subdir by spawning a new process.
# If false, load subdir's workfile in current context.
default_const(:CONFIG_SUBDIR_RELOAD, false)

# After all tasks are completed, make sure that none of them are still needed.
# This takes significant time, so should only be turned on to debug the workfile system.
default_const(:CONFIG_CHECK_TASK_INTEGRITY, false)

default_const(:MSBUILD_PATH, nil)

#require "#{File.dirname(__FILE__)}/gccModule.rb"

# The above require would cause a broken requirement loop.
# By defining the module here as empty, its name is available for use,
# but attempting to use its contents will cause errors.
module GccCompilerModule; end

default_const(:DefaultCCompilerModule, GccCompilerModule)
