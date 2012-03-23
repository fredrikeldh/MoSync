# Copyright (C) 2009 Mobile Sorcery AB
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License, version 2, as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# load local_config.rb, if it exists.
lc = "#{File.dirname(__FILE__)}/local_config.rb"
require lc if(File.exists?(lc))

default_const(:PRINT_FLAG_CHANGES, false)

# This module contains functions for saving and comparing compile or link flags.
# execFlags and flagsNeeded require the function "cFlags".
module FlagsChanged
	# Call from initialize.
	def initFlags
		# save cflags to disk, for use as dependency.
		# but first read it back from disk, if existing.
		@FLAGSFILE = @NAME + ".flags"
		if(File.exists?(@FLAGSFILE)) then
			@OLDFLAGS = open(@FLAGSFILE) { |f| f.read }
		end
		#puts "Oldflags: #{@OLDFLAGS.inspect}"
	end

	# Call from execute.
	def execFlags
		if(@OLDFLAGS != cFlags) then
			open(@FLAGSFILE, 'w') { |f| f.write(cFlags) }
			@OLDFLAGS = cFlags
		end
	end

	def needed?(log = true)
		return true if(super(log))
		return flagsNeeded?(log)
	end

	# Call from needed?.
	def flagsNeeded?(log=true)
		#puts "Oldflags: #{@OLDFLAGS.inspect} newflags: #{@FLAGS.inspect}"
		if(@OLDFLAGS != cFlags)
			puts "Because the flags have changed:" if(log)
			puts "Old: #{@OLDFLAGS}" if(log && PRINT_FLAG_CHANGES)
			puts "New: #{cFlags}" if(log && PRINT_FLAG_CHANGES)
			return true
		end
		return false
	end
end
