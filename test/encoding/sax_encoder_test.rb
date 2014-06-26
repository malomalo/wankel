require 'stringio'

class Wankel::SaxEncoderTest < Minitest::Test
  
  test 'default inherited from Wankel' do
      encoder = Wankel::SaxEncoder.new(StringIO.new)
      assert_equal(Wankel::DEFAULTS, encoder.instance_variable_get("@options"))
  end
  
  test 'default options merged with options inherited from Wankel && Class' do
      encoder = Wankel::SaxEncoder.new(StringIO.new, :hello => true, :metoo => false)
      assert_equal(Wankel::DEFAULTS.merge({:hello => true, :metoo => false}), encoder.instance_variable_get("@options"))
  end

  test "number" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.number(1234)
    encoder.complete
    assert_equal("1234", output.string)
  end
  
  test "string" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.string("hello world")
    encoder.complete
    assert_equal('"hello world"', output.string)
  end
    
  test "null" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.null
    encoder.complete
    assert_equal("null", output.string)
  end
  
  test "bool(false)" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.bool(false)
    encoder.complete
    assert_equal("false", output.string)
  end
  
  test "bool(true)" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.bool(true)
    encoder.complete
    assert_equal("true", output.string)
  end
      
  test "map_open" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.map_open
    encoder.complete
    assert_equal("{", output.string)
  end
  
  test "map_close" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.map_open
    encoder.map_close
    encoder.complete
    assert_equal("{}", output.string)
  end
  
  test "array_open" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.array_open
    encoder.complete
    assert_equal("[", output.string)
  end
  
  test "array_close" do
    output = StringIO.new
    encoder = Wankel::SaxEncoder.new(output)
    encoder.array_open
    encoder.array_close
    encoder.complete
    assert_equal("[]", output.string)
  end

end