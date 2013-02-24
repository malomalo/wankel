require "rake/extensiontask"
require "rake/testtask"
require "rubygems/package_task"
require "rdoc/task"


spec = Gem::Specification.load("wankel.gemspec")

# Setup compile tasks
Rake::ExtensionTask.new do |ext|
  ext.gem_spec = spec
  ext.name = 'wankel'
  ext.ext_dir = 'ext/wankel'
  ext.lib_dir = 'lib/wankel'
end

# Test Task
Rake::TestTask.new do |t|
    t.libs << 'lib' << 'test'
    t.test_files = FileList['test/**/*_test.rb']
    # t.warning = true
    # t.verbose = true
end
Rake::Task[:test].prerequisites << :compile