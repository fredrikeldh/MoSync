#!/usr/bin/ruby

require File.expand_path('../../rules/work.rb')

class DefaultSkinTask < FileTask
	def initialize
		@prerequisites = Dir.glob('*.png') +  Dir.glob('*.mof')
		@prerequisites.collect! do |file| FileTask.new(file) end
		super('../../libs/MAUI-revamp/DefaultSkin.h')
	end
	def fileExecute
		sh 'ruby Generator.rb'
	end
end

DefaultSkinTask.new
Works.run
