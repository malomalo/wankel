# encoding: UTF-8
require 'test_helper'

class Wankel::MultipleValuesTest < ::Test::Unit::TestCase

  test "parsing with :multiple_values returns an array of values" do
    assert_equal([true], Wankel.parse('true', :multiple_values => true))
  end
  
  test "parsing with :multiple_values => false returns the json object" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = Wankel.parse('[{"abc": 123},{"def": 456}]', :multiple_values => false)
    assert_equal(expected, result)
  end
  
  test "parsing with should raise a Wankel::ParseError error if multiple JSON strings were found when :multiple_values => false" do
    assert_raise Wankel::ParseError do
      result = Wankel.parse('[{"abc": 123}][{"def": 456}]', :multiple_values => false)
    end
  end
  
  test "parsing with :multiple_values with a single chunk" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = Wankel.parse('[{"abc": 123},{"def": 456}]', :multiple_values => true)
    assert_equal([expected], result)
  end
  
  test "parsing with :multiple_values with a single chunk as an IO" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = Wankel.parse(StringIO.new('[{"abc": 123},{"def": 456}]'), :multiple_values => true)
    assert_equal([expected], result)
  end
  
  test "parsing a single chunk, 3 times" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = Wankel.parse('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]', :multiple_values => true)
    assert_equal([expected,expected,expected], result)
  end
  
  test "parsing a single chunk, 3 times as an IO" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = Wankel.parse(StringIO.new('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]'), :multiple_values => true)
    assert_equal([expected,expected,expected], result)
  end
  
  test "instance.parse chunks with a block" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = []
    
    p = Wankel::Parser.new(:multiple_values => true)
    p.parse('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]') do |data|
      result << data
    end
    assert_equal([expected,expected], result)
  end
  
  test "instance.parse chunks with a block as an IO" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = []
    
    p = Wankel::Parser.new(:multiple_values => true)
    p.parse(StringIO.new('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]')) do |data|
      result << data
    end
    assert_equal([expected,expected], result)
  end
  
  test "class.parse chunks with a block" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = []

    Wankel.parse('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]', :multiple_values => true) do |data|
      result << data
    end
    assert_equal([expected,expected], result)
  end

  test "class.parse chunks with a block as an IO" do
    expected = [{"abc" => 123},{"def" => 456}]
    result = []

    Wankel.parse(StringIO.new('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]'), :multiple_values => true) do |data|
      result << data
    end
    assert_equal([expected,expected], result)
  end
  
  test "parsing twitter_stream.json and fire callback 430 times" do
    path = File.expand_path(File.dirname(__FILE__) + '/../../benchmark/subjects/twitter_stream.json')
    json = File.new(path, 'r')
    calls = 0
    
    Wankel.parse(json, :multiple_values => true) do |data|
      calls += 1
    end
    
    assert_equal(430, calls)
  end
  
end