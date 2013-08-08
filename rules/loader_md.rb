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

require "#{File.dirname(__FILE__)}/work.rb"

# A dummy FileTask which allows header files to be removed without
# causing errors in the Work system.
# Removing header files without modifying the source files that include them
# will still cause compile errors.
class HeaderFileTask < FileTask
	def fileExecute
	end
end


class MakeDependLoader
	SPACE_MARK = "__&NBSP;__"

	# Load makefile dependencies in +fn+.
	# Return an array of Tasks.
	def MakeDependLoader.load(fn, target)
		res = nil
		target = target.gsub(' ', SPACE_MARK)
		open(fn) do |mf|
			lines = mf.read
			lines.gsub!(/\\ /, SPACE_MARK)
			lines.gsub!(/#[^\n]*\n/m, "")
			lines.gsub!(/\\\n/, ' ')
			lines.split("\n").each do |line|
				if(res) then
					error "MD: too many logical lines in file '#{fn}'"
				end
				res = process_line(fn, line, target)
			end
		end
		if(res == nil)
			res = []
		end
		return res
	end

	private

	# Process one logical line of makefile data.
	def MakeDependLoader.process_line(fn, line, target)
		index = line.index(': ')
		md = line[0, index].strip
		if(!filenamesEqual(target, md)) then
			puts "Warning: MD target mismatch in #{fn}. target: '#{target}' md: '#{md}'"
		end
		args = line[index+1, line.length]
		return [] if args.nil?
		depNames = args.split.map { |a| respace(a) }
		depTasks = depNames.collect { |d|
			HeaderFileTask.new(d)
		}
		return depTasks
	end

	def MakeDependLoader.respace(str)
		str.gsub(/#{SPACE_MARK}/, ' ')
	end
end
