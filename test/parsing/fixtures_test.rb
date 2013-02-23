# encoding: UTF-8
require 'test_helper'

class Wankel::ParsingJSONFixturesTest < ::Test::Unit::TestCase
  
  fixtures = File.join(File.dirname(__FILE__), 'fixtures/*.json')
  passed, failed = Dir[fixtures].partition { |f| f['pass'] }
  PASSED = passed.inject([]) { |a, f| a << [ f, File.read(f) ] }.sort
  FAILED = failed.inject([]) { |a, f| a << [ f, File.read(f) ] }.sort

  FAILED.each do |name, source|
    test "should not be able to parse #{File.basename(name)} as an IO" do
      assert_raise Wankel::ParseError do
        Wankel.parse(StringIO.new(source))
      end
    end
  end

  FAILED.each do |name, source|
    test "should not be able to parse #{File.basename(name)} as a string" do
      assert_raise Wankel::ParseError do
        Wankel.parse(source)
      end
    end
  end

  PASSED.each do |name, source|
    test "should be able to parse #{File.basename(name)} as an IO" do
      assert_nothing_raised Wankel::ParseError do
        Wankel.parse(StringIO.new(source))
      end
    end
  end

  PASSED.each do |name, source|
    test "should be able to parse #{File.basename(name)} as a string" do
      assert_nothing_raised Wankel::ParseError do
        Wankel.parse(source)
      end
    end
  end
  
end