#!/usr/bin/ruby

require File.expand_path('../../rules/host.rb')
require File.expand_path('../../rules/mosync_util.rb')

if(HOST == :linux || HOST == :darwin)
	require File.expand_path('../../rules/cLib.rb')
	c = LibWork
else
	require File.expand_path('../../rules/cDll.rb')
	c = DllWork
end

c.new do
	@SOURCES = ["amr_nb/common/src","amr_nb/dec/src","common/dec/src","amr_wb/common/src","amr_wb/dec/src"]
	@EXTRA_INCLUDES = ["amr_nb/common/include","oscl","amr_nb/dec/include","common/dec/include","amr_wb/dec/include"]
	@EXTRA_CPPFLAGS = " -DGSM_AMR_EXPORTS -DC_EQUIVALENT -DWMOPS=0 -Wno-undef"
	@NAME = "gsm_amr"

	@INSTALLDIR = mosyncdir + '/bin' if(self.class == DllWork)
end

Works.run
