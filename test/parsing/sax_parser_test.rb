# encoding: UTF-8
require 'test_helper'

class TestParser < Wankel::SaxParser
  def on_map_start
  end
  def on_map_end
  end
  def on_map_key(key)
  end
  def on_null
  end
  def on_boolean(value)
  end
  def on_integer(i)
  end
  def on_double(d)
  end
  def on_string(s)
  end
  def on_array_start
  end
  def on_array_end
  end
end

class Wankel::SaxParserTest < Minitest::Test
  
  test 'default inherited from Wankel' do
      parser = TestParser.new
      parser.instance_variable_get("@options") == Wankel::DEFAULTS
  end
  
  test 'default inherited from Wankel && Class' do
      class TestParser2 < Wankel::SaxParser
        DEFAULTS = {:hello => true}
      end
      
      parser = TestParser2.new
      parser.instance_variable_get("@options") == Wankel::DEFAULTS.merge({:hello => true})
  end
  
  test 'default options merged with options inherited from Wankel && Class' do
      class TestParser3 < Wankel::SaxParser
        DEFAULTS = {:hello => true}
      end
      
      parser = TestParser3.new(:metoo => false)
      parser.instance_variable_get("@options") == Wankel::DEFAULTS.merge({:hello => true, :metoo => false})
  end
  
  test "#parse(STRING)" do
    parser = TestParser.new
    parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
  end
  
  test "#parse(IO)" do
    #TODO
    # parser = TestParser.new
    # parser.parse(StringIO.new("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}"))
  end
  
  test "#write(STRING) && #complete() instead of #parse(DATA)" do
    parser = TestParser.new
    parser.expects(:on_map_start).twice
    
    parser.write("{\"id\":")
    parser << " 2147483649,\"ke"
    parser.write("y\": [1,null,1.0,{}, true]}")
    parser.complete
  end
  
  test "on_map_start callback" do
      parser = TestParser.new
      parser.expects(:on_map_start).twice
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
  end

  test "on_map_end callback" do
      parser = TestParser.new
      parser.expects(:on_map_end).twice
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
  end
  
  test "on_map_key callback" do
      parser = TestParser.new
      parser.expects(:on_map_key).twice
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")

      # TODO when reseting parser avail reset
      parser = TestParser.new      
      parser.expects(:on_map_key).once.with('id')
      parser.parse("{\"id\": 2147483649}")
  end
  
  test "on_map_null callback" do
      parser = TestParser.new
      parser.expects(:on_null).once
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
  end
  
  test "on_map_boolean callback" do
      parser = TestParser.new
      parser.expects(:on_boolean).once.with(true)
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
      
      parser = TestParser.new
      parser.expects(:on_boolean).once.with(false)
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, false]}")
  end
  
  test "on_integer callback" do
      parser = TestParser.new
      parser.expects(:on_integer).twice
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
      
      parser = TestParser.new
      parser.expects(:on_integer).once.with(2147483649)
      parser.parse("{\"id\": 2147483649}")
  end
  
  test "on_double callback" do
      parser = TestParser.new
      parser.expects(:on_double).once.with(1.0)
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,1.0,{}, true]}")
  end

  test "on_string callback" do
      parser = TestParser.new
      parser.expects(:on_string).once.with("data")
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,\"data\",1.0,{}, true]}")
  end
  
  test "on_array_start callback" do
      parser = TestParser.new
      parser.expects(:on_array_start).once
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,\"data\",1.0,{}, true]}")
  end
  
  test "on_array_end callback" do
      parser = TestParser.new
      parser.expects(:on_array_end).once
      parser.parse("{\"id\": 2147483649,\"key\": [1,null,\"data\",1.0,{}, true]}")
  end
  
end