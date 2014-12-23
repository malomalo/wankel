require 'test_helper'
require 'stringio'

class Wankel::SaxEncoderTest < Minitest::Test
  
  def setup
    @output = StringIO.new
    @encoder = Wankel::SaxEncoder.new(@output)
  end
  
  def assert_output(value)
    assert_equal(value, @output.string)
  end
  
  test 'default inherited from Wankel' do
      encoder = Wankel::SaxEncoder.new(StringIO.new)
      assert_equal(Wankel::DEFAULTS, encoder.instance_variable_get("@options"))
  end
  
  test 'default options merged with options inherited from Wankel && Class' do
      encoder = Wankel::SaxEncoder.new(StringIO.new, :hello => true, :metoo => false)
      assert_equal(Wankel::DEFAULTS.merge({:hello => true, :metoo => false}), encoder.instance_variable_get("@options"))
  end

  test "number" do
    @encoder.number(1234)
    @encoder.flush
    
    assert_output("1234")
  end
  
  test "string" do
    @encoder.string("hello world")
    @encoder.flush
    
    assert_output('"hello world"')
  end
    
  test "null" do
    @encoder.null
    @encoder.flush

    assert_output("null")
  end
  
  test "boolean(false)" do
    @encoder.boolean(false)
    @encoder.flush
    
    assert_output("false")
  end
  
  test "boolean(true)" do
    @encoder.boolean(true)
    @encoder.flush

    assert_output("true")
  end
      
  test "map_open" do
    @encoder.map_open
    @encoder.flush
    
    assert_output("{")
  end
  
  test "map_close" do
    @encoder.map_open
    @encoder.map_close
    @encoder.flush
    
    assert_output("{}")
  end
  
  test "array_open" do
    @encoder.array_open
    @encoder.flush
    
    assert_output("[")
  end
  
  test "array_close" do
    @encoder.array_open
    @encoder.array_close
    @encoder.flush
    
    assert_output("[]")
  end

  test "value(NUMBER)" do
    @encoder.value(123)
    @encoder.flush
    
    assert_output("123")
  end
  
  test "value(STRING)" do
    @encoder.value("hello")
    @encoder.flush
    
    assert_output("\"hello\"")
  end
  
  test "value(NIL)" do
    @encoder.value(nil)
    @encoder.flush
    
    assert_output("null")
  end
  
  test "value(BOOLEAN)" do
    @encoder.value(true)
    @encoder.flush
    
    assert_output("true")
    
    @output.rewind
    @encoder.value(false)
    @encoder.flush
    
    assert_output("true")
  end
  
  test "value(ARRAY)" do
    @encoder.value([])
    @encoder.flush
    
    assert_output("[]")
  end
  
  test "value(ARRAY_WITH_VALUES)" do
    @encoder.value([1, "hello", nil, false, true, 45.21, {:key => 'val'}])
    @encoder.flush
    
    assert_output("[1,\"hello\",null,false,true,45.21,{\"key\":\"val\"}]")
  end
  
  test "value(HASH)" do
    @encoder.value(:one => 'one', :two => 2, :three => true, :four => nil, 5 => [45.21])
    @encoder.flush
    
    assert_output('{"one":"one","two":2,"three":true,"four":null,"5":[45.21]}')
  end
  
end
__END__
test "map_open" do
test "map_close" do

