# encoding: UTF-8
require 'test_helper'

class Wankel::OneOffParseTest < Minitest::Test
  
  test "should parse 23456789012E666 as Infinity" do
    infinity = (1.0/0)
    assert_equal({'key' => infinity}, Wankel.parse('{"key": 23456789012E999}'))
  end
  
  test "should not parse JSON with a comment, with :allow_comments set to false" do
    assert_raises Wankel::ParseError do
      Wankel.parse('{"key": /* this is a comment */ "value"}', :allow_comments => false)
    end
  end
  
  test "should not parse invalid UTF8 with :validate_utf8 set to true" do
    assert_raises Wankel::ParseError do
      Wankel.parse("[\"#{"\201\203"}\"]", :validate_utf8 => true)
    end
  end

  test "should parse invalid UTF8 with :validate_utf8 set to false" do
    Wankel.parse("[\"#{"\201\203"}\"]", :validate_utf8 => false)
  end
  
  test "should not allow trailing garbage with :allow_trailing_garbage set to false" do
    assert_raises Wankel::ParseError do
      Wankel.parse('{"key": "value"}gar', :allow_trailing_garbage => false)
    end
  end
  
  test "should allow trailing garbage with :allow_trailing_garbage set to true" do
    assert_equal(
      {"key" => "value"},
      Wankel.parse('{"key": "value"}gar', :allow_trailing_garbage => true)
    )
  end
  
  test "should parse using it's class method, from an IO" do
    io = StringIO.new('{"key": 1234}')
    assert_equal({'key' => 1234}, Wankel.parse(io))
  end

  test "should parse using it's class method, from a string with symbolized keys" do
    assert_equal({:key => 1234}, Wankel.parse('{"key": 1234}', :symbolize_keys => true))
  end
  
  test "should parse using it's class method, from a utf-8 string with multibyte characters, with symbolized keys" do
    assert_equal({:"日本語" => 1234}, Wankel.parse('{"日本語": 1234}', :symbolize_keys => true))
  end

  test "should parse using it's class method, from a string" do
    assert_equal({"key" => 1234}, Wankel.parse('{"key": 1234}'))
  end

  test "should parse numbers greater than 2,147,483,648" do
    assert_equal({"id" => 2147483649}, Wankel.parse("{\"id\": 2147483649}"))
    assert_equal({"id" => 5687389800}, Wankel.parse("{\"id\": 5687389800}"))
    assert_equal({"id" => 1046289770033519442869495707521600000000}, Wankel.parse("{\"id\": 1046289770033519442869495707521600000000}"))
  end

  test "should return strings and hash keys in utf-8 if Encoding.default_internal is nil" do
    Encoding.default_internal = nil
    assert_equal(Encoding.find('utf-8'), Wankel.parse('{"key": "value"}').keys.first.encoding)
    assert_equal(Encoding.find('utf-8'), Wankel.parse('{"key": "value"}').values.first.encoding)
  end

  test "should return strings and hash keys encoded as specified in Encoding.default_internal if it's set" do
    Encoding.default_internal = Encoding.find('utf-8')
    assert_equal(Encoding.default_internal, Wankel.parse('{"key": "value"}').keys.first.encoding)
    assert_equal(Encoding.default_internal, Wankel.parse('{"key": "value"}').values.first.encoding)
    Encoding.default_internal = Encoding.find('us-ascii')
    assert_equal(Encoding.default_internal, Wankel.parse('{"key": "value"}').keys.first.encoding)
    assert_equal(Encoding.default_internal, Wankel.parse('{"key": "value"}').values.first.encoding)
  end

end