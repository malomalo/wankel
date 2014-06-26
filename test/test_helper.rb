if ENV['COVERALLS_REPO_TOKEN']
  require 'simplecov'
  SimpleCov.start do
    add_filter "/test/"
  end
end

# To make testing/debugging easier, test within this source tree versus an
# installed gem
dir = File.dirname(__FILE__)
root = File.expand_path(File.join(dir, '..'))
lib = File.expand_path(File.join(root, 'lib'))

$LOAD_PATH << lib

require "minitest/autorun"
require 'minitest/unit'
require 'minitest/reporters'
require "mocha/setup"
require 'wankel'

Minitest::Reporters.use! Minitest::Reporters::SpecReporter.new

# File 'lib/active_support/testing/declarative.rb', somewhere in rails....
class Minitest::Test
  def self.test(name, &block)
    test_name = "test_#{name.gsub(/\s+/,'_')}".to_sym
    defined = instance_method(test_name) rescue false
    raise "#{test_name} is already defined in #{self}" if defined
    if block_given?
      define_method(test_name, &block)
    else
      define_method(test_name) do
        flunk "No implementation provided for #{name}"
      end
    end
  end
end