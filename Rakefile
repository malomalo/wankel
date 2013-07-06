require "rake/extensiontask"
require "rake/testtask"
require "rubygems/package_task"
require "rdoc/task"
require 'fileutils'

if ENV['COVERALLS_REPO_TOKEN']
  require 'simplecov'
  require 'coveralls'
  require 'coveralls/rake/task'
  Coveralls::RakeTask.new
end

spec = Gem::Specification.load("wankel.gemspec")

# Setup compile tasks
Rake::ExtensionTask.new do |ext|
  ext.gem_spec = spec
  ext.name = 'wankel'
  ext.ext_dir = 'ext/wankel'
  ext.lib_dir = 'lib/wankel'
  ext.config_options << '--coverage' if ENV['COVERALLS_REPO_TOKEN']
end

# Test Task
Rake::TestTask.new do |t|
    t.libs << 'lib' << 'test'
    t.test_files = FileList['test/**/*_test.rb']
    # t.warning = true
    # t.verbose = true
end
Rake::Task[:test].prerequisites << :compile

task :c_coverage do
  out_dir = "tmp/#{RUBY_PLATFORM}/wankel/#{RUBY_VERSION}"
  src_dir = 'ext/wankel'
  src_files = FileList["#{src_dir}/*.c"]
  FileUtils.cp(src_files, out_dir)
  src_files.map! { |f| File.basename(f) }
  `cd "#{out_dir}" && gcov #{src_files.join(" ")}`
  raise 'gcov error' if $? != 0

  data = SimpleCov::ResultMerger.resultset
  
  src_files.each do |f|
    file_data = File.read("#{out_dir}/#{f}.gcov").split("\n").map{|l| l.gsub(/\s+/,' ').split(' ') }
    file_data = file_data.select{|l| l[1].sub(/:.*$/,'').to_i != 0 }
    file_data.map! do |l|
      case l[0]
      when '-:'
        nil
      when '#####:'
        0
      else
        l[0].sub(/:.*$/,'').to_i
      end
    end
    
    data["Unit Tests"]["coverage"]["#{File.dirname(__FILE__)}/#{src_dir}/#{f}"] = file_data
    
    SimpleCov::ResultMerger.store_result(data)
  end
end